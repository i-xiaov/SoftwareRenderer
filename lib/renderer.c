//
//  renderer.c
//  SoftwareRender
//
//  Created by xiaov.
//

#include "renderer.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    false, true
} bool;

#define EPSILON 0.000001
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define EQUAL(a, b) (ABS(a - b) < EPSILON)
#define CLAMP(x, min, max) ((x < min)? min : ((x > max)? max : x))
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define CEIL(x) ((int)(x))
#define ROUND(x) ((int)(x + 0.5))

// t -> [0, 1]
float interp(float x1, float x2, float t) {
    return x1 + (x2 - x1) * t;
}

// |v|
float vec4_len(const float* v) {
    float sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    return (float)sqrt(sq);
}

// normalize
void vec4_normalize(float* v) {
    float len = vec4_len(v);
    if (len != 0) {
        float inv = 1.0f/ len;
        v[0] *= inv;
        v[1] *= inv;
        v[2] *= inv;
    }
}

// out = a + b
void vec4_add(float* out, const float* a, const float* b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
    out[3] = a[3] + b[3];
}

// out = a - b
void vec4_sub(float* out, const float* a, const float* b) {
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
    out[3] = a[3] - b[3];
}

// a . b
float vec4_dot(const float* a, const float* b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// a x b
void vec4_cross(float* out, const float* a, const float* b) {
    float m1, m2, m3;
    m1 = a[1] * b[2] - a[2] * b[1];
    m2 = a[2] * b[0] - a[0] * b[2];
    m3 = a[0] * b[1] - a[1] * b[0];
    out[0] = m1;
    out[1]= m2;
    out[2] = m3;
}

// a x b
void vec4_multiply(float* out, const float* a, float b) {
    out[0] = a[0] * b;
    out[1] = a[1] * b;
    out[2] = a[2] * b;
}

// t -> [0, 1]
void vec4_interp(float* out, const float* a, const float* b, float t) {
    out[0] = interp(a[0], b[0], t);
    out[1] = interp(a[1], b[1], t);
    out[2] = interp(a[2], b[2], t);
}

// identity
void mat4_identity(float* m) {
    m[0] = 1;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 1;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 1;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

// invert
void mat4_invert(float * out, const float* a) {
    float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
    a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],
    a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
    a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15],
    
    b00 = a00 * a11 - a01 * a10,
    b01 = a00 * a12 - a02 * a10,
    b02 = a00 * a13 - a03 * a10,
    b03 = a01 * a12 - a02 * a11,
    b04 = a01 * a13 - a03 * a11,
    b05 = a02 * a13 - a03 * a12,
    b06 = a20 * a31 - a21 * a30,
    b07 = a20 * a32 - a22 * a30,
    b08 = a20 * a33 - a23 * a30,
    b09 = a21 * a32 - a22 * a31,
    b10 = a21 * a33 - a23 * a31,
    b11 = a22 * a33 - a23 * a32,
    
    // calculate the determinant
    det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    
    if (det == 0) {
        return;
    }
    det = 1.0 / det;
    
    out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
    out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
    out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
    out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
    out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
    out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
    out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
    out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
    out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
    out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
}

// multiply
void mat4_multiply(float* out, const float* a, const float* b) {
    float a00 = a[0],  a01 = a[1],  a02 = a[2],  a03 = a[3],
          a10 = a[4],  a11 = a[5],  a12 = a[6],  a13 = a[7],
          a20 = a[8],  a21 = a[9],  a22 = a[10], a23 = a[11],
          a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];
    
    float b0  = b[0], b1 = b[1], b2 = b[2], b3 = b[3];
    out[0] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out[1] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out[2] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out[3] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
    
    b0 = b[4]; b1 = b[5]; b2 = b[6]; b3 = b[7];
    out[4] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out[5] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out[6] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out[7] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
    
    b0 = b[8]; b1 = b[9]; b2 = b[10]; b3 = b[11];
    out[8] =  b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out[9] =  b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out[10] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out[11] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
    
    b0 = b[12]; b1 = b[13]; b2 = b[14]; b3 = b[15];
    out[12] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
    out[13] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
    out[14] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
    out[15] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
}

