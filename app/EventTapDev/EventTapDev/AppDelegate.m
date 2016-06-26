#import "AppDelegate.h"
#import "AXProcessTrustedObserver.h"
#import "weakify.h"

@interface AppDelegate ()

@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet AXProcessTrustedObserver* axProcessTrustedObserver;

@property NSMutableArray* observers;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  self.observers = [NSMutableArray new];

  {
    @weakify(self);

    id observer = [[NSNotificationCenter defaultCenter] addObserverForName:kProcessTrustedNotification
                                                                    object:nil
                                                                     queue:[NSOperationQueue mainQueue]
                                                                usingBlock:^(NSNotification* notification) {
                                                                  @strongify(self);
                                                                  if (!self) return;

                                                                  NSLog(@"Process Trusted");
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

@end
