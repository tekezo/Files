//
//  pseudoAppDelegate.m
//  pseudo
//
//  Created by Takayama Fumihiko on 11/04/29.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "pseudoAppDelegate.h"

@implementation pseudoAppDelegate

@synthesize window;

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  [label_ setStringValue:[[NSBundle mainBundle] bundleIdentifier]];
}

@end
