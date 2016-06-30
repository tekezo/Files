#include <CoreGraphics/CoreGraphics.h>
#include <IOKit/hid/IOHIDManager.h>
#include <iostream>

class IOHIDManagerObserver final {
public:
  IOHIDManagerObserver(void) {
    manager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (manager_) {
      IOReturn result = IOHIDManagerOpen(manager_, kIOHIDOptionsTypeNone);
      IOHIDManagerScheduleWithRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
      IOHIDManagerRegisterDeviceMatchingCallback(manager_, Handle_DeviceMatchingCallback, nullptr);

      CFDictionaryRef matchingCFDictRef = hu_CreateDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
      if (matchingCFDictRef) {
        // set the HID device matching dictionary
        IOHIDManagerSetDeviceMatching(manager_, matchingCFDictRef);

        if (result == kIOReturnSuccess) {
          std::cout << "opened" << std::endl;
          IOHIDManagerRegisterInputValueCallback(manager_, inputValueCallback, nullptr);
          IOHIDManagerClose(manager_, kIOHIDOptionsTypeNone);
        }
        CFRelease(matchingCFDictRef);
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

  static CFMutableDictionaryRef hu_CreateDeviceMatchingDictionary(UInt32 inUsagePage, UInt32 inUsage) {
    // create a dictionary to add usage page/usages to
    CFMutableDictionaryRef result = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (result) {
      if (inUsagePage) {
        // Add key for device type to refine the matching dictionary.
        CFNumberRef pageCFNumberRef = CFNumberCreate(
            kCFAllocatorDefault, kCFNumberIntType, &inUsagePage);
        if (pageCFNumberRef) {
          CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
          CFRelease(pageCFNumberRef);

          // note: the usage is only valid if the usage page is also defined
          if (inUsage) {
            CFNumberRef usageCFNumberRef = CFNumberCreate(
                kCFAllocatorDefault, kCFNumberIntType, &inUsage);
            if (usageCFNumberRef) {
              CFDictionarySetValue(result,
                                   CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);
              CFRelease(usageCFNumberRef);
            } else {
              fprintf(stderr, "%s: CFNumberCreate(usage) failed.", __PRETTY_FUNCTION__);
            }
          }
        } else {
          fprintf(stderr, "%s: CFNumberCreate(usage page) failed.", __PRETTY_FUNCTION__);
        }
      }
    } else {
      fprintf(stderr, "%s: CFDictionaryCreateMutable failed.", __PRETTY_FUNCTION__);
    }
    return result;
  } // hu_CreateDeviceMatchingDictionary

private:
  static void inputValueCallback(
      void* _Nullable context,
      IOReturn result,
      void* _Nullable sender,
      IOHIDValueRef value) {
    std::cout << value << std::endl;
  }

  // this will be called when the HID Manager matches a new (hot plugged) HID device
  static void Handle_DeviceMatchingCallback(
      void* inContext,                // context from IOHIDManagerRegisterDeviceMatchingCallback
      IOReturn inResult,              // the result of the matching operation
      void* inSender,                 // the IOHIDManagerRef for the new device
      IOHIDDeviceRef inIOHIDDeviceRef // the new HID device
      ) {
    printf("%s(context: %p, result: %d, sender: %p, device: %p).\n",
           __PRETTY_FUNCTION__, inContext, inResult, inSender, inIOHIDDeviceRef);
  } // Handle_DeviceMatchingCallback

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
