#import "AppDelegate.h"

@interface AppDelegate () {
  NSTimer *timer_;
}

@property(weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)timerFireMethod:(NSTimer *)timer {
  NSDictionary *defaults = [[NSUserDefaults standardUserDefaults] persistentDomainForName:NSGlobalDomain];
  NSLog(@"%@", defaults[@"InitialKeyRepeat"]);
  NSLog(@"%@", defaults[@"KeyRepeat"]);
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  timer_ = [NSTimer scheduledTimerWithTimeInterval:1
                                            target:self
                                          selector:@selector(timerFireMethod:)
                                          userInfo:nil
                                           repeats:YES];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Insert code here to tear down your application
}

@end
