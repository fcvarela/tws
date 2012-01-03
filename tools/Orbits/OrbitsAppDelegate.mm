//
//  OrbitsAppDelegate.m
//  Orbits
//
//  Created by Filipe Varela on 10/12/12.
//  Copyright 2010 Filipe Varela. All rights reserved.
//

#import "OrbitsAppDelegate.h"
#include "untitled.h"

@implementation OrbitsAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	[self redraw: self];
}

- (void)redraw:(id)sender {
	[orbitView setNeedsDisplay: YES];
	[self performSelector:@selector(redraw:) withObject:nil afterDelay:1/15.0f];	
}

@end
