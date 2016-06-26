// -*- Mode: objc -*-

@import Cocoa;

#define kProcessTrustedNotification @"kProcessTrustedNotification"

@interface AXProcessTrustedObserver : NSObject

@property(readonly) BOOL trusted;

- (void)start;
- (void)stop;

@end
