#pragma once

// pqrs::ioservice_monitor v1.3.0

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

// `pqrs::osx::iokit::ioservice_monitor` can be used safely in a multi-threaded environment.

#include <nod/nod.hpp>
#include <optional>
#include <pqrs/cf_run_loop_thread.hpp>
#include <pqrs/dispatcher.hpp>
#include <pqrs/osx/iokit/ioobject_ptr.hpp>
#include <pqrs/osx/iokit/types.hpp>

namespace pqrs {
namespace osx {
namespace iokit {
class ioservice_monitor final : dispatcher::extra::dispatcher_client {
public:
  // Signals (invoked from the dispatcher thread)

  nod::signal<void(registry_entry_id, ioobject_ptr)> ioservice_detected;
  nod::signal<void(registry_entry_id)> ioservice_removed;
  nod::signal<void(const std::string&, kern_return_t)> error_occurred;

  // Methods

  ioservice_monitor(const ioservice_monitor&) = delete;

  ioservice_monitor(std::weak_ptr<dispatcher::dispatcher> weak_dispatcher,
                    CFDictionaryRef _Nonnull matching_dictionary) : dispatcher_client(weak_dispatcher),
                                                                    matching_dictionary_(matching_dictionary),
                                                                    notification_port_(nullptr) {
    cf_run_loop_thread_ = std::make_unique<cf_run_loop_thread>();
  }

  virtual ~ioservice_monitor(void) {
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

  void async_invoke_ioservice_detected(void) {
    enqueue_to_dispatcher([this] {
      if (*matching_dictionary_) {
        io_iterator_t it = IO_OBJECT_NULL;
        CFRetain(*matching_dictionary_);
        auto kr = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                               *matching_dictionary_,
                                               &it);
        if (kr != KERN_SUCCESS) {
          enqueue_to_dispatcher([this, kr] {
            error_occurred("IOServiceGetMatchingServices is failed.", kr);
          });
        } else {
          matched_callback(it);
          IOObjectRelease(it);
        }
      }
    });
  }

private:
  // This method is executed in the dispatcher thread.
  void start(void) {
    if (!notification_port_) {
      notification_port_ = IONotificationPortCreate(kIOMasterPortDefault);
      if (!notification_port_) {
        enqueue_to_dispatcher([this] {
          error_occurred("IONotificationPortCreate is failed.", kIOReturnError);
        });
        return;
      }

      if (auto loop_source = IONotificationPortGetRunLoopSource(notification_port_)) {
        CFRunLoopAddSource(cf_run_loop_thread_->get_run_loop(),
                           loop_source,
                           kCFRunLoopCommonModes);
      } else {
        enqueue_to_dispatcher([this] {
          error_occurred("IONotificationPortGetRunLoopSource is failed.", kIOReturnError);
        });
        return;
      }
    }

    // kIOMatchedNotification

    if (!matched_notification_) {
      if (*matching_dictionary_) {
        io_iterator_t it = IO_OBJECT_NULL;
        CFRetain(*matching_dictionary_);
        auto kr = IOServiceAddMatchingNotification(notification_port_,
                                                   kIOFirstMatchNotification,
                                                   *matching_dictionary_,
                                                   &static_matched_callback,
                                                   static_cast<void*>(this),
                                                   &it);
        if (kr != kIOReturnSuccess) {
          enqueue_to_dispatcher([this, kr] {
            error_occurred("IOServiceAddMatchingNotification is failed.", kr);
          });
        } else {
          matched_notification_ = it;
          IOObjectRelease(it);
          matched_callback(*matched_notification_);
        }
      }
    }

    // kIOTerminatedNotification

    if (!terminated_notification_) {
      if (*matching_dictionary_) {
        io_iterator_t it = IO_OBJECT_NULL;
        CFRetain(*matching_dictionary_);
        auto kr = IOServiceAddMatchingNotification(notification_port_,
                                                   kIOTerminatedNotification,
                                                   *matching_dictionary_,
                                                   &static_terminated_callback,
                                                   static_cast<void*>(this),
                                                   &it);
        if (kr != kIOReturnSuccess) {
          enqueue_to_dispatcher([this, kr] {
            error_occurred("IOServiceAddMatchingNotification is failed.", kr);
          });
        } else {
          terminated_notification_ = it;
          IOObjectRelease(it);
          terminated_callback(*terminated_notification_);
        }
      }
    }
  }

  // This method is executed in the dispatcher thread.
  void stop(void) {
    matched_notification_.reset();
    terminated_notification_.reset();

    if (notification_port_) {
      if (auto loop_source = IONotificationPortGetRunLoopSource(notification_port_)) {
        CFRunLoopRemoveSource(cf_run_loop_thread_->get_run_loop(),
                              loop_source,
                              kCFRunLoopCommonModes);
      }

      IONotificationPortDestroy(notification_port_);
      notification_port_ = nullptr;
    }
  }

  static void static_matched_callback(void* _Nonnull refcon, io_iterator_t iterator) {
    auto self = static_cast<ioservice_monitor*>(refcon);
    if (!self) {
      return;
    }

    self->matched_callback(iterator);
  }

  void matched_callback(io_iterator_t iterator) {
    if (iterator) {
      while (auto s = IOIteratorNext(iterator)) {
        if (s) {
          if (auto registry_entry_id = find_registry_entry_id(s)) {
            ioobject_ptr ptr(s);

            enqueue_to_dispatcher([this, registry_entry_id, ptr] {
              ioservice_detected(*registry_entry_id, ptr);
            });
          }

          IOObjectRelease(s);
        }
      }
    }
  }

  static void static_terminated_callback(void* _Nonnull refcon, io_iterator_t iterator) {
    auto self = static_cast<ioservice_monitor*>(refcon);
    if (!self) {
      return;
    }

    self->terminated_callback(iterator);
  }

  void terminated_callback(io_iterator_t iterator) {
    if (iterator) {
      while (auto s = IOIteratorNext(iterator)) {
        if (s) {
          if (auto registry_entry_id = find_registry_entry_id(s)) {
            enqueue_to_dispatcher([this, registry_entry_id] {
              ioservice_removed(*registry_entry_id);
            });
          }

          IOObjectRelease(s);
        }
      }
    }
  }

  std::optional<registry_entry_id> find_registry_entry_id(io_service_t service) {
    if (service) {
      uint64_t value;
      auto kr = IORegistryEntryGetRegistryEntryID(service, &value);
      if (kr == KERN_SUCCESS) {
        return registry_entry_id(value);
      }
    }

    return std::nullopt;
  }

  cf_ptr<CFDictionaryRef> matching_dictionary_;

  std::unique_ptr<cf_run_loop_thread> cf_run_loop_thread_;
  IONotificationPortRef _Nullable notification_port_;
  ioobject_ptr matched_notification_;
  ioobject_ptr terminated_notification_;
};
} // namespace iokit
} // namespace osx
} // namespace pqrs
