#include <csignal>
#include <pqrs/cf_run_loop_thread.hpp>
#include <pqrs/osx/iokit_hid_device.hpp>
#include <pqrs/osx/iokit_hid_manager.hpp>
#include <unordered_map>

namespace {
class hid_report_handler final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  // Signals (invoked from the shared dispatcher thread)

  nod::signal<void(IOHIDReportType type, uint32_t report_id, std::shared_ptr<std::vector<uint8_t>>)> report_arrived;

  // Methods

  hid_report_handler(IOHIDDeviceRef device) : dispatcher_client(),
                                              opened_(false),
                                              scheduled_(false) {
    cf_run_loop_thread_ = std::make_unique<pqrs::cf_run_loop_thread>();

    hid_device_ = std::make_shared<pqrs::osx::iokit_hid_device>(device);

    if (auto manufacturer = hid_device_->find_string_property(CFSTR(kIOHIDManufacturerKey))) {
      device_name_ += *manufacturer + " ";
    }
    if (auto product = hid_device_->find_string_property(CFSTR(kIOHIDProductKey))) {
      device_name_ += *product;
    }

    // Resize report_buffer_

    size_t buffer_size = 32; // use this provisional value if we cannot get max input report size from device.
    if (auto size = hid_device_->find_max_input_report_size()) {
      buffer_size = static_cast<size_t>(*size);
    }

    report_buffer_.resize(buffer_size);

    // Register callback

    IOHIDDeviceRegisterInputReportCallback(device,
                                           &(report_buffer_[0]),
                                           report_buffer_.size(),
                                           static_input_report_callback,
                                           this);
  }

  ~hid_report_handler(void) {
    detach_from_dispatcher([this] {
      stop();
    });

    cf_run_loop_thread_->terminate();
    cf_run_loop_thread_ = nullptr;
  }

  std::shared_ptr<pqrs::osx::iokit_hid_device> get_hid_device(void) const {
    return hid_device_;
  }

  void start(void) {
    if (hid_device_) {
      if (auto device_ptr = hid_device_->get_device()) {
        if (!opened_) {
          pqrs::osx::iokit_return r = IOHIDDeviceOpen(*device_ptr, kIOHIDOptionsTypeNone);
          if (!r) {
            std::cerr << "IOHIDDeviceOpen:" << r << " " << device_name_ << std::endl;
            return;
          }
          opened_ = true;
        }

        if (!scheduled_) {
          IOHIDDeviceScheduleWithRunLoop(*device_ptr,
                                         cf_run_loop_thread_->get_run_loop(),
                                         kCFRunLoopCommonModes);
          scheduled_ = true;
        }
      }
    }
  }

  void stop(void) {
    if (hid_device_) {
      if (auto device_ptr = hid_device_->get_device()) {
        if (scheduled_) {
          IOHIDDeviceUnscheduleFromRunLoop(*device_ptr,
                                           cf_run_loop_thread_->get_run_loop(),
                                           kCFRunLoopCommonModes);
          scheduled_ = false;
        }

        if (opened_) {
          pqrs::osx::iokit_return r = IOHIDDeviceClose(*device_ptr, kIOHIDOptionsTypeNone);
          if (!r) {
            std::cerr << "IOHIDDeviceClose:" << r << " " << device_name_ << std::endl;
          }
          opened_ = true;
        }
      }
    }
  }

private:
  static void static_input_report_callback(void* context,
                                           IOReturn result,
                                           void* sender,
                                           IOHIDReportType type,
                                           uint32_t report_id,
                                           uint8_t* report,
                                           CFIndex report_length) {
    pqrs::osx::iokit_return r = result;
    if (!r) {
      std::cerr << "static_input_report_callback: " << r << std::endl;
      return;
    }

    auto self = static_cast<hid_report_handler*>(context);
    if (!self) {
      return;
    }

    self->input_report_callback(type, report_id, report, report_length);
  }

