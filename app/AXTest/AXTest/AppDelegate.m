#import "AXApplicationObserver.h"
#import "AXUtilities.h"
#import "AppDelegate.h"
#import "NotificationKeys.h"
#import "WindowObserver.h"

@interface AppDelegate ()
{
  NSMutableDictionary* observers_;
  WindowObserver* windowObserver_;
  NSTimer* timer_;
  NSRunningApplication* runningApplicationForAXApplicationObserver_;
}
@end

@implementation AppDelegate

- (void) timerFireMethod:(NSTimer*)timer
{
  dispatch_async(dispatch_get_main_queue(), ^{
    @synchronized(self) {
      if (runningApplicationForAXApplicationObserver_) {
        @try {
          AXApplicationObserver* o = [[AXApplicationObserver alloc] initWithRunningApplication:runningApplicationForAXApplicationObserver_];
          pid_t pid = [runningApplicationForAXApplicationObserver_ processIdentifier];
          observers_[@(pid)] = o;

          [o observeTitleChangedNotification];
          [o postNotification];

          runningApplicationForAXApplicationObserver_ = nil;

        } @catch (NSException* e) {
          NSLog(@"%@", e);
        }
      }
    }
  });
}

- (void) observer_NSWorkspaceDidActivateApplicationNotification:(NSNotification*)notification
{
  dispatch_async(dispatch_get_main_queue(), ^{
    @synchronized(self) {
      for (NSNumber* pid in observers_) {
        [observers_[pid] unobserveTitleChangedNotification];
      }

      runningApplicationForAXApplicationObserver_ = [notification userInfo][NSWorkspaceApplicationKey];
      [timer_ fire];
    }
  });
}

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
  observers_ = [NSMutableDictionary new];
  windowObserver_ = [WindowObserver new];

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

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(observer_kWindowVisibilityChanged:)
                                               name:kWindowVisibilityChanged
                                             object:nil];

  for (NSRunningApplication* runningApplication in [[NSWorkspace sharedWorkspace] runningApplications]) {
    @try {
      pid_t pid = [runningApplication processIdentifier];
      AXApplicationObserver* app = [[AXApplicationObserver alloc] initWithRunningApplication:runningApplication];
      observers_[@(pid)] = app;
    } @catch (NSException* e) {
      NSLog(@"%@", e);
    }
  }

  timer_ = [NSTimer scheduledTimerWithTimeInterval:0.5
                                            target:self
                                          selector:@selector(timerFireMethod:)
                                          userInfo:nil
                                           repeats:YES];
  runningApplicationForAXApplicationObserver_ = [[NSWorkspace sharedWorkspace] frontmostApplication];
  [timer_ fire];
}

@end
