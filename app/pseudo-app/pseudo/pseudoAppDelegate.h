// -*- Mode: objc -*-
//
//  pseudoAppDelegate.h
//  pseudo
//
//  Created by Takayama Fumihiko on 11/04/29.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface pseudoAppDelegate : NSObject <NSApplicationDelegate> {
  @private
  NSWindow* window;
  IBOutlet NSTextField* label_;
}

@property (assign) IBOutlet NSWindow* window;

@end
