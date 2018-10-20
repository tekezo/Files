#include <Carbon/Carbon.h>

CGEventRef callback(CGEventTapProxy proxy,
                    CGEventType type,
                    CGEventRef event,
                    void *refcon) {
  puts("callback");
  return event;
}

int main(void) {
  CGEventMask eventMask = CGEventMaskBit(kCGEventLeftMouseDown) |
                          CGEventMaskBit(kCGEventLeftMouseUp) |
                          CGEventMaskBit(kCGEventMouseMoved);

  CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap,
                                            kCGTailAppendEventTap,
                                            kCGEventTapOptionListenOnly,
                                            eventMask,
                                            callback,
                                            NULL);

  if (!eventTap) {
    fprintf(stderr, "failed to create event tap\n");
    exit(1);
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault,
                                                                   eventTap,
                                                                   0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(),
                     runLoopSource,
                     kCFRunLoopCommonModes);

  CGEventTapEnable(eventTap, true);

  CFRunLoopRun();

  return 0;
}
