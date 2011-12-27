//
//  AppDelegate.m
//  TISTest
//
//  Created by Takayama Fumihiko on 11/11/10.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import <Carbon/Carbon.h>

@implementation AppDelegate

- (void) dealloc
{
  [super dealloc];
}

- (void) loadXML
{
  NSURL* url = [NSURL fileURLWithPath:@"/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources/languagedef.xml"];
  NSError* error = nil;
  NSXMLDocument* xmldocument = [[NSXMLDocument alloc] initWithContentsOfURL:url options:0 error:&error];
  NSLog(@"%@", xmldocument);
}

- (void) dumpInputSource
{
  CFDictionaryRef filter = NULL;
  CFArrayRef list = NULL;

  const void* keys[] = {
    kTISPropertyInputSourceIsSelectCapable,
  };
  const void* values[] = {
    kCFBooleanTrue,
  };

  filter = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
  if (! filter) goto finish;

  //list = TISCreateInputSourceList(filter, false);
  list = TISCreateInputSourceList(filter, true);
  if (! list) goto finish;

  for (int i = 0; i < CFArrayGetCount(list); ++i) {
    TISInputSourceRef source = (TISInputSourceRef)(CFArrayGetValueAtIndex(list, i));
    if (! source) continue;

    NSLog(@"kTISPropertyInputSourceID: %@", TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
    NSLog(@"kTISPropertyInputModeID: %@", TISGetInputSourceProperty(source, kTISPropertyInputModeID));

    NSArray* languages = TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
    if (languages && [languages count] > 0) {
      NSString* lang = [languages objectAtIndex:0];
      NSLog(@"lang: %@", lang);
    }

    NSLog(@"--------------------");
  }

finish:
  if (filter) {
    CFRelease(filter);
  }
  if (list) {
    CFRelease(list);
  }
}

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification
{
  [self loadXML];
  [self dumpInputSource];
}

@end
