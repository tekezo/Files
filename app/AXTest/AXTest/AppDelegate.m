#import "AppDelegate.h"
#import "AXUtilities.h"

@interface AppDelegate ()
{
  AXObserverRef observer_;
}
@end

@implementation AppDelegate

static void observerCallback(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void* refcon)
{
  NSLog(@"observerCallback %@", (__bridge NSString*)(notification));

  if (CFStringCompare(notification, kAXTitleChangedNotification, 0) == kCFCompareEqualTo) {
    NSLog(@"title changed: %@", [AXUtilities titleOfUIElement:element]);
  } else if (CFStringCompare(notification, kAXFocusedWindowChangedNotification, 0) == kCFCompareEqualTo) {
    NSLog(@"focused window changed: %@", [AXUtilities titleOfUIElement:element]);
  } else if (CFStringCompare(notification, kAXFocusedUIElementChangedNotification, 0) == kCFCompareEqualTo) {
    NSLog(@"role of element: %@", [AXUtilities roleOfUIElement:element]);
  }
}

- (void) registerApplication:(NSRunningApplication*)runningApplication
{
  // ----------------------------------------
  // unregister
  if (observer_) {
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(),
                          AXObserverGetRunLoopSource(observer_),
                          kCFRunLoopDefaultMode);

    CFRelease(observer_);
    observer_ = NULL;
  }

  // ----------------------------------------
  if (! AXIsProcessTrusted()) return;

  // ----------------------------------------
  AXError error = kAXErrorSuccess;
  AXUIElementRef application = NULL;

  pid_t pid = [runningApplication processIdentifier];

  error = AXObserverCreate(pid, observerCallback, &observer_);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverCreate is failed. pid:%d error:%d", pid, error);
    goto finish;
  }

  application = AXUIElementCreateApplication(pid);
  if (! application) {
    NSLog(@"AXUIElementCreateApplication is failed. pid:%d", pid);
    goto finish;
  }

  error = AXObserverAddNotification(observer_, application, kAXTitleChangedNotification,            (__bridge void*)self);
  error = AXObserverAddNotification(observer_, application, kAXWindowCreatedNotification,           (__bridge void*)self);
  error = AXObserverAddNotification(observer_, application, kAXFocusedWindowChangedNotification,    (__bridge void*)self);
  error = AXObserverAddNotification(observer_, application, kAXFocusedUIElementChangedNotification, (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
    goto finish;
  }

  CFRunLoopAddSource(CFRunLoopGetCurrent(),
                     AXObserverGetRunLoopSource(observer_),
                     kCFRunLoopDefaultMode);

finish:
  if (application) {
    CFRelease(application);
    application = NULL;
  }
}

- (void) observer_NSWorkspaceDidActivateApplicationNotification:(NSNotification*)notification
{
  dispatch_async(dispatch_get_main_queue(), ^{
      NSRunningApplication* runningApplication = [notification userInfo][NSWorkspaceApplicationKey];
      [self registerApplication:runningApplication];

      AXUIElementRef element = AXUIElementCreateSystemWide();
      if (element) {
        NSLog(@"AXUIElementCreateSystemWide role of element: %@", [AXUtilities roleOfUIElement:element]);
        CFRelease(element);
      }
  });
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

  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceDidActivateApplicationNotification:)
                                                             name:NSWorkspaceDidActivateApplicationNotification
                                                           object:nil];
}

@end
