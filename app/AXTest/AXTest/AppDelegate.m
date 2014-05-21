#import "AppDelegate.h"
#import "AXUtilities.h"

@implementation AppDelegate

static void observerCallback(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void* refcon)
{
  NSLog(@"observerCallback %@", (__bridge NSString*)(notification));

  if (CFStringCompare(notification, kAXTitleChangedNotification, 0) == kCFCompareEqualTo) {
    NSLog(@"title changed: %@", [AXUtilities titleOfUIElement:element]);
  } else if (CFStringCompare(notification, kAXFocusedWindowChangedNotification, 0) == kCFCompareEqualTo) {
    NSLog(@"focused window changed: %@", [AXUtilities titleOfUIElement:element]);
  }
}

- (void) registerApplication:(NSRunningApplication*)runningApplication
{
  pid_t processIdentifier = [runningApplication processIdentifier];

  AXUIElementRef application = AXUIElementCreateApplication(processIdentifier);

  AXObserverRef observer;
  AXObserverCreate(processIdentifier, observerCallback, &observer);

  AXError error = kAXErrorSuccess;
  error = AXObserverAddNotification(observer, application, kAXTitleChangedNotification,         (__bridge void*)self);
  error = AXObserverAddNotification(observer, application, kAXWindowCreatedNotification,        (__bridge void*)self);
  error = AXObserverAddNotification(observer, application, kAXFocusedWindowChangedNotification, (__bridge void*)self);
  error = AXObserverAddNotification(observer, application, kAXFocusedUIElementChangedNotification, (__bridge void*)self);

  CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop],
                     AXObserverGetRunLoopSource(observer),
                     kCFRunLoopDefaultMode);
}

- (void) observer_NSWorkspaceDidLaunchApplicationNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceDidLaunchApplicationNotification");

  NSRunningApplication* runningApplication = [notification userInfo][NSWorkspaceApplicationKey];
  [self registerApplication:runningApplication];
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

  } else {
    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                           selector:@selector(observer_NSWorkspaceDidLaunchApplicationNotification:)
                                                               name:NSWorkspaceDidLaunchApplicationNotification
                                                             object:nil];

    for (NSRunningApplication* runningApplication in [[NSWorkspace sharedWorkspace] runningApplications]) {
      [self registerApplication:runningApplication];
    }
  }
}

// call AXObserverRemoveNotification

@end
