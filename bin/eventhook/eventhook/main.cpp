#include <CoreGraphics/CoreGraphics.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>
#include <iostream>

class IOHIDManagerObserver final {
public:
  IOHIDManagerObserver(void) {
    manager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (manager_) {
      auto deviceMatchingDictionaryArray = createDeviceMatchingDictionaryArray();
      if (deviceMatchingDictionaryArray) {
        IOHIDManagerSetDeviceMatchingMultiple(manager_, deviceMatchingDictionaryArray);
        CFRelease(deviceMatchingDictionaryArray);

        IOHIDManagerRegisterDeviceMatchingCallback(manager_, Handle_DeviceMatchingCallback, nullptr);
        IOHIDManagerRegisterDeviceRemovalCallback(manager_, Handle_RemovalCallback, nullptr);
        IOHIDManagerScheduleWithRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

        IOReturn result = IOHIDManagerOpen(manager_, kIOHIDOptionsTypeNone);
        if (result == kIOReturnSuccess) {
          std::cout << "opened" << std::endl;
          IOHIDManagerRegisterInputValueCallback(manager_, inputValueCallback, nullptr);
        }
      }
    }
  }

  ~IOHIDManagerObserver(void) {
    if (manager_) {
      IOHIDManagerUnscheduleFromRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
      CFRelease(manager_);
      manager_ = nullptr;
    }
  }

  static void Handle_DeviceMatchingCallback(
      void* inContext,                // context from IOHIDManagerRegisterDeviceMatchingCallback
      IOReturn inResult,              // the result of the matching operation
      void* inSender,                 // the IOHIDManagerRef for the new device
      IOHIDDeviceRef inIOHIDDeviceRef // the new HID device
      ) {
    printf("%s(context: %p, result: %d, sender: %p, device: %p).\n",
           __PRETTY_FUNCTION__, inContext, inResult, inSender, (void*)inIOHIDDeviceRef);
  } // Handle_DeviceMatchingCallback

  // this will be called when a HID device is removed (unplugged)
  static void Handle_RemovalCallback(
      void* inContext,                // context from IOHIDManagerRegisterDeviceMatchingCallback
      IOReturn inResult,              // the result of the removing operation
      void* inSender,                 // the IOHIDManagerRef for the device being removed
      IOHIDDeviceRef inIOHIDDeviceRef // the removed HID device
      ) {
    printf("%s(context: %p, result: %d, sender: %p, device: %p).\n",
           __PRETTY_FUNCTION__, inContext, inResult, inSender, (void*)inIOHIDDeviceRef);
  } // Handle_RemovalCallback

  // Get a HID device's vendor ID (long)
  static long IOHIDDevice_GetVendorID(IOHIDDeviceRef inIOHIDDeviceRef) {
    long result = 0;
    IOHIDDevice_GetLongProperty(inIOHIDDeviceRef, CFSTR(kIOHIDVendorIDKey), &result);
    return result;
  } // IOHIDDevice_GetVendorID

  // Get a HID device's product ID (long)
  static long IOHIDDevice_GetProductID(IOHIDDeviceRef inIOHIDDeviceRef) {
    long result = 0;
    IOHIDDevice_GetLongProperty(inIOHIDDeviceRef, CFSTR(kIOHIDProductIDKey), &result);
    return result;
  } // IOHIDDevice_GetProductID

  static Boolean IOHIDDevice_GetLongProperty(
      IOHIDDeviceRef inDeviceRef, // the HID device reference
      CFStringRef inKey,          // the kIOHIDDevice key (as a CFString)
      long* outValue) {
    Boolean result = FALSE;

    CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(inDeviceRef, inKey);
    if (tCFTypeRef) {
      // if this is a number
      if (CFNumberGetTypeID() == CFGetTypeID(tCFTypeRef)) {
        // get its value
        result = CFNumberGetValue((CFNumberRef)tCFTypeRef, kCFNumberSInt32Type, outValue);
      }
    }
    return result;
  } // IOHIDDevice_GetLongProperty

private:
  CFDictionaryRef createDeviceMatchingDictionary(uint32_t inUsagePage, uint32_t inUsage) {
    auto deviceMatchingDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (!deviceMatchingDictionary) {
      goto finish;
    }

    // inUsagePage
    if (!inUsagePage) {
      goto finish;
    } else {
      auto usagePage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsagePage);
      if (!usagePage) {
        goto finish;
      }
      CFDictionarySetValue(deviceMatchingDictionary, CFSTR(kIOHIDDeviceUsagePageKey), usagePage);
      CFRelease(usagePage);
    }

    // inUsage (The usage is only valid if the usage page is also defined)
    if (!inUsage) {
      goto finish;
    } else {
      auto usage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsage);
      if (!usage) {
        goto finish;
      }
      CFDictionarySetValue(deviceMatchingDictionary, CFSTR(kIOHIDDeviceUsageKey), usage);
      CFRelease(usage);
    }

  finish:
    return deviceMatchingDictionary;
  }

  CFArrayRef createDeviceMatchingDictionaryArray(void) {
    auto deviceMatchingDictionaryArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (!deviceMatchingDictionaryArray) {
      return nullptr;
    }

    // kHIDUsage_GD_Keyboard
    {
      auto deviceMatchingDictionary = createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
      if (deviceMatchingDictionary) {
        CFArrayAppendValue(deviceMatchingDictionaryArray, deviceMatchingDictionary);
        CFRelease(deviceMatchingDictionary);
      }
    }

    // kHIDUsage_GD_Mouse
    {
      auto deviceMatchingDictionary = createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Mouse);
      if (deviceMatchingDictionary) {
        CFArrayAppendValue(deviceMatchingDictionaryArray, deviceMatchingDictionary);
        CFRelease(deviceMatchingDictionary);
      }
    }

    return deviceMatchingDictionaryArray;
  }

  static void inputValueCallback(
      void* _Nullable context,
      IOReturn result,
      void* _Nullable sender,
      IOHIDValueRef value) {
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
            std::cout << "inputValueCallback usagePage:" << usagePage << " usage:" << usage << " value:" << integerValue << std::endl;
          }
          break;

        default:
          std::cout << "inputValueCallback unknown usagePage:" << usagePage << " usage:" << usage << std::endl;
      }
    }
  }

  IOHIDManagerRef manager_;
};

class EventHook {
public:
  EventHook(void) : eventTap_(nullptr) {
    eventTap_ = CGEventTapCreate(kCGHIDEventTap,
                                 kCGHeadInsertEventTap,
                                 kCGEventTapOptionDefault,
                                 CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp),
                                 EventHook::eventTapCallBack,
                                 NULL);
    if (!eventTap_) {
      std::cerr << "CGEventTapCreate is failed" << std::endl;
      return;
    }

    auto runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap_, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    CGEventTapEnable(eventTap_, 1);

    CFRelease(runLoopSource);
  }

  static CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) {
    std::cout << "type " << type << std::endl;
    switch (type) {
    case kCGEventKeyDown:
    case kCGEventKeyUp: {
      int64_t keycode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
      std::cout << "key " << keycode;
      // Escape
      if (keycode == 0x35) {
      }
      break;
    }
    default:
      break;
    }
    return event;
  }

private:
  CFMachPortRef eventTap_;
};

int main(int argc, const char* argv[]) {
  //IOHIDQueueInterfaceHook hook1;
  IOHIDManagerObserver observer;
  EventHook hook2;
  CFRunLoopRun();
  return 0;
}
