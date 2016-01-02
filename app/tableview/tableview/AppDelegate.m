#import "AppDelegate.h"

@interface AppDelegate ()
@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet NSTextFieldCell* wrappedTextHeightCalculator;
@property(weak) IBOutlet NSOutlineView* outlineView;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
}

//- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn*)tableColumn byItem:(id)item {
//  return [NSString stringWithFormat:@"index is %@", item[@"index"]];
//}

- (NSView*)outlineView:(NSOutlineView*)outlineView viewForTableColumn:(NSTableColumn*)tableColumn item:(id)item {
  NSTableCellView* result = [outlineView makeViewWithIdentifier:@"mycellview" owner:self];

  result.textField.stringValue = [NSString stringWithFormat:@"index is %@. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.", item[@"index"]];

  NSInteger preferredMaxLayoutWidth = (NSInteger)(tableColumn.width);
  result.textField.preferredMaxLayoutWidth = preferredMaxLayoutWidth;

  if ([item[@"preferredMaxLayoutWidth"] integerValue] != preferredMaxLayoutWidth) {
    item[@"preferredMaxLayoutWidth"] = @(preferredMaxLayoutWidth);

    NSSize size = [result.textField fittingSize];
    NSLog(@"size: %f,%f", size.width, size.height);
    item[@"height"] = @(size.height);
  }

  return result;
}

- (CGFloat)outlineView:(NSOutlineView*)outlineView heightOfRowByItem:(id)item {

  NSTableColumn* column = [outlineView outlineTableColumn];
  NSLog(@"column %@", column);

  NSInteger height = [item[@"height"] integerValue];
  if (height == 0) {
    return [outlineView rowHeight];
  } else {
    return height;
  }
}

- (void)outlineView:(NSOutlineView*)outlineView didAddRowView:(NSTableRowView*)rowView forRow:(NSInteger)row {
}

- (IBAction)reload:(id)sender {
  [self.outlineView reloadData];
}

@end
