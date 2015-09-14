//
//  AppDelegate.m
//  NSFPlayer
//
//  Created by David Naylor on 9/13/15.
//  Copyright (c) 2015 David Naylor. All rights reserved.
//

#import "AppDelegate.h"
#import "nsf.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    // TEST: temporary testing running cpp nsfplayer from objc
    play_song("/Users/dnaylor/Desktop/NSF-06-01-2011/k/Kirby's Adventure [Hoshi no Kirby - Yume no Izumi no Monogatari] (1993)(HAL Laboratory)(Nintendo).nsf", 4);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
