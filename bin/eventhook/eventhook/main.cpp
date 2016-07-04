#include <CoreGraphics/CoreGraphics.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include <IOKit/hidsystem/IOLLEvent.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <iostream>

#include "event_grabber.hpp"

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
  event_grabber observer;
  //EventHook hook2;
  CFRunLoopRun();
  return 0;
}