// translate
void mat4_translate(float* out, const float* a, const float* v) {
    float x = v[0], y = v[1], z = v[2],
    a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23;
    
    if (a == out) {
        out[12] = a[0] * x + a[4] * y + a[8]  * z + a[12];
        out[13] = a[1] * x + a[5] * y + a[9]  * z + a[13];
        out[14] = a[2] * x + a[6] * y + a[10] * z + a[14];
        out[15] = a[3] * x + a[7] * y + a[11] * z + a[15];
    } else {
        a00 = a[0]; a01 = a[1]; a02 = a[2];  a03 = a[3];
        a10 = a[4]; a11 = a[5]; a12 = a[6];  a13 = a[7];
        a20 = a[8]; a21 = a[9]; a22 = a[10]; a23 = a[11];
        
        out[0] = a00; out[1] = a01; out[2]  = a02;  out[3] = a03;
        out[4] = a10; out[5] = a11; out[6]  = a12;  out[7] = a13;
        out[8] = a20; out[9] = a21; out[10] = a22; out[11] = a23;
        
        out[12] = a00 * x + a10 * y + a20 * z + a[12];
        out[13] = a01 * x + a11 * y + a21 * z + a[13];
        out[14] = a02 * x + a12 * y + a22 * z + a[14];
        out[15] = a03 * x + a13 * y + a23 * z + a[15];
    }
}

// scale
void mat4_scale(float* out, const float* a, const float* v) {
    float x = v[0], y = v[1], z = v[2];
    
    out[0] = a[0] * x;
    out[1] = a[1] * x;
    out[2] = a[2] * x;
    out[3] = a[3] * x;
    out[4] = a[4] * y;
    out[5] = a[5] * y;
    out[6] = a[6] * y;
    out[7] = a[7] * y;
    out[8] = a[8] * z;
    out[9] = a[9] * z;
    out[10] = a[10] * z;
    out[11] = a[11] * z;
    out[12] = a[12];
    out[13] = a[13];
    out[14] = a[14];
    out[15] = a[15];
}

// rotate
void mat4_rotate(float* out, const float* a, float rad, const float* axis) {
    float x = axis[0], y = axis[1], z = axis[2],
    len = (float)sqrt(x * x + y * y + z * z),
    s, c, t,
    a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23,
    b00, b01, b02,
    b10, b11, b12,
    b20, b21, b22;
    
    if (ABS(len) < EPSILON) { return;}
    
    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;
    
    s = sin(rad);
    c = cos(rad);
    t = 1 - c;
    
    a00 = a[0]; a01 = a[1]; a02 = a[2]; a03 = a[3];
    a10 = a[4]; a11 = a[5]; a12 = a[6]; a13 = a[7];
    a20 = a[8]; a21 = a[9]; a22 = a[10]; a23 = a[11];
    
    b00 = x * x * t + c;     b01 = y * x * t + z * s; b02 = z * x * t - y * s;
    b10 = x * y * t - z * s; b11 = y * y * t + c;     b12 = z * y * t + x * s;
    b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;
    
    out[0] = a00 * b00 + a10 * b01 + a20 * b02;
    out[1] = a01 * b00 + a11 * b01 + a21 * b02;
    out[2] = a02 * b00 + a12 * b01 + a22 * b02;
    out[3] = a03 * b00 + a13 * b01 + a23 * b02;
    out[4] = a00 * b10 + a10 * b11 + a20 * b12;
    out[5] = a01 * b10 + a11 * b11 + a21 * b12;
    out[6] = a02 * b10 + a12 * b11 + a22 * b12;
    out[7] = a03 * b10 + a13 * b11 + a23 * b12;
    out[8] = a00 * b20 + a10 * b21 + a20 * b22;
    out[9] = a01 * b20 + a11 * b21 + a21 * b22;
    out[10] = a02 * b20 + a12 * b21 + a22 * b22;
    out[11] = a03 * b20 + a13 * b21 + a23 * b22;
    
    if (a != out) {
        out[12] = a[12];
        out[13] = a[13];
        out[14] = a[14];
        out[15] = a[15];
    }
}

// apply
void mat4_apply(float* out, const float* m, const float* v) {
    float x = v[0], y = v[1], z = v[2], w = v[3];
    out[0] = x * m[0] + y * m[4] + z * m[8]  + w * m[12];
    out[1] = x * m[1] + y * m[5] + z * m[9]  + w * m[13];
    out[2] = x * m[2] + y * m[6] + z * m[10] + w * m[14];
    out[3] = x * m[3] + y * m[7] + z * m[11] + w * m[15];
}

// perspective
void mat4_perspective(float* out, float fovy, float aspect, float near, float far) {
    float f = 1.0 / tan(fovy / 2),
    nf = 1 / (near - far);
    out[0] = f / aspect;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = f;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = (far + near) * nf;
    out[11] = -1;
    out[12] = 0;
    out[13] = 0;
    out[14] = (2 * far * near) * nf;
    out[15] = 0;
}

