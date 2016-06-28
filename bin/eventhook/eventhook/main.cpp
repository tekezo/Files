#include <CoreGraphics/CoreGraphics.h>
#include <iostream>

class EventHook {
public:
  EventHook(void) {
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

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap_, 0);
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
  EventHook hook;
  CFRunLoopRun();
  return 0;
}
