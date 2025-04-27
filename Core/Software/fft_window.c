//
// Created by 20614 on 25-4-19.
//

#include "fft_window.h"
#include <stdlib.h>
#include "arm_math.h"
#include "malloc.h"
/*
 * @brief 输入ADC采样完毕的数组组，模拟量以及数组长度
 * type0：汉明窗
 * type1：汉宁窗
 * type2：布莱克曼窗
 * 具体窗的旁瓣抑制效果以及主瓣宽度请到.C文件查看
 */
void window_calculate(float32_t *input,uint32_t fft_length,int type)
{
    float32_t *window = (float32_t*)malloc(fft_length * sizeof(float32_t));
    //计算窗函数，用type来选择不同的窗效果
    switch(type)
    {
    case 0:
        hamming_window(window,fft_length);
        break;
    case 1:
        hanning_window(window,fft_length);
        break;
    case 2:
        blackman_window(window,fft_length);
        break;
    }
    //逐点相乘，求窗
    for(int i=0;i<fft_length;i++)
    {
        input[i] = input[i] * window[i];
    }
}
/*
  @brief         Hamming window generating function (f32).
  @param[out]    pDst       points to the output generated window
  @param[in]     blockSize  number of samples in the window

  @par Parameters of the window

  | Parameter                             | Value              |
  | ------------------------------------: | -----------------: |
  | Peak sidelobe level                   |           42.7 dB  |
  | Normalized equivalent noise bandwidth |       1.3628 bins  |
  | 3 dB bandwidth                        |       1.3008 bins  |
  | Flatness                              |        -1.7514 dB  |
  | Recommended overlap                   |              50 %  |
 */
static void hamming_window(float32_t* pDst, uint32_t blockSize)
{
    float step = 2.0f /((float32_t)blockSize);
    float32_t window;
    for (uint32_t i = 0; i < blockSize; i++)
    {
        window =0.54f - 0.46f * cosf(PI * i * step);
        pDst[i] = window;
    }
}
/*
  @brief         Hanning window generating function (f32).
  @param[out]    pDst       points to the output generated window
  @param[in]     blockSize  number of samples in the window

  @par Parameters of the window

  | Parameter                             | Value              |
  | ------------------------------------: | -----------------: |
  | Peak sidelobe level                   |           31.5 dB  |
  | Normalized equivalent noise bandwidth |          1.5 bins  |
  | 3 dB bandwidth                        |       1.4382 bins  |
  | Flatness                              |        -1.4236 dB  |
  | Recommended overlap                   |              50 %  |
 */
static void hanning_window(float32_t* pDst, uint32_t blockSize)
{
    float step = 2.0f /((float32_t)blockSize);
    float32_t window;
    for (uint32_t i = 0; i < blockSize; i++)
    {
        window = PI * i * step;
        window = 0.5f *(1.0f - cosf(window));
        pDst[i] = window;
    }
}

/*
  @brief         92 dB Blackman Harris window generating function (f32).
  @param[out]    pDst       points to the output generated window
  @param[in]     blockSize  number of samples in the window

  @par Parameters of the window

  | Parameter                             | Value              |
  | ------------------------------------: | -----------------: |
  | Peak sidelobe level                   |           92.0 dB  |
  | Normalized equivalent noise bandwidth |       2.0044 bins  |
  | 3 dB bandwidth                        |       1.8962 bins  |
  | Flatness                              |        -0.8256 dB  |
  | Recommended overlap                   |            66.1 %  |

 */
static void blackman_window(float32_t* pDst, uint32_t blockSize)
{
    float step = 2.0f /((float32_t)blockSize);
    float32_t window;
    for (uint32_t i = 0; i < blockSize; i++)
    {
        window = PI * i * step;
        window = 0.35875f - 0.48829f * cosf (window) +
        0.14128f * cosf (2.f * window) - 0.01168f * cosf (3.f * window);
        pDst[i] = window;
    }
}


