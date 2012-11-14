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

    BOOL categoryfilter = NO;
    NSString* category = TISGetInputSourceProperty(source, kTISPropertyInputSourceCategory);
    if (! [category isEqualToString:(NSString*)(kTISCategoryKeyboardInputSource)]) {
      categoryfilter = YES;
    }
    BOOL sourceidfilter = NO;
    NSString* sourceID = TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
    if ([sourceID hasPrefix:@"com.apple."] &&
        // com.apple.keylayout.French
        ! [sourceID hasPrefix:@"com.apple.keylayout."] &&
        // com.apple.inputmethod.Kotoeri.Roman
        ! [sourceID hasPrefix:@"com.apple.inputmethod."] &&
        // com.apple.keyboardlayout.fr-dvorak-bepo.keylayout.FrenchDvorak
        ! [sourceID hasPrefix:@"com.apple.keyboardlayout."]) {
      sourceidfilter = YES;
    }

    if (categoryfilter != sourceidfilter) {
      NSLog(@"filter is not match");
    }

    NSLog(@"kTISPropertyInputSourceCategory: %@", TISGetInputSourceProperty(source, kTISPropertyInputSourceCategory));
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
  [self dumpInputSource];
}

@end