  void input_report_callback(IOHIDReportType type,
                             uint32_t report_id,
                             uint8_t* report,
                             CFIndex report_length) {
    auto r = std::make_shared<std::vector<uint8_t>>(report_length);
    memcpy(&((*r)[0]), report, report_length);

    enqueue_to_dispatcher([this, type, report_id, r] {
      report_arrived(type, report_id, r);
    });
  }

  std::unique_ptr<pqrs::cf_run_loop_thread> cf_run_loop_thread_;
  std::shared_ptr<pqrs::osx::iokit_hid_device> hid_device_;
  std::vector<uint8_t> report_buffer_;
  bool opened_;
  bool scheduled_;
  std::string device_name_;
};

class dump_hid_report final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  dump_hid_report(const dump_hid_report&) = delete;

  dump_hid_report(void) : dispatcher_client() {
    std::vector<pqrs::cf_ptr<CFDictionaryRef>> matching_dictionaries{
        pqrs::osx::iokit_hid_manager::make_matching_dictionary(
            pqrs::osx::iokit_hid_usage_page_generic_desktop,
            pqrs::osx::iokit_hid_usage_generic_desktop_keyboard),

        pqrs::osx::iokit_hid_manager::make_matching_dictionary(
            pqrs::osx::iokit_hid_usage_page_generic_desktop,
            pqrs::osx::iokit_hid_usage_generic_desktop_mouse),

        pqrs::osx::iokit_hid_manager::make_matching_dictionary(
            pqrs::osx::iokit_hid_usage_page_generic_desktop,
            pqrs::osx::iokit_hid_usage_generic_desktop_pointer),

        pqrs::osx::iokit_hid_manager::make_matching_dictionary(
            pqrs::osx::iokit_hid_usage_page_digitizer),
    };

    hid_manager_ = std::make_unique<pqrs::osx::iokit_hid_manager>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                                  matching_dictionaries);

    hid_manager_->device_matched.connect([this](auto&& registry_entry_id, auto&& device_ptr) {
      if (device_ptr) {
        auto handler = std::make_shared<hid_report_handler>(*device_ptr);
        hid_report_handlers_[registry_entry_id] = handler;

        auto manufacturer = handler->get_hid_device()->find_string_property(CFSTR(kIOHIDManufacturerKey));
        auto product = handler->get_hid_device()->find_string_property(CFSTR(kIOHIDProductKey));

        handler->report_arrived.connect([this, manufacturer, product](auto&& type,
                                                                      auto&& report_id,
                                                                      auto&& report) {
          report_arrived(manufacturer, product, type, report_id, report);
        });

        handler->start();
      }
    });

    hid_manager_->device_terminated.connect([this](auto&& registry_entry_id) {
      hid_report_handlers_.erase(registry_entry_id);
    });

    hid_manager_->async_start();
  }

  virtual ~dump_hid_report(void) {
    detach_from_dispatcher([this] {
      hid_manager_ = nullptr;
    });
  }

private:
  void report_arrived(std::optional<std::string> manufacturer,
                      std::optional<std::string> product,
                      IOHIDReportType type,
                      uint32_t report_id,
                      std::shared_ptr<std::vector<uint8_t>> report) const {
    // Logitech Unifying Receiver sends a lot of null report. We ignore them.
    if (manufacturer) {
      if (product) {
        if (*manufacturer == "Logitech" && *product == "USB Receiver") {
          if (report_id == 0) {
            return;
          }
        }
      }
    }

    std::cout << "report_length: " << report->size();
    std::cout << "  report_id: " << std::dec << report_id << std::endl;
    for (CFIndex i = 0; i < report->size(); ++i) {
      std::cout << "  key[" << i << "]: 0x" << std::hex << static_cast<int>((*report)[i]) << std::dec << std::endl;
    }
  }

  std::unique_ptr<pqrs::osx::iokit_hid_manager> hid_manager_;
  std::unordered_map<pqrs::osx::iokit_registry_entry_id, std::shared_ptr<hid_report_handler>> hid_report_handlers_;
};

auto global_wait = pqrs::make_thread_wait();
} // namespace

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  auto d = std::make_unique<dump_hid_report>();

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  d = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  std::cout << "finished" << std::endl;

  return 0;
}
