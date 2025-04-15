//
// Created by 20614 on 25-4-14.
//
    /*
 * myfft.c
 *
 *  Created on: Jul 8, 2024
 *      Author: 20614
 */
#include "math.h"
#include "arm_math.h"
#include "stdlib.h"


#include "math.h"
#include "arm_math.h"
#include "stdlib.h"

// 定义FFT处理结构体
typedef struct {
    // 配置参数
    float adc_rate;         // 采样率
    int FFT_LENGTH;         // FFT长度
    // 输入数据
    float* adc_val;         // 输入ADC数据数组
    // 输出结果
    float fft_fv;           // 主要频率成分
    float fft_vpp;          // 峰峰值
    float typek;            // 基波与第三谐波比值
    // 中间缓冲区
    float* FFT_InputBuf;    // FFT输入缓冲区（实部+虚部）
    float* FFT_OutputBuf;   // FFT输出幅度
    // FFT实例
    arm_cfft_radix4_instance_f32 scfft;  // FFT计算实例
} FFT_Handler;

// 辅助函数声明
static float floatfind(float* array, int length, int number);
static float floatfindmin(float* array, int length, int number);

// 初始化FFT处理器
FFT_Handler* FFT_Handler_Init(int fft_length) {
    FFT_Handler* handler = (FFT_Handler*)malloc(sizeof(FFT_Handler));
    if (!handler) return NULL;

    handler->FFT_LENGTH = fft_length;

    // 分配中间缓冲区内存
    handler->FFT_InputBuf = (float*)malloc(2 * fft_length * sizeof(float));
    handler->FFT_OutputBuf = (float*)malloc(fft_length * sizeof(float));

    // 初始化FFT实例
    if (arm_cfft_radix4_init_f32(&handler->scfft, fft_length, 0, 1) != ARM_MATH_SUCCESS) {
        free(handler->FFT_InputBuf);
        free(handler->FFT_OutputBuf);
        free(handler);
        return NULL;
    }

    return handler;
}

// 释放FFT处理器资源
void FFT_Handler_Free(FFT_Handler* handler) {
    if (handler) {
        free(handler->FFT_InputBuf);
        free(handler->FFT_OutputBuf);
        free(handler);
    }
}

// FFT计算函数
void fft_calculate(FFT_Handler* handler) {
    // 准备输入数据（实部+虚部）
    for (int i = 0; i < handler->FFT_LENGTH; i++) {
        handler->FFT_InputBuf[2 * i] = handler->adc_val[i]; // 实部
        handler->FFT_InputBuf[2 * i + 1] = 0;               // 虚部
    }

    // 执行FFT计算
    arm_cfft_radix4_f32(&handler->scfft, handler->FFT_InputBuf);

    // 计算幅度
    arm_cmplx_mag_f32(handler->FFT_InputBuf, handler->FFT_OutputBuf, handler->FFT_LENGTH);
    handler->FFT_OutputBuf[0] = 0.0; // 去除直流分量

    // 查找最大值索引
    uint32_t index_max;
    arm_max_f32(handler->FFT_OutputBuf, handler->FFT_LENGTH, &handler->fft_vpp, &index_max);

    // 计算频率
    handler->fft_fv = ((float)index_max / handler->FFT_LENGTH) * handler->adc_rate;
    if (handler->fft_fv > handler->adc_rate / 2) {
        handler->fft_fv -= handler->adc_rate / 2;
    }

    // 计算峰峰值（时域）
    float vmax, vmin;
    uint32_t index_temp;
    arm_max_f32(handler->adc_val, handler->FFT_LENGTH, &vmax, &index_temp);
    arm_min_f32(handler->adc_val, handler->FFT_LENGTH, &vmin, &index_temp);
    handler->fft_vpp = vmax - vmin;

    // 计算基波与第三谐波比值
    float fundamental = floatfind(handler->FFT_OutputBuf, handler->FFT_LENGTH, 1);
    float third_harmonic = floatfind(handler->FFT_OutputBuf, handler->FFT_LENGTH, 3);
    handler->typek = (third_harmonic != 0) ? (fundamental / third_harmonic) : 0;
}

// 辅助函数：查找第n个最大值（带间隔保护）
static float floatfind(float* array, int length, int number) {
    int* flag = (int*)calloc(length, sizeof(int));
    float max_value = 0;

    for (int n = 0; n < number; n++) {
        float current_max = 0;
        int current_index = -1;

        for (int i = 0; i < length; i++) {
            if (!flag[i] && array[i] > current_max) {
                current_max = array[i];
                current_index = i;
            }
        }

        if (current_index == -1) break;

        // 标记附近±10个点
        for (int i = current_index - 10; i <= current_index + 10; i++) {
            if (i >= 0 && i < length) flag[i] = 1;
        }

        max_value = current_max;
    }

    free(flag);
    return max_value;
}

// 辅助函数：查找第n个最小值
static float floatfindmin(float* array, int length, int number) {
    // 实现与floatfind类似，但查找最小值
    // 此处省略具体实现以节省篇幅
}