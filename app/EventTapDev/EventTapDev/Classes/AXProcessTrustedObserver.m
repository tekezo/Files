#import "AXProcessTrustedObserver.h"
#import "weakify.h"

@interface AXProcessTrustedObserver ()

@property dispatch_source_t timer;

@end

@implementation AXProcessTrustedObserver

- (BOOL)trusted {
  return AXIsProcessTrusted();
}

- (void)start {
  if (self.trusted) {
    self.timer = nil;
    [[NSNotificationCenter defaultCenter] postNotificationName:kProcessTrustedNotification object:nil];
    return;
  }

  NSDictionary* options = @{(__bridge NSString*)(kAXTrustedCheckOptionPrompt) : @YES };
  AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)options);

  self.timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
  if (self.timer) {
    @weakify(self);

    dispatch_source_set_timer(self.timer, dispatch_time(DISPATCH_TIME_NOW, 1.0 * NSEC_PER_SEC), 1.0 * NSEC_PER_SEC, 0);
    dispatch_source_set_event_handler(self.timer, ^{
      @strongify(self);
      if (!self) return;

      if (self.trusted) {
        self.timer = nil;
        [[NSNotificationCenter defaultCenter] postNotificationName:kProcessTrustedNotification object:nil];
      }
    });
    dispatch_resume(self.timer);
  }
}

- (void)stop {
  self.timer = nil;
}

@end
