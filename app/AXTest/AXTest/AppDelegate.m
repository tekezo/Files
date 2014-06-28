#import "AXApplication.h"
#import "AXUtilities.h"
#import "AppDelegate.h"
#import "NotificationKeys.h"

@interface AppDelegate ()
{
  NSMutableDictionary* observers_;
}
@end

@implementation AppDelegate

- (void) observer_NSWorkspaceDidActivateApplicationNotification:(NSNotification*)notification
{
  dispatch_async(dispatch_get_main_queue(), ^{
    NSRunningApplication* runningApplication = [notification userInfo][NSWorkspaceApplicationKey];
    pid_t pid = [runningApplication processIdentifier];
    observers_[@(pid)] = [[AXApplication alloc] initWithRunningApplication:runningApplication];
  });
}

- (void) observer_kFocusedUIElementChanged:(NSNotification*)notification
{
  NSLog(@"%@", [notification userInfo]);
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  observers_ = [NSMutableDictionary new];

  NSDictionary* options = @{ (__bridge NSString*)(kAXTrustedCheckOptionPrompt): @YES };
  BOOL accessibilityEnabled = AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)options);

  if (! accessibilityEnabled) {
    [[NSAlert alertWithMessageText:@"Please permit the accessibility features."
                     defaultButton:nil
                   alternateButton:nil
                       otherButton:nil
         informativeTextWithFormat:@""] runModal];
  }

  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceDidActivateApplicationNotification:)
                                                             name:NSWorkspaceDidActivateApplicationNotification
                                                           object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(observer_kFocusedUIElementChanged:)
                                               name:kFocusedUIElementChanged
                                             object:nil];

  for (NSRunningApplication* runningApplication in [[NSWorkspace sharedWorkspace] runningApplications]) {
    pid_t pid = [runningApplication processIdentifier];
    observers_[@(pid)] = [[AXApplication alloc] initWithRunningApplication:runningApplication];
  }
}

@end
