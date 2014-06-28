// -*- Mode: objc; Coding: utf-8; indent-tabs-mode: nil; -*-

@import Cocoa;

@interface AXApplicationObserver : NSObject

- (instancetype) initWithRunningApplication:(NSRunningApplication*)runningApplication;
- (void) unregisterTitleChangedNotification;
- (void) registerTitleChangedNotification;
- (void) postNotification;

@end
