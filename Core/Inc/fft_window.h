//
// Created by 20614 on 25-4-19.
//

#ifndef FFT_WINDOW_H
#define FFT_WINDOW_H

#include <stdio.h>
#include "arm_math.h"

#define PI 3.14159265358979f  //32bit PI

void window_calculate(float32_t *input,uint32_t fft_length,int type);

//仅在内部使用
static void hamming_window(float32_t* pDst, uint32_t blockSize) ;
static void hanning_window(float32_t* pDst, uint32_t blockSize) ;
static void blackman_window(float32_t* pDst, uint32_t blockSize);
#endif //FFT_WINDOW_H
