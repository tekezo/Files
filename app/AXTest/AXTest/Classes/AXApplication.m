#import "AXApplication.h"

@interface AXApplication ()
{
  NSRunningApplication* runningApplication_;
  AXObserverRef observer_;
  AXUIElementRef applicationElement_;
}
@end

@implementation AXApplication

- (instancetype) initWithRunningApplication:(NSRunningApplication*)runningApplication
{
  self = [super init];

  if (self) {
    runningApplication_ = runningApplication;
  }

  return self;
}

@end
