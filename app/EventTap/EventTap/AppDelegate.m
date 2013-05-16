// This code is based on PowerKey of Peter Kamb.
// https://github.com/pkamb/PowerKey/

#import <IOKit/hidsystem/ev_keymap.h>
#import <IOKit/hidsystem/IOLLEvent.h>
#import "AppDelegate.h"

@implementation AppDelegate

CFMachPortRef eventTap;

enum {
  POWER_KEY_TYPE_NONE,
  POWER_KEY_TYPE_SUBTYPE, // subtype == NX_SUBTYPE_POWER_KEY
  POWER_KEY_TYPE_KEYCODE, // subtype == NX_SUBTYPE_AUX_CONTROL_BUTTONS and keyCode == NX_POWER_KEY
};

// The power button sends two events.
// POWER_KEY_TYPE_SUBTYPE and POWER_KEY_TYPE_KEYCODE.
//
// A build-in keyboard of MacBook sends these events.
// - POWER_KEY_TYPE_SUBTYPE (at key down)
// - POWER_KEY_TYPE_KEYCODE (at key down)
//
// An external keyboard which has power key sends these events.
// - POWER_KEY_TYPE_SUBTYPE (at key down)
// - POWER_KEY_TYPE_KEYCODE (at key down)
// - POWER_KEY_TYPE_KEYCODE (at key up)

+ (int) getPowerKeyType:(CGEventRef)cgEvent
{
  if (! cgEvent) return POWER_KEY_TYPE_NONE;

  NSEvent* event = [NSEvent eventWithCGEvent:cgEvent];
  if (! event) return POWER_KEY_TYPE_NONE;

  if ([event type] != NSSystemDefined) return POWER_KEY_TYPE_NONE;

  if ([event subtype] == NX_SUBTYPE_POWER_KEY) {
    return POWER_KEY_TYPE_SUBTYPE;
  }

  int keyCode = (([event data1] & 0xFFFF0000) >> 16);
  if (keyCode == NX_POWER_KEY) {
    return POWER_KEY_TYPE_KEYCODE;
  }

  return POWER_KEY_TYPE_NONE;
}

CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon)
{
  switch (type) {
    case NSSystemDefined:
    {
      NSEvent* e = [NSEvent eventWithCGEvent:event];
      NSLog(@"eventTapCallBack: type:%ld", [e type]);
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
  // We need to grab NSSystemDefined events.
  // So, we call CGEventCreate with kCGEventMaskForAllEvents.
  eventTap = CGEventTapCreate(kCGSessionEventTap,
                              kCGHeadInsertEventTap,
                              kCGEventTapOptionDefault,
                              NSSystemDefinedMask,
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
  //CFRelease(eventTap);

  //CGEventTapEnable(eventTap, 0);
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  // Insert code here to initialize your application
  [self monitorPowerKey];
}

@end
