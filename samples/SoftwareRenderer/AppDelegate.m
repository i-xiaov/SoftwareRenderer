//
//  AppDelegate.m
//  SoftwareRenderer
//
//  Created by xiaov.
//

#import "AppDelegate.h"

#import "models/venusl.h"
#import "models/ateneal.h"
#import "banana.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseDragged handler:^(NSEvent *event) {
        switch (event.type) {
            case NSEventTypeLeftMouseDragged: {
                
                float m[16];
                mat4_identity(m);
                
                float vy[] = {0, 1, 0, 0};
                mat4_rotate(m, m, event.deltaX / 100, vy);
                
                float vx[] = {1, 0, 0, 0};
                mat4_rotate(m, m, event.deltaY / 100, vx);
                
                mat4_multiply(device.transform.model, device.transform.model, m);
                
                transform_update(&device.transform);
                
                break;
            }
                
            default:
                break;
        }
        
        [self draw];
        
        return (NSEvent *)nil;
    }];
    
    width = 320 * 2;
    height = 320 * 2;
    
    device_init(&device, width, height);
    device_clear(&device);
    
    float value = [sliderView floatValue];
    float eye[] = {0, 0, value}, center[] = {0, 0, 0}, up[] = {0, 1, 0};
    mat4_look_at(device.transform.view, eye, center, up);
    transform_update(&device.transform);
    
    device_enable_light(&device, 1);
    float light[] = {0, 0, 10};
    float color[] = {1, 1, 1, 1};
    device_light(&device, light, color, 0.2, 0.5, 0.5, 50);
    
    pixels = (unsigned char *)malloc(width * height * 4);
    
    // textures
    NSImage *img;
    NSBitmapImageRep *bmp;
    
    img = [NSImage imageNamed:@"lenna"];
    bmp = (NSBitmapImageRep *)[[img representations] objectAtIndex:0];
    tex[0] = device_gen_texture(0, (int)[bmp pixelsWide], (int)[bmp pixelsHigh], [bmp bitmapData]);
    
    img = [NSImage imageNamed:@"banana"];
    bmp = (NSBitmapImageRep *)[[img representations] objectAtIndex:0];
    tex[1] = device_gen_texture(0, (int)[bmp pixelsWide], (int)[bmp pixelsHigh], [bmp bitmapData]);
    
    [self draw];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    
    device_destroy(&device);
    
    free(pixels);
    
    for (int i = 0; i < 10; i++) {
        if (tex[i]) {
            device_del_texture(tex[i]);
        }
    }
}

-(NSImage*)makeNSImage {
    const int size = width * 4;
    for (int i = 0; i < height; i++) {
        memcpy(pixels + i * size, (unsigned char *)device.framebuffer + (height - i - 1) * size, size);
    }
    
    NSBitmapImageRep * bmp = [[NSBitmapImageRep alloc]initWithBitmapDataPlanes:&pixels pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:4 hasAlpha:YES isPlanar:NO colorSpaceName:NSCalibratedRGBColorSpace bytesPerRow:width * 4 bitsPerPixel:32];
    NSImage* img = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
    [img addRepresentation:bmp];
    return img;
}

-(void)draw {
    device_clear(&device);
    
    if ([chk0 state]) {
        float vertex[] = {
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
            
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
        };
        
        float color[] = {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
        };
        
        float m[16];
        mat4_identity(m);
        
        device_vertex_pointer(&device, 36, vertex);
        device_color_pointer(&device, color);
        device_bind_texture(&device, NULL);
        device_texcoord_pointer(&device, NULL);
        device_normal_pointer(&device, NULL);
        draw_arrays(&device, 0, 36);
    }
    
    if ([chk1 state]) {
        float vertex1[] = {
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            
            -1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f
        };
        
        float texcoord1[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };
        
        float normal1[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
        };
        
        device_vertex_pointer(&device, 6, vertex1);
        device_color_pointer(&device, NULL);
        device_texcoord_pointer(&device, texcoord1);
        device_bind_texture(&device, tex[0]);
        device_normal_pointer(&device, normal1);
        draw_arrays(&device, 0, 6);
    }
    
    if ([chk2 state]) {
        device_vertex_pointer(&device, atenealNumVerts, atenealVerts);
        device_color_pointer(&device, NULL);
        device_bind_texture(&device, NULL);
        device_texcoord_pointer(&device, NULL);
        device_normal_pointer(&device, atenealNormals);
        draw_arrays(&device, 0, atenealNumVerts);
    }
    
    if ([chk3 state]) {
        device_vertex_pointer(&device, bananaNumVerts, bananaVerts);
        device_color_pointer(&device, NULL);
        device_bind_texture(&device, tex[1]);
        device_texcoord_pointer(&device, bananaTexCoords);
        device_normal_pointer(&device, bananaNormals);
        draw_arrays(&device, 0, bananaNumVerts);
        
    }
    
    if ([chk4 state]) {
        device_vertex_pointer(&device, venuslNumVerts, venuslVerts);
        device_color_pointer(&device, NULL);
        device_bind_texture(&device, NULL);
        device_texcoord_pointer(&device, NULL);
        device_normal_pointer(&device, venuslNormals);
        draw_arrays(&device, 0, venuslNumVerts);
    }
        
    image = [self makeNSImage];
    [imageView setImage:image];
}

- (IBAction)btnPressed:(id)sender {
    if (sender == btnReset) {
        mat4_identity(device.transform.model);
        transform_update(&device.transform);
    }
    
    [self draw];
}

-(IBAction)checkAction:(id)sender {
    if (sender == chkLight) {
        if ([chkLight state]) {
            device_enable_light(&device, 1);
        }
        else {
            device_disable_light(&device, 1);
        }
    }
    else if (sender == chkWild) {
        device_draw_mode(&device, [chkWild state] ? (DEVICE_DRAW_MODE_NORMAL | DEVICE_DRAW_MODE_WILD) : DEVICE_DRAW_MODE_NORMAL);
    }
    [self draw];
}

-(IBAction)sliderAction:(id)sender {
    if (sender == sliderView) {
        float value = [sliderView floatValue];
        mat4_identity(device.transform.view);
        float eye[] = {0, 0, value}, center[] = {0, 0, 0}, up[] = {0, 1, 0};
        mat4_look_at(device.transform.view, eye, center, up);
        transform_update(&device.transform);
    }
    
    [self draw];
}

@end
