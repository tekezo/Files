#import "AppDelegate.h"

@interface AppDelegate ()
@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet NSOutlineView* outlineView;
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

@end
