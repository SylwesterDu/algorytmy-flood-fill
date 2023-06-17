#pragma once
#include "values.h"
#include <stdint.h>

void flood_fill(algorithm_t, uint32_t, uint32_t, Image*, Color_t);
void stack_based_recursive_four_way(uint32_t, uint32_t, Image*, Color_t);
void stack_based_recursive_eight_way(uint32_t, uint32_t, Image*, Color_t);
void queue_based_four_way(uint32_t, uint32_t, Image*, Color_t);
void scanline_recursive(uint32_t, uint32_t, uint32_t, Image*, Color_t);

void stack_based_recursive_four_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void stack_based_recursive_eight_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void queue_based_four_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void scanline_recursive_visualize(uint32_t, uint32_t, uint32_t, Image*, Color_t);