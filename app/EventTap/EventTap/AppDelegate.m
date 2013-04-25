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
    case kCGEventTapDisabledByTimeout:
      // Re-enable the event tap if it times out.
      CGEventTapEnable(eventTap, true);
      break;

    case NSSystemDefined:
    {
      // NSSystemDefined events that are not the Power Key will be returned unmodified.
      int powerKeyType = [AppDelegate getPowerKeyType:event];
      if (powerKeyType == POWER_KEY_TYPE_SUBTYPE) {
        NSLog(@"isPowerKey POWER_KEY_TYPE_SUBTYPE");
        event = NULL;
      }
      if (powerKeyType == POWER_KEY_TYPE_KEYCODE) {
        NSLog(@"isPowerKey POWER_KEY_TYPE_KEYCODE");
        event = NULL;
      }
      break;
    }

    default:
      break;
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
  CFRelease(eventTap);
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  // Insert code here to initialize your application
  [self monitorPowerKey];
}

@end
