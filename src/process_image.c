#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    int w, h, k;

    w = clamp(im.w, x);
    h = clamp(im.h, y);
    k = clamp(im.c, c);
    
    int index = w + im.w * h + im.h * im.w * k;
    
    return im.data[index];
}

int clamp(int bound, int value) {
    if (value < 0) return 0;
    if (value >= bound) return bound - 1;
    return value;
}

int check_bound(int bound, int value) {
    if (value < 0) return -1;
    if (value >= bound) return -1;
    return 0;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int w = x, h = y, k = c;

    if (check_bound(im.w, x) == -1){
        return;
    }
    if (check_bound(im.h, y) == -1){
        return;
    }
    if (check_bound(im.c, c) == -1){
        return;
    }
    
    int index = w + im.w * h + im.h * im.w * k;

    im.data[index] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
}

void clamp_image(image im)
{
    // TODO Fill this in
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
}