// look at
void mat4_look_at(float* out, const float* eye, const float* center, const float* up) {
    float x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
    eyex = eye[0],
    eyey = eye[1],
    eyez = eye[2],
    upx = up[0],
    upy = up[1],
    upz = up[2],
    centerx = center[0],
    centery = center[1],
    centerz = center[2];
    
    if (EQUAL(eyex, centerx) && EQUAL(eyey, centery) && EQUAL(eyez, centerz)) {
        mat4_identity(out);
        return;
    }
    
    z0 = eyex - centerx;
    z1 = eyey - centery;
    z2 = eyez - centerz;
    
    len = 1 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
    z0 *= len;
    z1 *= len;
    z2 *= len;
    
    x0 = upy * z2 - upz * z1;
    x1 = upz * z0 - upx * z2;
    x2 = upx * z1 - upy * z0;
    len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
    if (len < EPSILON) {
        x0 = 0;
        x1 = 0;
        x2 = 0;
    } else {
        len = 1 / len;
        x0 *= len;
        x1 *= len;
        x2 *= len;
    }
    
    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;
    
    len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
    if (len < EPSILON) {
        y0 = 0;
        y1 = 0;
        y2 = 0;
    } else {
        len = 1 / len;
        y0 *= len;
        y1 *= len;
        y2 *= len;
    }
    
    out[0] = x0;
    out[1] = y0;
    out[2] = z0;
    out[3] = 0;
    out[4] = x1;
    out[5] = y1;
    out[6] = z1;
    out[7] = 0;
    out[8] = x2;
    out[9] = y2;
    out[10] = z2;
    out[11] = 0;
    out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
    out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
    out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
    out[15] = 1;
}

void transform_init(transform_t* transform, int width, int height) {
    transform->w = (float)width;
    transform->h = (float)height;
    mat4_identity(transform->model);
    mat4_identity(transform->view);
    mat4_identity(transform->projection);
    mat4_identity(transform->transform);
    
    float eye[] = {0, 0, 3}, center[] = {0, 0, 0}, up[] = {0, 1, 0};
    mat4_look_at(transform->view, eye, center, up);
    
    float aspect = (float)width / ((float)height);
    mat4_perspective(transform->projection, 3.14159265f / 4, aspect, 1.0f, 100.0f);
    
    transform_update(transform);
}

void transform_update(transform_t* transform) {
    mat4_multiply(transform->transform, transform->projection, transform->view);
    mat4_multiply(transform->transform, transform->transform, transform->model);
    
    mat4_invert(transform->inv_model, transform->model);
}

void transform_apply(const transform_t* transform, float* v) {
    mat4_apply(v, transform->transform, v);
}

void device_init(device_t * device, uint32_t width, uint32_t height) {
    device->width = width;
    device->height = height;
    device->framebuffer = (uint32_t*)malloc(width * height * 4);
    device->zbuffer = (float*)malloc(width * height * sizeof(float));
    
    device->vertex_pointer = NULL;
    device->normal_pointer = NULL;
    device->texcoord_pointer = NULL;
    device->color_pointer = NULL;
    
    device->lighting = 0;
    memset(device->lights, 0, sizeof(light_t));
    
    device->draw_mode = DEVICE_DRAW_MODE_NORMAL;
    
    device->texture = NULL;
    
    transform_init(&device->transform, width, height);
}

void device_destroy(device_t *device) {
    if (device->framebuffer) free(device->framebuffer);
    if (device->zbuffer) free(device->zbuffer);
}

void device_clear(device_t *device) {
//    memset(device->framebuffer, 0, device->width * device->height * sizeof(uint32_t));
//    memset(device->zbuffer, 0, device->width * device->height * sizeof(float));
    int index;
    for (int i = 0; i < device->height; i++)
        for (int j = 0; j < device->width; j++) {
            index = i * device->width + j;
            device->framebuffer[index] = 0xff000000;
            device->zbuffer[index] = 0;
        }
}

void device_vertex_pointer(device_t *device, int count, float* pointer) {
    device->vertex_pointer = pointer;
    device->vertex_count = count;
}

void device_normal_pointer(device_t *device, float* pointer) {
    device->normal_pointer = pointer;
}

void device_color_pointer(device_t *device, float* pointer) {
    device->color_pointer = pointer;
}

void device_texcoord_pointer(device_t *device, float* pointer) {
    device->texcoord_pointer = pointer;
}

void device_draw_mode(device_t *device, int mode) {
    device->draw_mode = mode;
}

void device_enable_light(device_t *device, int light) {
    device->lighting |= light;
}

void device_disable_light(device_t *device, int light) {
    device->lighting &= ~light;
}

void device_light(device_t *device, float* postion, float* color, float ka, float kd, float ks, uint16_t shininess) {
    device->lights[0].postion[0] = postion[0];
    device->lights[0].postion[1] = postion[1];
    device->lights[0].postion[2] = postion[2];
    
    device->lights[0].color[0] = color[0];
    device->lights[0].color[1] = color[1];
    device->lights[0].color[2] = color[2];
    
    device->lights[0].ka = ka;
    device->lights[0].kd = kd;
    device->lights[0].ks = ks;
    device->lights[0].shininess = shininess;
}

