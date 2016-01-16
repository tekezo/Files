#import "CheckboxCellView.h"
#import "OutlineViewText.h"

@implementation OutlineViewText

- (BOOL)validateProposedFirstResponder:(NSResponder *)responder forEvent:(NSEvent *)event {
  return YES;
}

- (void)mouseDown:(NSEvent *)theEvent {
  [super mouseDown:theEvent];

  // Forward the click to the row's cell view
  NSPoint point = [self convertPoint:theEvent.locationInWindow fromView:nil];
  NSInteger row = [self rowAtPoint:point];
  if (row >= 0) {
    CheckboxCellView* view = [self viewAtColumn:0 row:row makeIfNecessary:NO];
    [view toggleCheckboxState];
  }
}

@end
