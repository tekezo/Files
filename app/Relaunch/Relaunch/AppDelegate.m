#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window = _window;

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{}

- (IBAction) relaunch:(id)sender
{
  NSLog(@"Relaunch");
  [NSTask launchedTaskWithLaunchPath:[[NSBundle mainBundle] executablePath] arguments:[NSArray array]];
  [NSApp terminate:self];
}

@end
