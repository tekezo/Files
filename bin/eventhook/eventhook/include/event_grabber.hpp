#pragma once

class event_grabber final {
public:
  event_grabber(void) {
    manager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!manager_) {
      return;
    }

    auto device_matching_dictionaries = create_device_matching_dictionaries();
    if (device_matching_dictionaries) {
      IOHIDManagerSetDeviceMatchingMultiple(manager_, device_matching_dictionaries);
      CFRelease(device_matching_dictionaries);

      IOHIDManagerRegisterDeviceMatchingCallback(manager_, device_matching_callback, this);
      IOHIDManagerRegisterDeviceRemovalCallback(manager_, device_removal_callback, this);

      IOHIDManagerScheduleWithRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

      IOReturn result = IOHIDManagerOpen(manager_, kIOHIDOptionsTypeSeizeDevice);
      if (result == kIOReturnSuccess) {
        std::cout << "opened" << std::endl;
        IOHIDManagerRegisterInputValueCallback(manager_, inputValueCallback, this);
      }
    }
  }

  ~event_grabber(void) {
    if (manager_) {
      IOHIDManagerUnscheduleFromRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
      CFRelease(manager_);
      manager_ = nullptr;
    }
  }

private:
  CFDictionaryRef _Nonnull create_device_matching_dictionary(uint32_t usage_page, uint32_t usage) {
    auto device_matching_dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (!device_matching_dictionary) {
      goto finish;
    }

    // usage_page
    if (!usage_page) {
      goto finish;
    } else {
      auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage_page);
      if (!number) {
        goto finish;
      }
      CFDictionarySetValue(device_matching_dictionary, CFSTR(kIOHIDDeviceUsagePageKey), number);
      CFRelease(number);
    }

    // usage (The usage is only valid if the usage page is also defined)
    if (!usage) {
      goto finish;
    } else {
      auto number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
      if (!number) {
        goto finish;
      }
      CFDictionarySetValue(device_matching_dictionary, CFSTR(kIOHIDDeviceUsageKey), number);
      CFRelease(number);
    }

  finish:
    return device_matching_dictionary;
  }

  CFArrayRef _Nullable create_device_matching_dictionaries(void) {
    auto device_matching_dictionaries = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (!device_matching_dictionaries) {
      return nullptr;
    }

    // kHIDUsage_GD_Keyboard
    {
      auto device_matching_dictionary = create_device_matching_dictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
      if (device_matching_dictionary) {
        CFArrayAppendValue(device_matching_dictionaries, device_matching_dictionary);
        CFRelease(device_matching_dictionary);
      }
    }

    // kHIDUsage_GD_Mouse
    {
      auto device_matching_dictionary = create_device_matching_dictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Mouse);
      if (device_matching_dictionary) {
        CFArrayAppendValue(device_matching_dictionaries, device_matching_dictionary);
        CFRelease(device_matching_dictionary);
      }
    }

    return device_matching_dictionaries;
  }

  static void device_matching_callback(void* _Nullable context, IOReturn result, void* _Nullable sender, IOHIDDeviceRef _Nonnull device) {
    if (result != kIOReturnSuccess) {
      return;
    }

    auto self = static_cast<event_grabber*>(context);
    if (!self) {
      return;
    }

    if (!device) {
      return;
    }

    auto vendor_id = self->get_vendor_id(device);
    auto product_id = self->get_product_id(device);

    std::cout << "matching vendor_id:0x" << std::hex << vendor_id << " product_id:0x" << std::hex << product_id << std::endl;
  }

  static void device_removal_callback(void* _Nullable context, IOReturn result, void* _Nullable sender, IOHIDDeviceRef _Nonnull device) {
    if (result != kIOReturnSuccess) {
      return;
    }

    auto self = static_cast<event_grabber*>(context);
    if (!self) {
      return;
    }

    if (!device) {
      return;
    }

    auto vendor_id = self->get_vendor_id(device);
    auto product_id = self->get_product_id(device);

    std::cout << "removal vendor_id:0x" << std::hex << vendor_id << " product_id:0x" << std::hex << product_id << std::endl;
  }

  bool get_long_property(const IOHIDDeviceRef _Nonnull device, const CFStringRef _Nonnull key, long& value) {
    if (!device) {
      return false;
    }

    auto property = IOHIDDeviceGetProperty(device, key);
    if (!property) {
      return false;
    }

    if (CFNumberGetTypeID() != CFGetTypeID(property)) {
      return false;
    }

    return CFNumberGetValue(static_cast<CFNumberRef>(property), kCFNumberLongType, &value);
  }

  long get_vendor_id(IOHIDDeviceRef _Nonnull device) {
    long value = 0;
    get_long_property(device, CFSTR(kIOHIDVendorIDKey), value);
    return value;
  }

  long get_product_id(IOHIDDeviceRef _Nonnull device) {
    long value = 0;
    get_long_property(device, CFSTR(kIOHIDProductIDKey), value);
    return value;
  }

  static void inputValueCallback(
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
      auto usagePage = IOHIDElementGetUsagePage(element);
      auto usage = IOHIDElementGetUsage(element);
      auto integerValue = IOHIDValueGetIntegerValue(value);

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

  IOHIDManagerRef _Nullable manager_;
};
