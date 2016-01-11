#import "OutlineViewDelegate.h"

@interface OutlineViewDelegate () {
  dispatch_queue_t textsHeightQueue_;
}
@property(weak) IBOutlet NSTextField* wrappedTextHeightCalculator;
@end

@implementation OutlineViewDelegate

- (instancetype)init {
  self = [super init];

  if (self) {
    textsHeightQueue_ = dispatch_queue_create("org.pqrs.Karabiner.OutlineView.textsHeightQueue_", NULL);
  }

  return self;
}

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
    dispatch_sync(textsHeightQueue_, ^{
      item[@"preferredMaxLayoutWidth"] = @(preferredMaxLayoutWidth);

      self.wrappedTextHeightCalculator.stringValue = item[@"text"];
      self.wrappedTextHeightCalculator.preferredMaxLayoutWidth = preferredMaxLayoutWidth;

      NSSize size = [self.wrappedTextHeightCalculator fittingSize];
      NSLog(@"size: %f,%f", size.width, size.height);
      NSInteger margin = 2;
      item[@"height"] = @(size.height + margin * 2);
    });
  }

  NSInteger height = [item[@"height"] integerValue];
  if (height == 0) {
    return [outlineView rowHeight];
  } else {
    return height;
  }
}

@end
