#import "AXApplication.h"
#import "AXUtilities.h"
#import "NotificationKeys.h"

@interface AXApplication ()

{
  AXUIElementRef applicationElement_;
  AXUIElementRef focusedWindowElementForAXTitleChangedNotification_;
  AXObserverRef observer_;
}

@property NSRunningApplication* runningApplication;
@property NSString* title;
@property NSString* role;

- (void) registerTitleChangedNotification;
- (void) updateTitle;
- (void) updateRole:(AXUIElementRef)element;

@end

static void
observerCallback(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void* refcon)
{
  AXApplication* self = (__bridge AXApplication*)(refcon);
  if (! self) return;

  @synchronized(self) {
    if (CFStringCompare(notification, kAXTitleChangedNotification, 0) == kCFCompareEqualTo) {
      [self updateTitle];
    }
    if (CFStringCompare(notification, kAXFocusedUIElementChangedNotification, 0) == kCFCompareEqualTo) {
      [self updateTitle];
      [self updateRole:element];
    }
    if (CFStringCompare(notification, kAXFocusedWindowChangedNotification, 0) == kCFCompareEqualTo) {
      // ----------------------------------------
      // refresh notification.
      [self registerTitleChangedNotification];

      [self updateTitle];
    }

    NSDictionary* userInfo = @{
      @"runningApplication" : self.runningApplication,
      @"notification" : (__bridge NSString*)(notification),
      @"title": self.title,
    };
    [[NSNotificationCenter defaultCenter] postNotificationName:kFocusedUIElementChanged object:self userInfo:userInfo];
  }
}

@implementation AXApplication

- (instancetype) initWithRunningApplication:(NSRunningApplication*)runningApplication
{
  self = [super init];

  if (self) {
    self.runningApplication = runningApplication;
    self.title = @"";
    self.role = @"";

    pid_t pid = [self.runningApplication processIdentifier];

    // ----------------------------------------
    // Create applicationElement_

    applicationElement_ = AXUIElementCreateApplication(pid);
    if (! applicationElement_) {
      NSLog(@"AXUIElementCreateApplication is failed. %@", self.runningApplication);
      goto finish;
    }

    // ----------------------------------------
    // Create observer_

    AXError error = AXObserverCreate(pid, observerCallback, &observer_);
    if (error != kAXErrorSuccess) {
      NSLog(@"AXObserverCreate is failed. error:%d %@", error, self.runningApplication);
      goto finish;
    }

    // ----------------------------------------
    // Observe notifications

    [self observeAXNotification:applicationElement_ notification:kAXFocusedUIElementChangedNotification add:YES];
    [self observeAXNotification:applicationElement_ notification:kAXFocusedWindowChangedNotification add:YES];
    [self registerTitleChangedNotification];

    [self updateTitle];
    [self updateRole:NULL];

    // ----------------------------------------
    CFRunLoopAddSource(CFRunLoopGetCurrent(),
                       AXObserverGetRunLoopSource(observer_),
                       kCFRunLoopDefaultMode);
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
  if (! element) return YES;
  if (! observer_) return YES;

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
      NSLog(@"AXObserverAddNotification is failed: error:%d %@", error, self.runningApplication);
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
      NSLog(@"AXObserverRemoveNotification is failed: error:%d %@", error, self.runningApplication);
      return NO;
    }
  }

  return YES;
}

- (void) unregisterTitleChangedNotification
{
  if (focusedWindowElementForAXTitleChangedNotification_) {
    [self observeAXNotification:focusedWindowElementForAXTitleChangedNotification_
                   notification:kAXTitleChangedNotification
                            add:NO];

    focusedWindowElementForAXTitleChangedNotification_ = NULL;
  }
}

- (void) registerTitleChangedNotification
{
  if (! applicationElement_) return;

  [self unregisterTitleChangedNotification];

  focusedWindowElementForAXTitleChangedNotification_ = [AXUtilities copyFocusedWindow:applicationElement_];
  if (! focusedWindowElementForAXTitleChangedNotification_) return;

  [self observeAXNotification:focusedWindowElementForAXTitleChangedNotification_
                 notification:kAXTitleChangedNotification
                          add:YES];
}

- (void) updateTitle
{
  self.title = @"";

  if (! applicationElement_) return;

  // Do not cache focusedWindowElement.
  // We need to get new focusedWindowElement because
  // getting title will be failed with cached focusedWindowElement on Finder.app.

  AXUIElementRef focusedWindowElement = [AXUtilities copyFocusedWindow:applicationElement_];
  if (focusedWindowElement) {
    NSString* title = [AXUtilities titleOfUIElement:focusedWindowElement];
    if (title) {
      self.title = title;
    }
    CFRelease(focusedWindowElement);
  }
}

- (void) updateRole:(AXUIElementRef)element
{
  self.role = @"";

  if (element) {
    CFRetain(element);
  } else {
    element = [AXUtilities copyFocusedUIElement];
  }
  if (element) {
    NSString* role = [AXUtilities roleOfUIElement:element];
    if (role) {
      self.role = role;
    }
    CFRelease(element);
  }
}

@end