texture_t* device_gen_texture(int type, int width, int height, uint8_t* data) {
    texture_t* tex = (texture_t*)malloc(sizeof(texture_t));
    tex->type = type;
    tex->width = width;
    tex->height = height;
    
    int row_size = width * 4, index1 = 0, index2 = (height - 1) * row_size;
    uint8_t* p = (uint8_t*)malloc(row_size * height);
    for (int i = 0; i < height; i++, index1 += row_size, index2 -= row_size) {
        memcpy(p + index1, data + index2, row_size);
    }
    
    tex->data = (uint32_t*)p;
    
    return tex;
}

void device_del_texture(texture_t* tex) {
    free(tex->data);
    free(tex);
}

void device_update_texture(texture_t* tex, int x, int y, int w, int h) {
    // TODO
}

void device_bind_texture(device_t *device, texture_t* tex) {
    device->texture = tex;
}

void draw_pixel(device_t* device, int x, int y, uint32_t color) {
    if (x >= 0 && x < device->width && y >= 0 && y < device->height) {
        device->framebuffer[y * device->width + x] = color;
    }
}

static uint8_t float_to_byte(float f) {
    int r = ROUND(f * 255);
    r = CLAMP(r, 0, 255);
    return (uint8_t)(r & 0xff);
}

static uint32_t rgba_float_to_uint(float r, float g, float b, float a) {
    uint32_t br = float_to_byte(r);
    uint32_t bg = float_to_byte(g);
    uint32_t bb = float_to_byte(b);
    uint32_t ba = float_to_byte(a);
    
    if (ba < 255) ba = 255;
    
    return br + (bg << 8) + (bb << 16) + (ba << 24);
}

void vertex_interp(vertex_t* out, const vertex_t* v1, const vertex_t* v2, float t) {
    vec4_interp(out->position, v1->position, v2->position, t);
    vec4_interp(out->normal, v1->normal, v2->normal, t);
    out->color[0] = interp(v1->color[0], v2->color[0], t);
    out->color[1] = interp(v1->color[1], v2->color[1], t);
    out->color[2] = interp(v1->color[2], v2->color[2], t);
    out->color[3] = interp(v1->color[3], v2->color[3], t);
    out->texcoord[0] = interp(v1->texcoord[0], v2->texcoord[0], t);
    out->texcoord[1] = interp(v1->texcoord[1], v2->texcoord[1], t);
    out->oneoverz = interp(v1->oneoverz, v2->oneoverz, t);
}

void draw_line(device_t* device, int x1, int y1, int x2, int y2, uint32_t color) {
    int x, y;
    if (x1 == x2 && y1 == y2) {
        draw_pixel(device, x1, y1, color);
    }
    else if (x1 == x2) {
        int inc = y1 > y2 ? -1 : 1;
        for (y = y1; y != y2; y += inc) {
            draw_pixel(device, x1, y, color);
        }
    }
    else if (y1 == y2) {
        int inc = x1 > x2 ? -1 : 1;
        for (x = x1; x != x2; x += inc) {
            draw_pixel(device, x, y1, color);
        }
    }
    else {
        int rem = 0, inc;
        int dx = x1 > x2 ? x1 - x2 : x2 - x1;
        int dy = y1 > y2 ? y1 - y2 : y2 - y1;
        if (dx > dy) {
            if (x2 < x1) {
                x = x1;
                y = y1;
                x1 = x2;
                y1 = y2;
                x2 = x;
                y2 = y;
            }
            inc = y1 > y2 ? -1 : 1;
            for (x = x1, y = y1; x <= x2; x++) {
                draw_pixel(device, x, y, color);
                
                rem += dy;
                if (rem >= dx) {
                    rem -= dx;
                    y += inc;
                    draw_pixel(device, x, y, color);
                }
            }
            draw_pixel(device, x2, y2, color);
        }
        else {
            if (y2 < y1) {
                x = x1;
                y = y1;
                x1 = x2;
                y1 = y2;
                x2 = x;
                y2 = y;
            }
            inc = x1 > x2 ? -1 : 1;
            for (x = x1, y = y1; y <= y2; y++) {
                draw_pixel(device, x, y, color);
                
                rem += dx;
                if (rem >= dy) {
                    rem -= dy;
                    x += inc;
                    draw_pixel(device, x, y, color);
                }
            }
            
            draw_pixel(device, x2, y2, color);
        }
    }
}

static void perspective_division(float *v) {
    if (v[3] == 0) return;
    float inv = 1 / v[3];
    v[0] *= inv;
    v[1] *= inv;
    v[2] *= inv;
    v[3] = 1;
}

static void cvv_to_view_port(float *v, int w, int h) {
    v[0] = (int)(w * (v[0] + 1) * 0.5f);
    v[1] = (int)(w * (v[1] + 1) * 0.5f);
}

