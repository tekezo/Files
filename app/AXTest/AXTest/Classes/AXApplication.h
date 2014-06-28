// -*- Mode: objc; Coding: utf-8; indent-tabs-mode: nil; -*-

@import Cocoa;

@interface AXApplication : NSObject

@property NSRunningApplication* runningApplication;

- (instancetype) initWithRunningApplication:(NSRunningApplication*)runningApplication;

@end
