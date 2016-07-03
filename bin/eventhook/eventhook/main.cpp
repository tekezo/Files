#include <CoreGraphics/CoreGraphics.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include <IOKit/hidsystem/IOLLEvent.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <iostream>

class IOHIDPostEventWrapper final {
public:
  IOHIDPostEventWrapper(void) : eventDriver_(IO_OBJECT_NULL) {
    mach_port_t masterPort = IO_OBJECT_NULL;
    mach_port_t service = 0;
    mach_port_t iter = 0;
    mach_port_t ev = 0;
    kern_return_t kr;

    // Getting master device port
    kr = IOMasterPort(bootstrap_port, &masterPort);
    if (KERN_SUCCESS != kr) {
      goto finish;
    }

    kr = IOServiceGetMatchingServices(masterPort, IOServiceMatching(kIOHIDSystemClass), &iter);
    if (KERN_SUCCESS != kr) {
      goto finish;
    }

    service = IOIteratorNext(iter);
    if (!service) {
      goto finish;
    }

    kr = IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &ev);
    if (KERN_SUCCESS != kr) {
      goto finish;
    }

    eventDriver_ = ev;

  finish:
    if (service) {
      IOObjectRelease(service);
    }
    if (iter) {
      IOObjectRelease(iter);
    }
  }

  void postAuxKey(uint8_t auxKeyCode, bool keyDown) {
    if (!eventDriver_) {
      return;
    }

    UInt32 eventType = keyDown ? NX_KEYDOWN : NX_KEYUP;

    NXEventData event;
    bzero(&event, sizeof(event));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = (auxKeyCode << 16 | eventType);

    IOGPoint loc = {0, 0};
    IOHIDPostEvent(eventDriver_, NX_SYSDEFINED, loc, &event, kNXEventDataVersion, kIOHIDPostHIDManagerEvent, 0);
  }

  void postKey(uint8_t keyCode, bool keyDown) {
    if (!eventDriver_) {
      return;
    }

    NXEventData event;
    bzero(&event, sizeof(event));
    event.key.repeat = FALSE;
    event.key.keyCode = keyCode;
    event.key.charSet = NX_ASCIISET;
    event.key.charCode = 0;
    event.key.origCharSet = NX_ASCIISET;
    event.key.origCharCode = 0;

    UInt32 eventType = keyDown ? NX_KEYDOWN : NX_KEYUP;

    IOGPoint loc = {0, 0};
    IOHIDPostEvent(eventDriver_, eventType, loc, &event, kNXEventDataVersion, 0, kIOHIDPostHIDManagerEvent);
  }

private:
  mach_port_t eventDriver_;
};

class IOHIDManagerObserver final {
public:
  IOHIDManagerObserver(void) {
    manager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (manager_) {
      auto deviceMatchingDictionaryArray = createDeviceMatchingDictionaryArray();
      if (deviceMatchingDictionaryArray) {
        IOHIDManagerSetDeviceMatchingMultiple(manager_, deviceMatchingDictionaryArray);
        CFRelease(deviceMatchingDictionaryArray);

        IOHIDManagerScheduleWithRunLoop(manager_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

        IOReturn result = IOHIDManagerOpen(manager_, kIOHIDOptionsTypeSeizeDevice);
        if (result == kIOReturnSuccess) {
          std::cout << "opened" << std::endl;
          IOHIDManagerRegisterInputValueCallback(manager_, inputValueCallback, this);
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

  bool getLongProperty(const IOHIDDeviceRef device, const CFStringRef key, long& value) {
    auto property = IOHIDDeviceGetProperty(device, key);
    if (property) {
      if (CFNumberGetTypeID() == CFGetTypeID(property)) {
        return CFNumberGetValue(static_cast<CFNumberRef>(property), kCFNumberLongType, &value);
      }
    }
    return false;
  }

  long getVendorID(IOHIDDeviceRef device) {
    long value = 0;
    getLongProperty(device, CFSTR(kIOHIDVendorIDKey), value);
    return value;
  }

  long getProductID(IOHIDDeviceRef device) {
    long value = 0;
    getLongProperty(device, CFSTR(kIOHIDProductIDKey), value);
    return value;
  }

  static void inputValueCallback(
      void* _Nullable context,
      IOReturn result,
      void* _Nullable sender,
      IOHIDValueRef value) {
    if (!context) {
      return;
    }

    IOHIDManagerObserver* self = static_cast<IOHIDManagerObserver*>(context);

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
          bool keyDown = (integerValue == 1);
          std::cout << "inputValueCallback usagePage:" << usagePage << " usage:" << usage << " value:" << integerValue << std::endl;
          if (usage == kHIDUsage_KeyboardCapsLock) {
            std::cout << "post" << std::endl;
            (self->postEventWrapper_).postAuxKey(NX_KEYTYPE_MUTE, keyDown);
            //(self->postEventWrapper_).postKey(0x33, keyDown);
          }
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

  IOHIDManagerRef manager_;
  IOHIDPostEventWrapper postEventWrapper_;
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
  //EventHook hook2;
  CFRunLoopRun();
  return 0;
}
