#ifndef __74HC595_H
#define __74HC595_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

void bin_disp_write(uint8_t a, uint8_t b, uint8_t c);
void bin_disp_init(void);

#ifdef __cplusplus
}
#endif

#endif 
