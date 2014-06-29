#import "AppDelegate.h"

@implementation AppDelegate

- (void) timerFireMethod:(NSTimer*)timer
{
  dispatch_async(dispatch_get_main_queue(), ^{
    @synchronized(self) {
      [self.window2 setTitle:[NSString stringWithFormat:@"window2 %@", [NSDate date]]];
    }
  });
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  NSRect rect = [[NSScreen mainScreen] frame];

  [self.window1 setTitle:@"window1"];
  [self.window1 setFrameTopLeftPoint:NSMakePoint(100, rect.size.height - 100)];

  [self.window2 setTitle:@"window2"];
  [self.window2 setFrameTopLeftPoint:NSMakePoint(200, rect.size.height - 200)];

  [self.window3 setTitle:@"window3"];
  [self.window3 setFrameTopLeftPoint:NSMakePoint(300, rect.size.height - 300)];

  [NSTimer scheduledTimerWithTimeInterval:0.5
                                   target:self
                                 selector:@selector(timerFireMethod:)
                                 userInfo:nil
                                  repeats:YES];
}

@end
