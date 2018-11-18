#pragma once

// pqrs::iokit_hid_device v1.1

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

// `pqrs::osx::iokit_hid_device` can be used safely in a multi-threaded environment.

#include <IOKit/hid/IOHIDDevice.h>
#include <optional>
#include <pqrs/cf_string.hpp>

namespace pqrs {
namespace osx {
class iokit_hid_device final {
public:
  iokit_hid_device(IOHIDDeviceRef device) : device_(device) {
  }

  virtual ~iokit_hid_device(void) {
  }

  cf_ptr<IOHIDDeviceRef> get_device(void) const {
    return device_;
  }

  std::optional<int64_t> find_number_property(CFStringRef key) const {
    if (device_) {
      auto property = IOHIDDeviceGetProperty(*device_, key);
      if (property) {
        if (CFGetTypeID(property) == CFNumberGetTypeID()) {
          int64_t value = 0;
          if (CFNumberGetValue(static_cast<CFNumberRef>(property), kCFNumberSInt64Type, &value)) {
            return value;
          }
        }
      }
    }

    return std::nullopt;
  }

  std::optional<std::string> find_string_property(CFStringRef key) const {
    if (device_) {
      auto property = IOHIDDeviceGetProperty(*device_, key);
      return make_string(property);
    }

    return std::nullopt;
  }

private:
  cf_ptr<IOHIDDeviceRef> device_;
};
} // namespace osx
} // namespace pqrs
