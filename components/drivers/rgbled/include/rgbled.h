//#include "drive/gpio.h"
#include "debug_cf.h" // DEBUG_PRINT_LOCAL

#ifndef RGBLED_H
#define RGBLED_H

void rgbled_init();

void process_rgb_command(uint8_t *,uint8_t);

void set_rgbled(_Bool,_Bool,_Bool);


#endif
