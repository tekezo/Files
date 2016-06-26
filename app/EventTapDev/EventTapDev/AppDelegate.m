#import "AppDelegate.h"
#import "AXProcessTrustedObserver.h"
#import "weakify.h"

@interface AppDelegate ()

@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet AXProcessTrustedObserver* axProcessTrustedObserver;

@property NSMutableArray* observers;
@property CFMachPortRef eventTap;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  self.observers = [NSMutableArray new];

  {
    @weakify(self);

    id observer = [[NSNotificationCenter defaultCenter]
        addObserverForName:kProcessTrustedNotification
                    object:nil
                     queue:[NSOperationQueue mainQueue]
                usingBlock:^(NSNotification* notification) {
                  @strongify(self);
                  if (!self) return;

                  [self setupEventTap];
                }];
    if (observer) {
      [self.observers addObject:observer];
    }
  }

  [self.axProcessTrustedObserver start];
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
  [self.axProcessTrustedObserver stop];

  for (id observer in self.observers) {
    [[NSNotificationCenter defaultCenter] removeObserver:observer];
  }
}

static CGEventRef eventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) {
  switch (type) {
  case kCGEventKeyDown:
    NSLog(@"kCGEventKeyDown");
    break;
  case kCGEventKeyUp:
    NSLog(@"kCGEventKeyUp");
  default:
    NSLog(@"eventTapCallBack: type:%d", type);
    break;
  }
  return event;
}

- (BOOL)setupEventTap {
  self.eventTap = CGEventTapCreate(kCGSessionEventTap,
                                   kCGHeadInsertEventTap,
                                   kCGEventTapOptionDefault,
                                   CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp),
                                   eventTapCallBack,
                                   NULL);
  if (!self.eventTap) {
    NSLog(@"CGEventTapCreate is failed");
    return NO;
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, self.eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

  CGEventTapEnable(self.eventTap, 1);

  CFRelease(runLoopSource);

  return YES;
}

@end
