#import "AppDelegate.h"

@interface AppDelegate ()

@property(weak) IBOutlet NSWindow* window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
  // Insert code here to tear down your application
}

- (NSInteger)outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item {
  return item == nil ? 5 : 0;
}

- (BOOL)outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item {
  return item == nil;
}

- (id)outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item {
  return @{ @"index" : @(index) };
}

//- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn*)tableColumn byItem:(id)item {
//  return [NSString stringWithFormat:@"index is %@", item[@"index"]];
//}

- (NSView*)outlineView:(NSOutlineView*)outlineView viewForTableColumn:(NSTableColumn*)tableColumn item:(id)item {
  NSTextField* result = [outlineView makeViewWithIdentifier:@"myview" owner:self];

  if (result == nil) {
    result = [NSTextField new];
    result.identifier = @"myview";
    result.lineBreakMode = NSLineBreakByWordWrapping;
  }

  result.stringValue = [NSString stringWithFormat:@"index is %@. Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.", item[@"index"]];
  result.preferredMaxLayoutWidth = tableColumn.width;
  NSLog(@"preferredMaxLayoutWidth: %f", result.preferredMaxLayoutWidth);

  NSSize size = [result fittingSize];
  result.frame = NSMakeRect(0, 0, size.width, size.height);

  return result;
}

@end
