#include <csignal>
#include <pqrs/osx/iokit_hid_device.hpp>
#include <pqrs/osx/iokit_hid_manager.hpp>

namespace {
class hid_set_report final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  hid_set_report(const hid_set_report&) = delete;

  hid_set_report(void) : dispatcher_client() {
    std::vector<pqrs::cf_ptr<CFDictionaryRef>> matching_dictionaries{
        pqrs::osx::iokit_hid_manager::make_matching_dictionary(
            pqrs::osx::iokit_hid_usage_page_generic_desktop,
            pqrs::osx::iokit_hid_usage_generic_desktop_keyboard),
    };

    hid_manager_ = std::make_unique<pqrs::osx::iokit_hid_manager>(weak_dispatcher_,
                                                                  matching_dictionaries);

    hid_manager_->device_matched.connect([](auto&& registry_entry_id, auto&& device_ptr) {
      if (device_ptr) {
        pqrs::osx::iokit_return r = IOHIDDeviceOpen(*device_ptr, kIOHIDOptionsTypeNone);
        if (!r) {
          std::cerr << "IOHIDDeviceOpen:" << r << " " << registry_entry_id << std::endl;
          return;
        }

        uint8_t report[1    // report_id
                       + 1  // modifiers
                       + 1  // reserved
                       + 32 // keys
        ];
        memset(report, 0, sizeof(report));
        report[0] = 1; // report_id

        r = IOHIDDeviceSetReport(*device_ptr,
                                 kIOHIDReportTypeInput,
                                 report[0],
                                 report,
                                 sizeof(report));
        if (!r) {
          std::cerr << "IOHIDDeviceSetReport:" << r << " " << registry_entry_id << std::endl;
          return;
        }
      }
    });

    hid_manager_->async_start();
  }

  virtual ~hid_set_report(void) {
    hid_manager_ = nullptr;
  }

private:
  std::unique_ptr<pqrs::osx::iokit_hid_manager> hid_manager_;
};

auto global_wait = pqrs::make_thread_wait();
} // namespace

int main(int argc, const char* argv[]) {
  pqrs::dispatcher::extra::initialize_shared_dispatcher();

  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  auto r = std::make_unique<hid_set_report>();

  // ------------------------------------------------------------

  global_wait->wait_notice();

  // ------------------------------------------------------------

  r = nullptr;

  pqrs::dispatcher::extra::terminate_shared_dispatcher();

  std::cout << "finished" << std::endl;

  return 0;
}
