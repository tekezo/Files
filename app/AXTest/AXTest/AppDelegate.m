#import "AppDelegate.h"

@implementation AppDelegate

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  NSDictionary* options = @{ (__bridge NSString*)(kAXTrustedCheckOptionPrompt): @YES };
  BOOL accessibilityEnabled = AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)options);

  while (true) {
    [NSThread sleepForTimeInterval:1];
    NSLog(@"%@", [NSApp accessibilityFocusedUIElement]);
  }
}

@end
