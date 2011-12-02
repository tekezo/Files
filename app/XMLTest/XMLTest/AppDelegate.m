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

  //xmlstring = @"#{aaa}#{xxx}#{yyy}#{zzz} #{aaa} #{xxx} #{yyy} #{zzz}";

  NSRange range = NSMakeRange(0, [xmlstring length]);
  NSStringCompareOptions options = NSLiteralSearch;

  NSString* replacementtarget[] = {@"#{aaa}",   @"#{xxx}", @"#{yyy}", @"#{zzz}"};
  NSString* replacementvalue[]  = {@"AAAAAAAA", @"XXXXXX", @"Y",      @""};

  for (;;) {
    NSRange replacementBegin = [xmlstring rangeOfString:@"#{" options:options range:range];
    if (replacementBegin.location == NSNotFound) break;
    NSLog(@"Begin location:%ld, length:%ld", replacementBegin.location, replacementBegin.length);

    range.location = replacementBegin.location + 1;
    range.length = [xmlstring length] - range.location;
    NSRange replacementEnd = [xmlstring rangeOfString:@"}" options:options range:range];
    if (replacementEnd.location == NSNotFound) break;
    NSLog(@"End location:%ld, length:%ld", replacementEnd.location, replacementEnd.length);

    for (int i = 0; i < 100; ++i) {
      NSRange replacementRange = NSMakeRange(replacementBegin.location,
                                             replacementEnd.location + 1 - replacementBegin.location);
      if (replacementRange.location + replacementRange.length > [xmlstring length]) {
        break;
      }
      NSRange targetRange = [xmlstring rangeOfString:replacementtarget[i % 4] options:options range:replacementRange];
      if (targetRange.location != NSNotFound) {
        xmlstring = [xmlstring stringByReplacingOccurrencesOfString:replacementtarget[i % 4]
                                                         withString:replacementvalue[i % 4]
                                                            options:options
                                                              range:replacementRange];
      }
    }

    range.location = replacementBegin.location + 1;
    if (range.location > [xmlstring length]) {
      break;
    }
    range.length = [xmlstring length] - range.location;
  }

  NSLog(@">%@<", xmlstring);
}

@end