static void sort_vertices_by_y(vertex_t* v1, vertex_t* v2, vertex_t* v3,
                               vertex_t** top, vertex_t** middle, vertex_t** bottom) {
    if (v1->position[1] > v2->position[1]) {
        if (v1->position[1] > v3->position[1]) {
            *top = v1;
            *middle = v2;
            *bottom = v3;
        }
        else {
            *top = v3;
            *middle = v1;
            *bottom = v2;
        }
    }
    else {
        if (v2->position[1] > v3->position[1]) {
            *top = v2;
            *middle = v1;
            *bottom = v3;
        }
        else {
            *top = v3;
            *middle = v1;
            *bottom = v2;
        }
    }
    
    if ((*middle)->position[1] < (*bottom)->position[1]) {
        vertex_t* t = *middle;
        *middle = *bottom;
        *bottom = t;
    }
}

static void dump_vertex(vertex_t* v) {
    printf("(%f, %f, %f)", v->position[0], v->position[1], v->position[2]);
}

static void vertex_pre_process(vertex_t* v) {
    float oneoverz = 1 / v->position[3];
    v->oneoverz = oneoverz;
    v->texcoord[0] *= oneoverz;
    v->texcoord[1] *= oneoverz;
    v->color[0] *= oneoverz;
    v->color[1] *= oneoverz;
    v->color[2] *= oneoverz;
    v->color[3] *= oneoverz;
}

static void vertex_division(vertex_t *out, const vertex_t *v1, const vertex_t *v2, float w) {
    float inv = 1.0f / w;
    out->position[0] = (v2->position[0] - v1->position[0]) * inv;
    out->position[1] = (v2->position[1] - v1->position[1]) * inv;
    out->position[2] = (v2->position[2] - v1->position[2]) * inv;
    out->position[3] = (v2->position[3] - v1->position[3]) * inv;
    out->normal[0] = (v2->normal[0] - v1->normal[0]) * inv;
    out->normal[1] = (v2->normal[1] - v1->normal[1]) * inv;
    out->normal[2] = (v2->normal[2] - v1->normal[2]) * inv;
    out->normal[3] = (v2->normal[3] - v1->normal[3]) * inv;
    out->texcoord[0] = (v2->texcoord[0] - v1->texcoord[0]) * inv;
    out->texcoord[1] = (v2->texcoord[1] - v1->texcoord[1]) * inv;
    out->color[0] = (v2->color[0] - v1->color[0]) * inv;
    out->color[1] = (v2->color[1] - v1->color[1]) * inv;
    out->color[2] = (v2->color[2] - v1->color[2]) * inv;
    out->color[3] = (v2->color[3] - v1->color[3]) * inv;
    out->oneoverz = (v2->oneoverz - v1->oneoverz) * inv;
}

static void vertex_add(vertex_t *out, const vertex_t *v) {
    out->position[0] += v->position[0];
    out->position[1] += v->position[1];
    out->position[2] += v->position[2];
    out->position[3] += v->position[3];
    out->normal[0] += v->normal[0];
    out->normal[1] += v->normal[1];
    out->normal[2] += v->normal[2];
    out->normal[3] += v->normal[3];
    out->texcoord[0] += v->texcoord[0];
    out->texcoord[1] += v->texcoord[1];
    out->color[0] += v->color[0];
    out->color[1] += v->color[1];
    out->color[2] += v->color[2];
    out->color[3] += v->color[3];
    out->oneoverz += v->oneoverz;
}

static int check_cvv(const float* v) {
    float w = v[3];
    int check = 0;
    
    if (v[0] < -w) check |= 1;
    if (v[0] >  w) check |= 2;
    if (v[1] < -w) check |= 4;
    if (v[1] >  w) check |= 8;
    if (v[2] < 0) check |= 16;
    if (v[2] >  w) check |= 32;
    
    return check;
}

typedef struct {
    vertex_t v, step;
    int x, y, w;
} scanline_t;

static void scanline_init(scanline_t* scanline, vertex_t* left, vertex_t* right, int y) {
    scanline->x = (int)(left->position[0] + 0.5);
    scanline->y = y;
    scanline->w = (int)(right->position[0] + 0.5) - scanline->x;
    scanline->v = *left;
    
    float width = right->position[0] - left->position[0];
    vertex_division(&scanline->step, left, right, width);
}

typedef struct {
    uint8_t r, g, b, a;
} color_t;

static color_t device_texture_read(device_t* device, float u, float v) {
    texture_t* tex = device->texture;
    u = u * (tex->width - 1);
    v = v * (tex->height - 1);
    int x = CLAMP((int)(u + 0.5f), 0, tex->width - 1);
    int y = CLAMP((int)(v + 0.5f), 0, tex->height - 1);
    uint32_t c = tex->data[y * tex->width + x];
    color_t color = *((color_t*)(&c));
    return color;
}

