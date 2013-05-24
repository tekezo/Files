#import "AppDelegate.h"

@implementation AppDelegate

static CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon)
{
  return event;
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
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
