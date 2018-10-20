#include <Carbon/Carbon.h>
@import Cocoa;

CGEventRef callback(CGEventTapProxy proxy,
                    CGEventType type,
                    CGEventRef event,
                    void *refcon) {
  puts("callback");
  return event;
}

int main(void) {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  CGEventMask eventMask = CGEventMaskBit(kCGEventFlagsChanged);

  CFMachPortRef eventTap = CGEventTapCreate(kCGAnnotatedSessionEventTap,
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

  [NSApp run];
  //  CFRunLoopRun();

  return 0;
}
