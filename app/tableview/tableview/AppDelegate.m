#import "AppDelegate.h"
#import "AnotherWindowController.h"

@interface AppDelegate ()
@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet NSOutlineView* outlineView;
@property AnotherWindowController* anotherWindowController;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
}

- (void)windowDidResize:(NSNotification*)notification {
  [self reload:self];
}

- (IBAction)reload:(id)sender {
  [self.outlineView reloadData];
}

- (IBAction)newAnotherWindow:(id)sender {
  self.anotherWindowController = [[AnotherWindowController alloc] initWithWindowNibName:@"AnotherWindow"];
  [self.anotherWindowController showWindow:self];
  [self.anotherWindowController setLabel];
}

@end
