#import "AppDelegate.h"

@interface AppDelegate () {
  dispatch_queue_t textsHeightQueue_;
}
@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet NSTextField* wrappedTextHeightCalculator;
@property(weak) IBOutlet NSOutlineView* outlineView;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  textsHeightQueue_ = dispatch_queue_create("org.pqrs.Karabiner.OutlineView.textsHeightQueue_", NULL);
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
}

//- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn*)tableColumn byItem:(id)item {
//  return [NSString stringWithFormat:@"index is %@", item[@"index"]];
//}

- (NSView*)outlineView:(NSOutlineView*)outlineView viewForTableColumn:(NSTableColumn*)tableColumn item:(id)item {
  NSTableCellView* result = [outlineView makeViewWithIdentifier:@"mycellview" owner:self];

  result.textField.stringValue = item[@"text"];

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

// heightOfRowByItem will be called before viewForTableColumn.
// So, we need to calculate the height by using wrappedTextHeightCalculator.

- (CGFloat)outlineView:(NSOutlineView*)outlineView heightOfRowByItem:(id)item {
  NSTableColumn* column = [outlineView outlineTableColumn];
  NSLog(@"column %@", column);

  NSInteger preferredMaxLayoutWidth = (NSInteger)(column.width);

  if ([item[@"preferredMaxLayoutWidth"] integerValue] != preferredMaxLayoutWidth) {
    item[@"preferredMaxLayoutWidth"] = @(preferredMaxLayoutWidth);

    self.wrappedTextHeightCalculator.stringValue = item[@"text"];
    self.wrappedTextHeightCalculator.preferredMaxLayoutWidth = preferredMaxLayoutWidth;

    NSSize size = [self.wrappedTextHeightCalculator fittingSize];
    NSLog(@"size: %f,%f", size.width, size.height);
    item[@"height"] = @(size.height);
  }

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
