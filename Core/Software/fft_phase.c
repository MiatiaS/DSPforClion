//
// Created by 20614 on 25-4-19.
//
#include "myfft.h"
#include "fft_phase.h"
#include "stdio.h"
#include "arm_math.h"
#include "math.h"


//计算Phase arctan 返回角度值 输入对应的序号即可
static float Phase_atan(float32_t *inputSignal, uint32_t index)
{
    if (inputSignal[2 * index + 1] >= 0 && inputSignal[2 * index] >= 0)
        return 0 + atan(inputSignal[2 * index + 1] / inputSignal[2 * index]) / PI * 180;
    else if (inputSignal[2 * index + 1] >= 0 && inputSignal[2 * index] <= 0)
        return 180 + atan(inputSignal[2 * index + 1] / inputSignal[2 * index]) / PI * 180;
    else if (inputSignal[2 * index + 1] <= 0 && inputSignal[2 * index] <= 0)
        return 180 + atan(inputSignal[2 * index + 1] / inputSignal[2 * index]) / PI * 180;
    else if (inputSignal[2 * index + 1] <= 0 && inputSignal[2 * index] >= 0)
        return 360 + atan(inputSignal[2 * index + 1] / inputSignal[2 * index]) / PI * 180;
}

float fft_calculate_phase(FFT_Handler* handler1,FFT_Handler* handler2)
{
    int Amax_pos_1,Amax_pos_2;
    int Amax_1,Amax_2;
    float Amax_phase_1,Amax_phase_2;
    //查找峰值
    arm_max_f32(handler1->FFT_OutputBuf,handler1->FFT_LENGTH/2,&Amax_1,&Amax_pos_1);
    arm_max_f32(handler2->FFT_OutputBuf,handler2->FFT_LENGTH/2,&Amax_2,&Amax_pos_2);

    Amax_phase_1 = Phase_atan(handler1->FFT_InputBuf,Amax_pos_1);
    Amax_phase_2 = Phase_atan(handler2->FFT_InputBuf,Amax_pos_2);
    float Phase = Amax_phase_1 - Amax_phase_2;

    if (Phase > 180)
        Phase = -360 + Phase;
    else if (Phase < -180)
        Phase = 360 + Phase;
    return Phase;
};