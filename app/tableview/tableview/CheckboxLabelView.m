#import "CheckboxLabelView.h"

@interface CheckboxLabelView ()
@property(weak) IBOutlet NSButton *checkbox;
@end

@implementation CheckboxLabelView

- (BOOL)validateProposedFirstResponder:(NSResponder *)responder forEvent:(NSEvent *)event {
  return YES;
}

- (void)mouseUp:(NSEvent *)theEvent {
  NSLog(@"mouseUp");
  if (self.checkbox) {
    if (self.checkbox.enabled) {
      if (self.checkbox.state == NSOnState) {
        self.checkbox.state = NSOffState;
      } else {
        self.checkbox.state = NSOnState;
      }
    }
  }
}

@end
