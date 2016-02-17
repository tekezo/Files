#import "AnotherWindowController.h"
#import "AnotherWindowObject.h"

@interface AnotherWindowController ()
@property(weak) IBOutlet NSTextField* label;
@property IBOutlet AnotherWindowObject* object;
@end

@implementation AnotherWindowController

- (void)dealloc {
  NSLog(@"AnotherWindowController dealloc");
}

- (void)setLabel {
  self.label.stringValue = [self.object getString];
}

@end
