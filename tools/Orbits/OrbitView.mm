//
//  OrbitView.m
//  Orbits
//
//  Created by Filipe Varela on 10/12/12.
//  Copyright 2010 Filipe Varela. All rights reserved.
//

#import "OrbitView.h"
#include "untitled.h"

@implementation OrbitView

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
	CelestialBody *mars = new CelestialBody();
	CelestialBody *sun = new CelestialBody();
	CelestialBody *moon = new CelestialBody();
	CelestialBody *mercury = new CelestialBody();
	
	double d = mars->juliandate();
	
	mercury->orbital.N =  48.3313*0.0174532925 + 3.24587E-5 * d * 0.0174532925;
	mercury->orbital.i = 7.0047*0.0174532925 + 5.00E-8 * d * 0.0174532925;
	mercury->orbital.w = 29.1241*0.0174532925 + 1.01444E-5 * d * 0.0174532925;
	mercury->orbital.a = 0.387098;
	mercury->orbital.e = 0.205635*0.0174532925 + 5.59E-10 * d * 0.0174532925;
	mercury->orbital.M =  168.6562*0.0174532925 + 4.0923344368 * d * 0.0174532925;
	
	moon->orbital.N =  125.1228*0.0174532925 - 0.0529538083 * d * 0.0174532925;
	moon->orbital.i = 5.1454*0.0174532925;
	moon->orbital.w = 318.0634*0.0174532925 + 0.1643573223 * d * 0.0174532925;
	moon->orbital.a = 0.1;
	moon->orbital.e = 0.054900*0.0174532925;
	moon->orbital.M =  115.3654*0.0174532925 + 13.0649929509 * d * 0.0174532925;
	
	mars->orbital.N =  49.5574*0.0174532925 + 2.11081E-5 * d * 0.0174532925;
	mars->orbital.i = 1.8497*0.0174532925 - 1.78E-8 * d * 0.0174532925;
	mars->orbital.w = 286.5016*0.0174532925 + 2.92961E-5 * d * 0.0174532925;
	mars->orbital.a = 1.523688;
	mars->orbital.e = 0.093405*0.0174532925 + 2.516E-9 * d * 0.0174532925;
	mars->orbital.M =  18.6021*0.0174532925 + 0.5240207766 * d * 0.0174532925;
	
	sun->orbital.N = 0.0;
    sun->orbital.i = 0.0;
    sun->orbital.w = 282.9404*0.0174532925 + 4.70935E-5 * d * 0.0174532925;
    sun->orbital.a = 1.000000;
    sun->orbital.e = 0.016709*0.0174532925 - 1.151E-9 * d * 0.0174532925;
    sun->orbital.M = 356.0470*0.0174532925 + 0.9856002585 * d * 0.0174532925;
	
	double mars_position[3];
	double sun_position[3];
	double moon_position[3];
	double mercury_position[3];
	
	mars->position(mars_position);
	sun->position(sun_position);
	moon->position(moon_position);
	mercury->position(mercury_position);
	
	NSPoint center = NSMakePoint([self bounds].size.width/2, [self bounds].size.height/2);
	
	// draw sun at center
	NSRect sunRect = NSMakeRect(center.x-10, center.y-10, 20, 20);
	NSBezierPath *sunPath = [NSBezierPath bezierPathWithOvalInRect:sunRect];
	[sunPath setLineWidth:2];
	[[NSColor grayColor] set]; 
	[sunPath stroke];
	[[NSColor redColor] set]; 
	[sunPath fill];
	
	// draw earth (-sun) (75px per AU)
	sun_position[0] *= 100;
	sun_position[1] *= -100;
	
	mercury_position[0] *= 100;
	mercury_position[1] *= -100;
	
	mars_position[0] *= 100;
	mars_position[1] *= -100;
	
	moon_position[0] *= 100;
	moon_position[1] *= -100;
	
	sun_position[0] += 300;
	sun_position[1] += 300;
	
	mercury_position[0] += 300;
	mercury_position[1] += 300;
	
	moon_position[0] += sun_position[0];
	moon_position[1] += sun_position[1];
	
	mars_position[0] += 300;
	mars_position[1] += 300;
	
	NSRect earthRect = NSMakeRect(sun_position[1]-2.5, sun_position[0]-2.5, 5, 5);
	NSBezierPath *earthPath = [NSBezierPath bezierPathWithOvalInRect:earthRect];
	[earthPath setLineWidth:2];
	[[NSColor blueColor] set]; 
	[earthPath stroke];
	
	NSRect moonRect = NSMakeRect(moon_position[1]-1, moon_position[0]-1, 2, 2);
	NSBezierPath *moonPath = [NSBezierPath bezierPathWithOvalInRect:moonRect];
	[moonPath setLineWidth:2];
	[[NSColor grayColor] set]; 
	[moonPath stroke];
	
	NSRect marsRect = NSMakeRect(mars_position[1]-2.5, mars_position[0]-2.5, 5, 5);
	NSBezierPath *marsPath = [NSBezierPath bezierPathWithOvalInRect:marsRect];
	[marsPath setLineWidth:2];
	[[NSColor brownColor] set]; 
	[marsPath stroke];
	
	NSRect mercuryRect = NSMakeRect(mercury_position[1]-2.5, mercury_position[0]-2.5, 5, 5);
	NSBezierPath *mercuryPath = [NSBezierPath bezierPathWithOvalInRect:mercuryRect];
	[mercuryPath setLineWidth:2];
	[[NSColor redColor] set]; 
	[mercuryPath stroke];
	// out height =  AU
	
	delete mars;
	delete sun;
}

@end