static void process_lighting(device_t* device, float* normal, float* color) {
    light_t * lt = &(device->lights[0]);
    float light[4] = {lt->postion[0], lt->postion[1], lt->postion[2], 0};
    mat4_apply(light, device->transform.inv_model, light);
    vec4_normalize(light);
    
    float eye[4] = {0, 0, 3, 0};
    mat4_apply(eye, device->transform.inv_model, eye);
    vec4_normalize(eye);
    
    float halfLE[4];
    vec4_add(halfLE, light, eye);
    vec4_normalize(halfLE);
    
    vec4_normalize(normal);
    
    float diffuse = vec4_dot(normal, light);
    diffuse = lt->kd * MAX(diffuse, 0);
    
    float specular = vec4_dot(normal, halfLE);
    specular = lt->ks * powf(MAX(specular, 0), lt->shininess);
    
    color[0] *= ((lt->ka + diffuse + specular) * lt->color[0]);
    color[1] *= ((lt->ka + diffuse + specular) * lt->color[1]);
    color[2] *= ((lt->ka + diffuse + specular) * lt->color[2]);
}

static void draw_scanline(device_t* device, scanline_t* scanline) {
    int left = scanline->x;
    int right = left + scanline->w;
    float z;
    float color[4], normal[4];
    int index = scanline->y * device->width + left;
    vertex_t* v = &scanline->v;
    for (; left < right; left++, index++) {
        if (left >= 0 && left < device->width && device->zbuffer[index] <= v->oneoverz) {
            z = 1 / v->oneoverz;
            color[0] = v->color[0] * z, color[1] = v->color[1] * z, color[2] = v->color[2] * z, color[3] = v->color[3] * z;
            normal[0] = v->normal[0] * z, normal[1] = v->normal[1] * z, normal[2] = v->normal[2] * z, normal[3] = 0;
            
            if (device->lighting) {
                process_lighting(device, normal, color);
            }
            
            uint32_t rgba;
            if (device->texture) {
                color_t c = device_texture_read(device, v->texcoord[0] * z, v->texcoord[1] * z);
                if (device->lighting) {
                    int t;
                    t = ROUND(c.r * color[0]);
                    c.r = CLAMP(t, 0, 255);
                    t = ROUND(c.g * color[1]);
                    c.g = CLAMP(t, 0, 255);
                    t = ROUND(c.b * color[2]);
                    c.b = CLAMP(t, 0, 255);
                    t = ROUND(c.a * color[3]);
                    c.a = CLAMP(t, 0, 255);
                }
                rgba = *(uint32_t*)(&c);
            }
            else {
                rgba = rgba_float_to_uint(color[0], color[1], color[2], color[3]);
            }
            
            device->framebuffer[index] = rgba;
            device->zbuffer[index] = v->oneoverz;
        }
        
        vertex_add(v, &scanline->step);
    }
}

static void fill_bottom_flat_triangle(device_t* device, vertex_t* v1, vertex_t* v2, vertex_t* v3) {
    int t = MIN(CEIL(v1->position[1]), device->height);
    int b = MAX(CEIL(v3->position[1]), 0);
    
    scanline_t scanline;
    vertex_t vl, vr;
    float height = v1->position[1] - v2->position[1];
    
    for (int scanlineY = t; scanlineY >= b; scanlineY--) {
        float t = (v1->position[1] - scanlineY) / height;
        vertex_interp(&vl, v1, v2, t);
        vertex_interp(&vr, v1, v3, t);
        
        scanline_init(&scanline, vl.position[0] > vr.position[0] ? &vr : &vl, vl.position[0] > vr.position[0] ? &vl : &vr, scanlineY);
        
        draw_scanline(device, &scanline);
    }
}

static void fill_top_flat_triangle(device_t* device, vertex_t* v1, vertex_t* v2, vertex_t* v3) {
    int t = MIN(CEIL(v2->position[1]), device->height);
    int b = MAX(CEIL(v3->position[1]), 0);
    
    scanline_t scanline;
    vertex_t vl, vr;
    float height = v2->position[1] - v3->position[1];
    
    for (int scanlineY = b; scanlineY < t; scanlineY++) {
        float t = (scanlineY - v3->position[1]) / height;
        vertex_interp(&vl, v3, v1, t);
        vertex_interp(&vr, v3, v2, t);
        
        scanline_init(&scanline, vl.position[0] > vr.position[0] ? &vr : &vl, vl.position[0] > vr.position[0] ? &vl : &vr, scanlineY);
        
        draw_scanline(device, &scanline);
    }
}

