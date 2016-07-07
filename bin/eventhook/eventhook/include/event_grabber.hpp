#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class hid_device final {
public:
  hid_device(IOHIDDeviceRef _Nonnull device) : device_(device),
                                               queue_(nullptr),
                                               grabbed_(false) {
  }

  ~hid_device(void) {
    if (queue_) {
      CFRelease(queue_);
    }
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

  IOHIDDeviceRef _Nonnull device_;
  IOHIDQueueRef _Nullable queue_;
  bool grabbed_;
};

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

#if 0
      IOReturn result = IOHIDManagerOpen(manager_, kIOHIDOptionsTypeSeizeDevice);
      if (result == kIOReturnSuccess) {
        std::cout << "opened" << std::endl;
        IOHIDManagerRegisterInputValueCallback(manager_, inputValueCallback, this);
      }
#endif
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

    auto dev = std::make_shared<hid_device>(device);
    if (!dev) {
      return;
    }

    std::cout << "matching vendor_id:0x" << std::hex << dev->get_vendor_id()
              << " product_id:0x" << std::hex << dev->get_product_id()
              << " location_id:0x" << std::hex << dev->get_location_id()
              << " " << dev->get_manufacturer()
              << " " << dev->get_product()
              << std::endl;

    if (dev->get_product() == "HHKB Professional JP") {
      dev->grab();
    }

    (self->hid_devices_)[device] = dev;
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

    auto it = (self->hid_devices_).find(device);
    if (it == (self->hid_devices_).end()) {
      std::cout << "unknown device has been removed" << std::endl;
    } else {
      auto dev = it->second;
      if (dev) {
        std::cout << "removal vendor_id:0x" << std::hex << dev->get_vendor_id() << " product_id:0x" << std::hex << dev->get_product_id() << std::endl;
        (self->hid_devices_).erase(it);
      }
    }
  }

  IOHIDManagerRef _Nullable manager_;
  std::unordered_map<IOHIDDeviceRef, std::shared_ptr<hid_device>> hid_devices_;
};
