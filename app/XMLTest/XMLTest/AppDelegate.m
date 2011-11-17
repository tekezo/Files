//
//  AppDelegate.m
//  XMLTest
//
//  Created by Takayama Fumihiko on 11/11/16.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window = _window;

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  NSError* error = nil;
  NSString* xmlstring = [NSString stringWithContentsOfFile:@"/Library/org.pqrs/KeyRemap4MacBook/prefpane/checkbox.xml"
                                                  encoding:NSUTF8StringEncoding
                                                     error:&error];

  xmlstring = [xmlstring stringByReplacingOccurrencesOfString:@"ReplacementTarget::MS_OFFICE"
                                                   withString:@"WORD,EXCEL,POWERPOINT"];

  NSLog(@"%@", xmlstring);
}

@end