void draw_triangle(device_t* device, vertex_t* v1, vertex_t* v2, vertex_t* v3) {
    transform_apply(&device->transform, v1->position);
    transform_apply(&device->transform, v2->position);
    transform_apply(&device->transform, v3->position);
    
    if (check_cvv(v1->position) && check_cvv(v2->position) && check_cvv(v3->position)) {
        return;
    }
    
    float normal[4];
    float vec12[4];
    float vec13[4];
    vec4_sub(vec12, v1->position, v2->position);
    vec4_sub(vec13, v1->position, v3->position);
    vec4_cross(normal, vec12, vec13);
    if (normal[2] < 0) {
        return;
    }
    
    vertex_pre_process(v1);
    vertex_pre_process(v2);
    vertex_pre_process(v3);
    
    perspective_division(v1->position);
    perspective_division(v2->position);
    perspective_division(v3->position);
    
    cvv_to_view_port(v1->position, device->width, device->height);
    cvv_to_view_port(v2->position, device->width, device->height);
    cvv_to_view_port(v3->position, device->width, device->height);
    
    if (device->draw_mode & DEVICE_DRAW_MODE_NORMAL) {
        vertex_t* top;
        vertex_t* middle;
        vertex_t* bottom;
        sort_vertices_by_y(v1, v2, v3, &top, &middle, &bottom);
        
        if (EQUAL(middle->position[1], bottom->position[1])) {
            fill_bottom_flat_triangle(device, top, middle, bottom);
        }
        else if (EQUAL(middle->position[1], top->position[1])) {
            fill_top_flat_triangle(device, top, middle, bottom);
        }
        else {
            vertex_t v4;
            vertex_interp(&v4, top, bottom, (top->position[1] - middle->position[1]) / (top->position[1] - bottom->position[1]));
            fill_bottom_flat_triangle(device, top, middle, &v4);
            fill_top_flat_triangle(device, middle, &v4, bottom);
        }
    }
    
    if (device->draw_mode & DEVICE_DRAW_MODE_WILD) {
        draw_line(device, CEIL(v1->position[0]), CEIL(v1->position[1]), CEIL(v2->position[0]), CEIL(v2->position[1]), 0xffffffff);
        draw_line(device, CEIL(v2->position[0]), CEIL(v2->position[1]), CEIL(v3->position[0]), CEIL(v3->position[1]), 0xffffffff);
        draw_line(device, CEIL(v3->position[0]), CEIL(v3->position[1]), CEIL(v1->position[0]), CEIL(v1->position[1]), 0xffffffff);
    }
}

void draw_arrays(device_t* device, int offset, int count) {
    float * vp = device->vertex_pointer;
    if (!vp || device->vertex_count < offset + count) return;
    
    float * np = device->normal_pointer;
    float * tp = device->texcoord_pointer;
    float * cp = device->color_pointer;
    
    vertex_t v1, v2, v3;
    
    float * const pos1 = v1.position;
    float * const pos2 = v2.position;
    float * const pos3 = v3.position;
    
    float * const normal1 = v1.normal;
    float * const normal2 = v2.normal;
    float * const normal3 = v3.normal;
    
    float * const color1 = v1.color;
    float * const color2 = v2.color;
    float * const color3 = v3.color;
    
    float * const texcoord1 = v1.texcoord;
    float * const texcoord2 = v2.texcoord;
    float * const texcoord3 = v3.texcoord;
    
    for (int i = 0; i < count;) {
        int index = i * 3;
        pos1[0] = vp[index + 0], pos1[1] = vp[index + 1], pos1[2] = vp[index + 2], pos1[3] = 1;
        pos2[0] = vp[index + 3], pos2[1] = vp[index + 4], pos2[2] = vp[index + 5], pos2[3] = 1;
        pos3[0] = vp[index + 6], pos3[1] = vp[index + 7], pos3[2] = vp[index + 8], pos3[3] = 1;
        
        normal1[0] = normal1[1] = normal1[2] = normal1[3] = 0;
        normal2[0] = normal2[1] = normal2[2] = normal2[3] = 0;
        normal3[0] = normal3[1] = normal3[2] = normal3[3] = 0;
        
        color1[0] = color1[1] = color1[2] = color1[3] = 1;
        color2[0] = color2[1] = color2[2] = color2[3] = 1;
        color3[0] = color3[1] = color3[2] = color3[3] = 1;
        
        texcoord1[0] = texcoord1[1] = 0;
        texcoord2[0] = texcoord2[1] = 0;
        texcoord3[0] = texcoord3[1] = 0;
        
        if (np) {
            index = i * 3;
            normal1[0] = np[index + 0], normal1[1] = np[index + 1], normal1[2] = np[index + 2], normal1[3] = 0;
            normal2[0] = np[index + 3], normal2[1] = np[index + 4], normal2[2] = np[index + 5], normal2[3] = 0;
            normal3[0] = np[index + 6], normal3[1] = np[index + 7], normal3[2] = np[index + 8], normal3[3] = 0;
        }
        
        if (cp) {
            index = i * 4;
            color1[0] = cp[index + 0], color1[1] = cp[index + 1], color1[2] = cp[index + 2], color1[3] = cp[index + 3];
            color2[0] = cp[index + 4], color2[1] = cp[index + 5], color2[2] = cp[index + 6], color2[3] = cp[index + 7];
            color3[0] = cp[index + 8], color3[1] = cp[index + 9], color3[2] = cp[index + 10], color3[3] = cp[index + 11];
        }
        
        if (tp) {
            index = i * 2;
            texcoord1[0] = tp[index + 0], texcoord1[1] = tp[index + 1];
            texcoord2[0] = tp[index + 2], texcoord2[1] = tp[index + 3];
            texcoord3[0] = tp[index + 4], texcoord3[1] = tp[index + 5];
        }
        
        draw_triangle(device, &v1, &v2, &v3);
        
        i += 3;
    }
}

