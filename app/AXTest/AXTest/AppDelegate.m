#import "AXApplicationObserver.h"
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
    @synchronized(self) {
      for (NSNumber* pid in observers_) {
        [observers_[pid] unobserveTitleChangedNotification];
      }

      NSRunningApplication* runningApplication = [notification userInfo][NSWorkspaceApplicationKey];
      pid_t pid = [runningApplication processIdentifier];

      AXApplicationObserver* o = [[AXApplicationObserver alloc] initWithRunningApplication:runningApplication];
      observers_[@(pid)] = o;

      [o observeTitleChangedNotification];
      [o postNotification];
    }
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
    AXApplicationObserver* app = [[AXApplicationObserver alloc] initWithRunningApplication:runningApplication];
    observers_[@(pid)] = app;
    NSLog(@"observe %@", runningApplication);
  }

  pid_t pid = [[[NSWorkspace sharedWorkspace] frontmostApplication] processIdentifier];
  AXApplicationObserver* o = observers_[@(pid)];
  [o observeTitleChangedNotification];
  [o postNotification];
}

@end
