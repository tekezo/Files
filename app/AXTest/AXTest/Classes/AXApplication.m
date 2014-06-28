#import "AXApplication.h"

static void
observerCallback(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void* refcon)
{
  NSLog(@"observerCallback %@", (__bridge NSString*)(notification));
}

@interface AXApplication ()
{
  NSRunningApplication* runningApplication_;
  AXUIElementRef applicationElement_;
  AXObserverRef observer_;
}
@end

@implementation AXApplication

- (instancetype) initWithRunningApplication:(NSRunningApplication*)runningApplication
{
  self = [super init];

  if (self) {
    runningApplication_ = runningApplication;

    pid_t pid = [runningApplication_ processIdentifier];

    // ----------------------------------------
    // Create applicationElement_

    applicationElement_ = AXUIElementCreateApplication(pid);
    if (! applicationElement_) {
      NSLog(@"AXUIElementCreateApplication is failed. %@", runningApplication_);
      goto finish;
    }

    // ----------------------------------------
    // Create observer_

    AXError error = AXObserverCreate(pid, observerCallback, &observer_);
    if (error != kAXErrorSuccess) {
      NSLog(@"AXObserverCreate is failed. error:%d %@", error, runningApplication_);
      goto finish;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(),
                       AXObserverGetRunLoopSource(observer_),
                       kCFRunLoopDefaultMode);

    // ----------------------------------------
    // Observe notifications
    [self observeAXNotification:applicationElement_ notification:kAXFocusedUIElementChangedNotification add:YES];
    [self observeAXNotification:applicationElement_ notification:kAXFocusedWindowChangedNotification add:YES];
  }

finish:
  return self;
}

- (void) dealloc
{
  if (observer_) {
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(),
                          AXObserverGetRunLoopSource(observer_),
                          kCFRunLoopDefaultMode);
    CFRelease(observer_);
    observer_ = NULL;
  }

  if (applicationElement_) {
    CFRelease(applicationElement_);
    applicationElement_ = NULL;
  }
}

- (BOOL) observeAXNotification:(AXUIElementRef)element notification:(CFStringRef)notification add:(BOOL)add
{
  if (! observer_) return YES;
  if (! element) return YES;

  if (add) {
    AXError error = AXObserverAddNotification(observer_,
                                              element,
                                              notification,
                                              (__bridge void*)self);
    if (error != kAXErrorSuccess) {
      if (error == kAXErrorNotificationUnsupported ||
          error == kAXErrorNotificationAlreadyRegistered) {
        // We ignore this error.
        return YES;
      }
      NSLog(@"AXObserverAddNotification is failed: error:%d %@", error, runningApplication_);
      return NO;
    }

  } else {
    AXError error = AXObserverRemoveNotification(observer_,
                                                 element,
                                                 notification);
    if (error != kAXErrorSuccess) {
      // Note: Ignore kAXErrorInvalidUIElement because it is expected error when focused window is closed.
      if (error == kAXErrorInvalidUIElement ||
          error == kAXErrorNotificationNotRegistered) {
        // We ignore this error.
        return YES;
      }
      NSLog(@"AXObserverRemoveNotification is failed: error:%d %@", error, runningApplication_);
      return NO;
    }
  }

  return YES;
}

@end