void draw_elements(device_t* device, int* indices, int count) {
    float * vp = device->vertex_pointer;
    if (!vp || !indices) return;
    
    float * np = device->normal_pointer;
    float * tp = device->texcoord_pointer;
    float * cp = device->color_pointer;
    
    vertex_t v1, v2, v3;
    
    float * const pos1 = v1.position;
    float * const pos2 = v2.position;
    float * const pos3 = v3.position;
    
    float * const normal1 = v1.normal;
    float * const normal2 = v2.normal;
    float * const normal3 = v3.normal;
    
    float * const color1 = v1.color;
    float * const color2 = v2.color;
    float * const color3 = v3.color;
    
    float * const texcoord1 = v1.texcoord;
    float * const texcoord2 = v2.texcoord;
    float * const texcoord3 = v3.texcoord;
    
    int index1, index2, index3;
    for (int i = 0; i < count;) {
        index1 = indices[i * 3];
        index2 = indices[i * 3 + 1];
        index3 = indices[i * 3 + 2];
        pos1[0] = vp[index1 + 0], pos1[1] = vp[index1 + 1], pos1[2] = vp[index1 + 2], pos1[3] = 1;
        pos2[0] = vp[index2 + 0], pos2[1] = vp[index2 + 1], pos2[2] = vp[index2 + 2], pos2[3] = 1;
        pos3[0] = vp[index3 + 0], pos3[1] = vp[index3 + 1], pos3[2] = vp[index3 + 2], pos3[3] = 1;
        
        normal1[0] = normal1[1] = normal1[2] = normal1[3] = 0;
        normal2[0] = normal2[1] = normal2[2] = normal2[3] = 0;
        normal3[0] = normal3[1] = normal3[2] = normal3[3] = 0;
        
        color1[0] = color1[1] = color1[2] = color1[3] = 1;
        color2[0] = color2[1] = color2[2] = color2[3] = 1;
        color3[0] = color3[1] = color3[2] = color3[3] = 1;
        
        texcoord1[0] = texcoord1[1] = 0;
        texcoord2[0] = texcoord2[1] = 0;
        texcoord3[0] = texcoord3[1] = 0;
        
        if (np) {
            index1 = indices[i * 3];
            index2 = indices[i * 3 + 1];
            index3 = indices[i * 3 + 2];
            normal1[0] = np[index1 + 0], normal1[1] = np[index1 + 1], normal1[2] = np[index1 + 2], normal1[3] = 0;
            normal2[0] = np[index2 + 0], normal2[1] = np[index2 + 1], normal2[2] = np[index2 + 2], normal2[3] = 0;
            normal3[0] = np[index3 + 0], normal3[1] = np[index3 + 1], normal3[2] = np[index3 + 2], normal3[3] = 0;
        }
        
        if (cp) {
            index1 = indices[i * 4];
            index2 = indices[i * 4 + 1];
            index3 = indices[i * 4 + 2];
            color1[0] = cp[index1 + 0], color1[1] = cp[index1 + 1], color1[2] = cp[index1 + 2], color1[3] = cp[index1 + 3];
            color2[0] = cp[index2 + 0], color2[1] = cp[index2 + 1], color2[2] = cp[index2 + 2], color2[3] = cp[index2 + 3];
            color3[0] = cp[index3 + 0], color3[1] = cp[index3 + 1], color3[2] = cp[index3 + 2], color3[3] = cp[index3 + 3];
        }
        
        if (tp) {
            index1 = indices[i * 2];
            index2 = indices[i * 2 + 1];
            index3 = indices[i * 2 + 2];
            texcoord1[0] = tp[index1 + 0], texcoord1[1] = tp[index1 + 1];
            texcoord2[0] = tp[index2 + 0], texcoord2[1] = tp[index2 + 1];
            texcoord3[0] = tp[index3 + 0], texcoord3[1] = tp[index3 + 1];
        }
        
        draw_triangle(device, &v1, &v2, &v3);
        
        i += 3;
    }
}
