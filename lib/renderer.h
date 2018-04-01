//
//  renderer.h
//  SoftwareRender
//
//  Created by xiaov.
//

#ifndef renderer_h
#define renderer_h

#include <stdio.h>
#include <stdint.h>

//===================================================================
//math
//===================================================================

// |v|
float vec4_len(const float* v);

// normalize
void vec4_normalize(float* v);

// out = a + b
void vec4_add(float* out, const float* a, const float* b);

// out = a - b
void vec4_sub(float* out, const float* a, const float* b);

// a . b
float vec4_dot(const float* a, const float* b);

// a x b
void vec4_cross(float* out, const float* a, const float* b);

// a x b
void vec4_multiply(float* out, const float* a, float b);

// t -> [0, 1]
void vec4_interp(float* out, const float* a, const float* b, float t);

// identity
void mat4_identity(float* m);

// invert
void mat4_invert(float * out, const float* m);

// multiply
void mat4_multiply(float* out, const float* a, const float* b);

// translate
void mat4_translate(float* out, const float* m, const float* v);

// scale
void mat4_scale(float* out, const float* m, const float* v);

// rotate
void mat4_rotate(float* out, const float* m, float rad, const float* v);

// apply
void mat4_apply(float* out, const float* m, const float* v);

// perspective
void mat4_perspective(float* out, float fovy, float aspect, float near, float far);

// look at
void mat4_look_at(float* out, const float* eye, const float* center, const float* up);

//===================================================================
//transform
//===================================================================
typedef struct {
    float model[16];
    float inv_model[16];
    float view[16];
    float projection[16];
    float transform[16];// mvp
    float w, h;
} transform_t;

void transform_init(transform_t* transform, int width, int height);
void transform_update(transform_t* transform);
void transform_apply(const transform_t* transform, float* v);

//===================================================================
//device
//===================================================================

typedef struct {
    float position[4];
    float normal[4];
    float color[4];
    float texcoord[2];
    float oneoverz;
} vertex_t;

typedef struct {
    int type;
    int width;
    int height;
    uint32_t* data;
} texture_t;

typedef struct {
    float color[3];
    float postion[3];
    float ka;
    float kd;
    float ks;
    uint16_t shininess;
} light_t;

#define DEVICE_DRAW_MODE_NORMAL 1
#define DEVICE_DRAW_MODE_WILD 2

typedef struct {
    transform_t transform;
    uint32_t width;
    uint32_t height;
    uint32_t* framebuffer;
    float* zbuffer;
    
    float* vertex_pointer;
    int vertex_count;
    float* normal_pointer;
    float* texcoord_pointer;
    float* color_pointer;
    
    int lighting;
    light_t lights[1];
    
    int draw_mode;
    
    texture_t* texture;
    
} device_t;

void device_init(device_t * device, uint32_t width, uint32_t height);
void device_destroy(device_t *device);
void device_clear(device_t *device);

void device_vertex_pointer(device_t *device, int count, float* pointer);
void device_normal_pointer(device_t *device, float* pointer);
void device_color_pointer(device_t *device, float* pointer);
void device_texcoord_pointer(device_t *device, float* pointer);

void device_draw_mode(device_t *device, int mode);

void device_enable_light(device_t *device, int light);
void device_disable_light(device_t *device, int light);

void device_light(device_t *device, float* postion, float* color, float ka, float kd, float ks, uint16_t shininess);

// rgba only
texture_t* device_gen_texture(int type, int width, int height, uint8_t* data);
void device_del_texture(texture_t* tex);
void device_update_texture(texture_t* tex, int x, int y, int w, int h);
void device_bind_texture(device_t *device, texture_t* tex);

void vertex_interp(vertex_t* out, const vertex_t* v1, const vertex_t* v2, float t);

void draw_pixel(device_t* device, int x, int y, uint32_t color);
void draw_line(device_t* device, int x1, int y1, int x2, int y2, uint32_t color);
void draw_triangle(device_t* device, vertex_t* v1, vertex_t* v2, vertex_t* v3);

// count: number of verxtexes
void draw_arrays(device_t* device, int offset, int count);
void draw_elements(device_t* device, int* indices, int count);





#endif /* renderer_h */
