#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    int w, h, k;

    w = clamp_index(im.w, x);
    h = clamp_index(im.h, y);
    k = clamp_index(im.c, c);
    
    int index = w + im.w * h + im.h * im.w * k;
    
    return im.data[index];
}

int clamp_index(int bound, int value) {
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

    float * dst = copy.data;
    float * src = im.data;

    memcpy(dst, src, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);

    int w = im.w, h = im.h;
    int i, j;

    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            float gama;
            int index_r = j + w * i + 0 * w * h;
            int index_g = j + w * i + 1 * w * h;
            int index_b = j + w * i + 2 * w * h;
            
            gama = im.data[index_r] * 0.299 + im.data[index_g] * 0.587 + im.data[index_b] * 0.113;

            gray.data[j + w * i] = gama;
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    int w = im.w, h = im.h;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int index = j + w * i + c * w * h;
            im.data[index] += v;
        }
    }
}

void clamp_image(image im)
{
    int w = im.w, h = im.h, c = im.c;

    for (int k = 0; k < c; ++k) {
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                int index = j + i * w  + k * w * h;
                im.data[index] = clamp_value(im.data[index]);
            }
        }
    }
}

float clamp_value(float v) {
    if (v < 0.0) return 0.0;
    if (v > 1.0) return 1.0;
    return v;

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
    int w = im.w, h = im.h;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int index_r = j + w * i + 0 * w * h;
            int index_g = j + w * i + 1 * w * h;
            int index_b = j + w * i + 2 * w * h;

            float red = im.data[index_r], green = im.data[index_g], blue = im.data[index_b];

            float v = three_way_max(red, green, blue);
            float min = three_way_min(red, green, blue);
            
            float C = v - min;
            float s = (v == 0) ? 0.0 : C / v;
            
            float h_prime, h;

            if ( C == 0 ) {
                h = 0;

                im.data[index_r] = h;
                im.data[index_g] = s;
                im.data[index_b] = v;
                continue;
            }
            if ( v == red ) {
                h_prime = (green - blue) / C;
                h = (h_prime < 0) ? h_prime / 6 + 1 : h_prime / 6;
                
                im.data[index_r] = h;
                im.data[index_g] = s;
                im.data[index_b] = v;
                continue;
            }

            if ( v == green ) {
                h_prime = (blue - red) / C + 2;
                h = (h_prime < 0) ? h_prime / 6 + 1 : h_prime / 6;
                
                im.data[index_r] = h;
                im.data[index_g] = s;
                im.data[index_b] = v;
                continue;
            }

            if ( v == blue ) {
                h_prime = (red - green) / C + 4;
                h = (h_prime < 0) ? h_prime / 6 + 1 : h_prime / 6;
                
                im.data[index_r] = h;
                im.data[index_g] = s;
                im.data[index_b] = v;
                continue;
            }

        }
    }
}

void hsv_to_rgb(image im)
{
    int w = im.w, h = im.h;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int index_h = j + w * i + 0 * w * h;
            int index_s = j + w * i + 1 * w * h;
            int index_v = j + w * i + 2 * w * h;

            int index_r = index_h;
            int index_g = index_s;
            int index_b = index_v;

            float hue = im.data[index_h], saturation = im.data[index_s], value = im.data[index_v];
            float C = value * saturation;
            float red, green, blue;

            float h_prime = hue * 6;

            float X = C * (1 - abs(fmod(h_prime, 2) - 1));

            float m = value - C;

            if (h_prime >= 0  && h_prime < 1) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            } else if (h_prime >= 1  && h_prime < 2) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            } else if (h_prime >= 2 && h_prime < 3) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            } else if (h_prime >= 3  && h_prime < 4) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            } else if (h_prime >= 4 && h_prime < 5) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            } else if (h_prime >= 5 && h_prime < 6) {
                red = C + m;
                green = X + m;
                blue = 0 + m;
                im.data[index_r] = red;
                im.data[index_g] = green;
                im.data[index_b] = blue;
            }

        
        }
    }
}
