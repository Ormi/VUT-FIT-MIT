/**
 * filter.h
 * HSC proj
 * xormos00 12.12.2018
 */

#ifndef __FILTER__
#define __FILTER__

#include <ac_int.h>
#include "../../cpu/common.h"

#define MCU_SIZE        512

typedef ac_int<PIXEL_WIDTH,false> t_pixel;
typedef ac_int<32,false> t_mcu_data;
typedef ac_int<1,false> t_bool;
typedef ac_int<10,false> t_index;
typedef ac_int<17,false> t_histogram;
typedef ac_int<2,false> t_test;
typedef ac_int<7,false> t_frame;

void filter(t_pixel &in_data, bool &in_data_vld, t_pixel &out_data, t_mcu_data mcu_data[MCU_SIZE]);

#endif
