//
//  OrbitsAppDelegate.h
//  Orbits
//
//  Created by Filipe Varela on 10/12/12.
//  Copyright 2010 Filipe Varela. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface OrbitsAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
	IBOutlet id orbitView;
}

@property (assign) IBOutlet NSWindow *window;

@end
