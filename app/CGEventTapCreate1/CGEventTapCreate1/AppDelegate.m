#import "AppDelegate.h"

@implementation AppDelegate

static CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef cgEvent, void* refcon)
{
  if (! cgEvent) return cgEvent;

  if (type == 14) {
    NSEvent* event = [NSEvent eventWithCGEvent:cgEvent];
    if (! event) return cgEvent;

    NSLog(@"type:%ld, subtype:%hd, data1:%d", [event type], [event subtype], [event data1]);
  }

  return cgEvent;
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  // Quartz Event Services has a bug:
  //
  // When we create CGEventTap with kCGSessionEventTap and kCGEventMaskForAllEvents,
  // look-up feature (tap trackpad with three-fingers) is disabled.

  CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
                                            kCGHeadInsertEventTap,
                                            kCGEventTapOptionDefault,
                                            kCGEventMaskForAllEvents,
                                            eventTapCallBack,
                                            NULL);
  if (! eventTap) {
    NSLog(@"CGEventTapCreate is failed");
    return;
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

  CGEventTapEnable(eventTap, 1);

  CFRelease(runLoopSource);
  CFRelease(eventTap);
}

@end
