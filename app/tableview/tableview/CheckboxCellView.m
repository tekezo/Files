#import "CheckboxCellView.h"

@implementation CheckboxCellView

- (BOOL)validateProposedFirstResponder:(NSResponder *)responder forEvent:(NSEvent *)event {
  return YES;
}

- (IBAction)labelClicked:(id)sender {
  NSLog(@"labelClicked");
}

- (void)toggleCheckboxState {
  if (self.checkbox.enabled) {
    if (self.checkbox.state == NSOnState) {
      self.checkbox.state = NSOffState;
    } else {
      self.checkbox.state = NSOnState;
    }
  }
}

@end
