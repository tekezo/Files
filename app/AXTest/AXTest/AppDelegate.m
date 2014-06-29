#import "AXApplicationObserverManager.h"
#import "AppDelegate.h"
#import "NotificationKeys.h"
#import "WindowObserver.h"

@interface AppDelegate ()
{
  AXApplicationObserverManager* axApplicationObserverManager_;
  WindowObserver* windowObserver_;
}
@end

@implementation AppDelegate

- (void) observer_kFocusedUIElementChanged:(NSNotification*)notification
{
  NSLog(@"%@", [notification userInfo]);
}

- (void) observer_kWindowVisibilityChanged:(NSNotification*)notification
{
  NSLog(@"%@", [notification userInfo]);
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  NSDictionary* options = @{ (__bridge NSString*)(kAXTrustedCheckOptionPrompt): @YES };
  BOOL accessibilityEnabled = AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)options);

  if (! accessibilityEnabled) {
    [[NSAlert alertWithMessageText:@"Please permit the accessibility features."
                     defaultButton:nil
                   alternateButton:nil
                       otherButton:nil
         informativeTextWithFormat:@""] runModal];
  }

  // ----------------------------------------
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(observer_kFocusedUIElementChanged:)
                                               name:kFocusedUIElementChanged
                                             object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(observer_kWindowVisibilityChanged:)
                                               name:kWindowVisibilityChanged
                                             object:nil];

  axApplicationObserverManager_ = [AXApplicationObserverManager new];
  windowObserver_ = [WindowObserver new];
}

@end
