//
//  AppDelegate.h
//  SoftwareRenderer
//
//  Created by xiaov.
//

#import <Cocoa/Cocoa.h>

#include "renderer.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    IBOutlet NSImageView *imageView;
    
    IBOutlet NSButton *btnReset;
    
    IBOutlet NSSlider *sliderView;
    
    IBOutlet NSButton *chkLight;
    IBOutlet NSButton *chkWild;
    
    IBOutlet NSButton *chk0;
    IBOutlet NSButton *chk1;
    IBOutlet NSButton *chk2;
    IBOutlet NSButton *chk3;
    IBOutlet NSButton *chk4;
    
    NSImage* image;
    unsigned char * pixels;
    int width;
    int height;
    
    device_t device;
    
    texture_t *tex[10];
}

-(NSImage*)makeNSImage;
-(void)draw;

-(IBAction)btnPressed:(id)sender;
-(IBAction)checkAction:(id)sender;
-(IBAction)sliderAction:(id)sender;

@end

