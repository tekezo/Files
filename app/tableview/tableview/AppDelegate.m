#import "AppDelegate.h"
#import "AnotherWindowController.h"

@interface AppDelegate ()
@property(weak) IBOutlet NSWindow* window;
@property(weak) IBOutlet NSOutlineView* outlineView;
@property AnotherWindowController* anotherWindowController;
@end

@implementation AppDelegate

- (void)observer_NSWindowWillCloseNotification:(NSNotification*)notification {
  dispatch_async(dispatch_get_main_queue(), ^{
    NSWindow* window = [notification object];
    if (self.anotherWindowController &&
        self.anotherWindowController.window == window) {
      self.anotherWindowController = nil;
    }
  });
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(observer_NSWindowWillCloseNotification:)
                                               name:NSWindowWillCloseNotification
                                             object:nil];
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
