#pragma once
#include <stdint.h>
#include "values.h"

void load_image(Image*, ALLEGRO_USTR*);
void save_image_to_bmp(uint8_t*, uint8_t*, uint32_t, uint32_t, uint32_t);
void clean_up_image(Image*);
void get_pixel_color(Color_t*, uint32_t, uint32_t, Image*);
void swap_color(Image*, uint32_t, uint32_t);
