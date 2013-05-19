// This code is based on PowerKey of Peter Kamb.
// https://github.com/pkamb/PowerKey/

#import <IOKit/hidsystem/ev_keymap.h>
#import <IOKit/hidsystem/IOLLEvent.h>
#import "AppDelegate.h"

@implementation AppDelegate

CFMachPortRef eventTap;

CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon)
{
  NSLog(@"eventTapCallBack: type:%d", type);

  switch (type) {
    case NSSystemDefined:
    {
      NSEvent* e = [NSEvent eventWithCGEvent:event];
      NSLog(@"modifierFlags:%lx", [e modifierFlags]);
      if ([e type] == NSSystemDefined) {
        NSLog(@"eventTapCallBack: subtype:%hd, data1:%lx, data2:%lx", [e subtype], [e data1], [e data2]);
        int keyCode = (([e data1] & 0xFFFF0000) >> 16);
        NSLog(@"eventTapCallBack: keyCode:%d", keyCode);
      }
      break;
    }
  }
  return event;
}

- (void) monitorPowerKey
{
  eventTap = CGEventTapCreate(kCGSessionEventTap,
                              kCGHeadInsertEventTap,
                              kCGEventTapOptionDefault,
                              kCGEventMaskForAllEvents,
                              eventTapCallBack,
                              NULL);
  if (! eventTap) {
    NSLog(@"CGEventTapCreate is failed");
    exit(1);
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

  CGEventTapEnable(eventTap, 1);

  CFRelease(runLoopSource);
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  // Insert code here to initialize your application
  [self monitorPowerKey];
}

@end
