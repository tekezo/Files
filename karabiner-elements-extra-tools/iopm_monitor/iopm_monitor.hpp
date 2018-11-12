#pragma once

#include <IOKit/IOMessage.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <pqrs/osx/iokit_service_monitor.hpp>

class iopm_monitor final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  // Signals (invoked from the shared dispatcher thread)

  nod::signal<void(uint32_t)> system_power_event_arrived;

  // Methods

  iopm_monitor(const iopm_monitor&) = delete;

  iopm_monitor(void) : dispatcher_client(),
                       notification_port_(nullptr),
                       notifier_(IO_OBJECT_NULL),
                       connect_(IO_OBJECT_NULL) {
    cf_run_loop_thread_ = std::make_unique<pqrs::cf_run_loop_thread>();
  }

  virtual ~iopm_monitor(void) {
    detach_from_dispatcher([this] {
      stop();
    });

    cf_run_loop_thread_->terminate();
    cf_run_loop_thread_ = nullptr;
  }

  void async_start(void) {
    enqueue_to_dispatcher([this] {
      start();
    });
  }

  void async_stop(void) {
    enqueue_to_dispatcher([this] {
      stop();
    });
  }

private:
  // This method is executed in the dispatcher thread.
  void start(void) {
    if (notification_port_) {
      return;
    }

    auto connect = IORegisterForSystemPower(this,
                                            &notification_port_,
                                            static_callback,
                                            &notifier_);
    if (connect == MACH_PORT_NULL) {
      return;
    }

    if (auto run_loop_source = IONotificationPortGetRunLoopSource(notification_port_)) {
      CFRunLoopAddSource(cf_run_loop_thread_->get_run_loop(),
                         run_loop_source,
                         kCFRunLoopCommonModes);
    }
  }

  // This method is executed in the dispatcher thread.
  void stop(void) {
    if (notifier_) {
      IODeregisterForSystemPower(&notifier_);
      notifier_ = IO_OBJECT_NULL;
    }

    if (notification_port_) {
      IONotificationPortDestroy(notification_port_);
      notification_port_ = nullptr;
    }

    if (connect_) {
      IOServiceClose(connect_);
      connect_ = IO_OBJECT_NULL;
    }
  }

  static void static_callback(void* refcon, io_service_t service, uint32_t message_type, void* message_argument) {
    auto self = static_cast<iopm_monitor*>(refcon);
    if (self) {
      self->callback(service, message_type, message_argument);
    }
  }

  void callback(io_service_t service, uint32_t message_type, void* message_argument) {
    switch (message_type) {
    case kIOMessageSystemWillSleep:
      if (connect_) {
        IOAllowPowerChange(connect_, reinterpret_cast<intptr_t>(message_argument));
      }
      break;

    case kIOMessageCanSystemSleep:
      if (connect_) {
        IOAllowPowerChange(connect_, reinterpret_cast<intptr_t>(message_argument));
      }
      break;

    case kIOMessageSystemWillPowerOn:
    case kIOMessageSystemHasPoweredOn:
    case kIOMessageSystemWillNotSleep:
      break;
    }

    enqueue_to_dispatcher([this, message_type] {
      system_power_event_arrived(message_type);
    });
  }

  std::unique_ptr<pqrs::cf_run_loop_thread> cf_run_loop_thread_;
  IONotificationPortRef notification_port_;
  io_object_t notifier_;
  io_connect_t connect_;
};
