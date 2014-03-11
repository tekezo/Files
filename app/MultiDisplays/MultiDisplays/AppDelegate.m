#import "AppDelegate.h"

@implementation AppDelegate

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  [self.window setCollectionBehavior:NSWindowCollectionBehaviorStationary];

  for (NSScreen* screen in [NSScreen screens]) {
    // BUG! self.window will be vanished after this orderOut even if we call orderFront.
    [self.window orderOut:self];
    [self.window setFrameOrigin:[screen frame].origin];
    [self.window orderFront:self];
  }
}

@end
