#import "AXApplication.h"

static void
observerCallback(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void* refcon)
{
  NSLog(@"%@", (__bridge NSString*)(notification));
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

@end
