#import "AppDelegate.h"
#import "AXUtilities.h"

@interface Observer : NSObject
@property AXObserverRef observer;
@property AXUIElementRef application;
- (AXObserverRef*) getRef;
@end

@implementation Observer

- (AXObserverRef*) getRef
{
  return &_observer;
}

@end

@interface AppDelegate ()
{
  NSMutableDictionary* observers_;
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
  pid_t pid = [runningApplication processIdentifier];

  {
    Observer* o = observers_[@(pid)];
    if (o) {
      CFRunLoopRemoveSource(CFRunLoopGetCurrent(),
                            AXObserverGetRunLoopSource(o.observer),
                            kCFRunLoopDefaultMode);
      CFRelease(o.observer);
      [observers_ removeObjectForKey:@(pid)];
    }
  }

  // ----------------------------------------
  if (! AXIsProcessTrusted()) return;

  // ----------------------------------------
  AXError error = kAXErrorSuccess;
  AXUIElementRef application = NULL;

  Observer* o = [Observer new];
  observers_[@(pid)] = o;

  error = AXObserverCreate(pid, observerCallback, [o getRef]);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverCreate is failed. pid:%d error:%d", pid, error);
    goto finish;
  }

  CFRunLoopAddSource(CFRunLoopGetCurrent(),
                     AXObserverGetRunLoopSource(o.observer),
                     kCFRunLoopDefaultMode);

  application = AXUIElementCreateApplication(pid);
  if (! application) {
    NSLog(@"AXUIElementCreateApplication is failed. pid:%d", pid);
    goto finish;
  }

  error = AXObserverAddNotification(o.observer, application, kAXTitleChangedNotification,            (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
  }
  error = AXObserverAddNotification(o.observer, application, kAXWindowCreatedNotification,           (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
  }
  error = AXObserverAddNotification(o.observer, application, kAXFocusedWindowChangedNotification,    (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
  }
  error = AXObserverAddNotification(o.observer, application, kAXFocusedUIElementChangedNotification, (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
  }
  error = AXObserverAddNotification(o.observer, application, kAXApplicationActivatedNotification,    (__bridge void*)self);
  if (error != kAXErrorSuccess) {
    NSLog(@"AXObserverAddNotification is failed: pid:%d error:%d", pid, error);
  }

finish:
  // Do not release application til you do not need notifications.
  NSLog(@"registered %@", runningApplication);
}

- (void) observer_NSWorkspaceDidActivateApplicationNotification:(NSNotification*)notification
{
  dispatch_async(dispatch_get_main_queue(), ^{
#if 0
      NSRunningApplication* runningApplication = [notification userInfo][NSWorkspaceApplicationKey];
      [self registerApplication:runningApplication];
#else
      for (NSRunningApplication* runningApplication in [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.dock"]) {
        [self registerApplication:runningApplication];
      }
#endif
  });
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

  for (NSRunningApplication* runningApplication in [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.dock"]) {
    [self registerApplication:runningApplication];
  }
}

@end
