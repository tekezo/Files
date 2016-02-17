#import "AnotherWindowObject.h"

@implementation AnotherWindowObject

- (void)dealloc {
  NSLog(@"AnotherWindowObject dealloc");
}

- (NSString*)getString {
  return @"Hello!";
}

@end
