#pragma once

class human_interface_device final {
public:
  human_interface_device(IOHIDDeviceRef _Nonnull device) : device_(device),
                                                           queue_(nullptr),
                                                           grabbed_(false) {
    CFRetain(device_);
  }

  ~human_interface_device(void) {
    if (queue_) {
      CFRelease(queue_);
    }

    CFRelease(device_);
  }

  void grab(void) {
    if (!device_) {
      return;
    }

    if (grabbed_) {
      ungrab();
    }

    IOReturn r = IOHIDDeviceOpen(device_, kIOHIDOptionsTypeSeizeDevice);
    if (r != kIOReturnSuccess) {
      std::cerr << "Failed to IOHIDDeviceOpen: " << std::hex << r << std::endl;
      return;
    }

    IOHIDDeviceRegisterInputValueCallback(device_, input_value_callback, this);
    IOHIDDeviceScheduleWithRunLoop(device_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

    grabbed_ = true;
  }

  void ungrab(void) {
    if (!device_) {
      return;
    }

    if (!grabbed_) {
      return;
    }

    IOHIDDeviceUnscheduleFromRunLoop(device_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    IOHIDDeviceRegisterInputValueCallback(device_, nullptr, nullptr);

    IOReturn r = IOHIDDeviceClose(device_, kIOHIDOptionsTypeSeizeDevice);
    if (r != kIOReturnSuccess) {
      std::cerr << "Failed to IOHIDDeviceClose: " << std::hex << r << std::endl;
      return;
    }

    grabbed_ = false;
  }

  long get_vendor_id(void) {
    long value = 0;
    get_long_property_(CFSTR(kIOHIDVendorIDKey), value);
    return value;
  }

  long get_product_id(void) {
    long value = 0;
    get_long_property_(CFSTR(kIOHIDProductIDKey), value);
    return value;
  }

  long get_location_id(void) {
    long value = 0;
    get_long_property_(CFSTR(kIOHIDLocationIDKey), value);
    return value;
  }

  std::string get_manufacturer(void) {
    std::string value;
    get_string_property_(CFSTR(kIOHIDManufacturerKey), value);
    return value;
  }

  std::string get_product(void) {
    std::string value;
    get_string_property_(CFSTR(kIOHIDProductKey), value);
    return value;
  }

  std::string get_serial_number_string(void) {
    std::string value;
    get_string_property_(CFSTR(kIOHIDSerialNumberKey), value);
    return value;
  }

private:
  bool get_long_property_(const CFStringRef _Nonnull key, long& value) {
    if (!device_) {
      return false;
    }

    auto property = IOHIDDeviceGetProperty(device_, key);
    if (!property) {
      return false;
    }

    if (CFNumberGetTypeID() != CFGetTypeID(property)) {
      return false;
    }

    return CFNumberGetValue(static_cast<CFNumberRef>(property), kCFNumberLongType, &value);
  }

  bool get_string_property_(const CFStringRef _Nonnull key, std::string& value) {
    if (!device_) {
      return false;
    }

    auto property = IOHIDDeviceGetProperty(device_, key);
    if (!property) {
      return false;
    }

    if (CFStringGetTypeID() != CFGetTypeID(property)) {
      return false;
    }

    auto p = CFStringGetCStringPtr(static_cast<CFStringRef>(property), kCFStringEncodingUTF8);
    if (!p) {
      value.clear();
    } else {
      value = p;
    }
    return true;
  }

  static void input_value_callback(
      void* _Nullable context,
      IOReturn result,
      void* _Nullable sender,
      IOHIDValueRef _Nullable value) {
    if (!context) {
      return;
    }

    // auto self = static_cast<event_grabber*>(context);

    if (value) {
      auto element = IOHIDValueGetElement(value);
      auto integerValue = IOHIDValueGetIntegerValue(value);

      if (element) {
        auto usagePage = IOHIDElementGetUsagePage(element);
        auto usage = IOHIDElementGetUsage(element);

        std::cout << "type: " << IOHIDElementGetType(element) << std::endl;

        switch (usagePage) {
        case kHIDPage_KeyboardOrKeypad:
          if (usage == kHIDUsage_KeyboardErrorRollOver ||
              usage == kHIDUsage_KeyboardPOSTFail ||
              usage == kHIDUsage_KeyboardErrorUndefined ||
              usage >= kHIDUsage_GD_Reserved) {
            // do nothing
          } else {
            // bool keyDown = (integerValue == 1);
            std::cout << "inputValueCallback usagePage:" << usagePage << " usage:" << usage << " value:" << integerValue << std::endl;
            if (usage == kHIDUsage_KeyboardEscape) {
              exit(0);
            }
          }
          break;

        default:
          std::cout << "inputValueCallback unknown usagePage:" << usagePage << " usage:" << usage << std::endl;
        }
      }
    }
  }

  IOHIDDeviceRef _Nonnull device_;
  IOHIDQueueRef _Nullable queue_;
  bool grabbed_;
};
