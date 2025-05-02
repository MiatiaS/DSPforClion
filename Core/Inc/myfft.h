#ifndef __MY_FFT_H
#define __MY_FFT_H

#ifdef __cplusplus
extern "C" {
#endif

/* 依赖头文件 ------------------------------------------------------------*/
#include "arm_math.h"
#include "fft.h"
#include <stdint.h>

/* 类型定义 --------------------------------------------------------------*/

/**
  * @brief  谐波信息结构体
  * @note   用于存储谐波分析结果
  */
typedef struct {
    int order;           ///< 谐波次数（1=基波，3=三次谐波等）
    int idx;             ///< 谐波分量在频谱数组中的索引位置
    float amplitude;     ///< 谐波分量幅值（线性刻度）
} harmonic;

/**
  * @brief  FFT处理器结构体
  * @note   包含FFT配置参数、分析结果及内部缓冲区
  */
typedef struct {
    /* 配置参数 */
    float adc_rate;      ///< ADC采样率（单位：Hz）
    int FFT_LENGTH;      ///< FFT变换点数（建议使用2的幂）

    /* 输入数据指针 */
    uint16_t* adc_buf;   ///< 原始ADC采样缓冲区（uint16格式）
    float* adc_val;      ///< 预处理后的采样数据（float格式）

    /* 分析结果 */
    float fft_fv;        ///< 基波频率（单位：Hz）
    float fft_vpp;        ///< 信号峰峰值（时域或频域计算）
    float fft_rms;       ///< 信号有效值（RMS）
    float typek;         ///< 基波与三次谐波幅值比
    int wave;            ///< 波形类型（0=正弦波，1=方波，2=三角波）

    /* 内部缓冲区 */
    float* FFT_InputBuf;  ///< FFT输入缓冲区（复数格式：实部+虚部）
    float* FFT_OutputBuf; ///< FFT输出幅度谱
    struct compx* buffer; ///< 复数运算中间缓冲区

    /* DSP库实例 */
    arm_cfft_radix4_instance_f32 scfft; ///< CMSIS-DSP FFT实例

    /* 谐波分析结果 */
    harmonic* harmonic;  ///< 谐波信息数组（最大支持20次谐波）
} FFT_Handler;

/* 函数声明 --------------------------------------------------------------*/

/**
  * @brief  初始化FFT处理器
  * @param  fft_length FFT点数（必须为2的幂，如256/512/1024等）
  * @retval FFT_Handler* 成功返回处理器指针，失败返回NULL
  * @note   该函数会分配所有需要的内存缓冲区，初始化FFT配置
  */
FFT_Handler* FFT_Handler_Init(uint32_t fft_length);

/**
  * @brief  释放FFT处理器资源
  * @param  handler 要释放的处理器指针
  * @note   安全处理NULL指针，释放所有相关内存
  */
void FFT_Handler_Free(FFT_Handler* handler);

/**
  * @brief  执行完整FFT分析流程
  * @param  handler 已初始化的处理器指针
  * @note   依次执行以下分析：
  *         - FFT变换
  *         - 基波频率计算
  *         - RMS有效值计算
  *         - 峰峰值计算
  */
void fft_calculate(FFT_Handler* handler);

/**
  * @brief  谐波分析计算
  * @param  handler 处理器指针
  * @note   计算基波与三次谐波幅值比，结果存入typek字段
  *         使用保护间隔法避免频谱泄漏影响
  */
void fft_calculate_harmonic(FFT_Handler* handler);

/**
  * @brief  基波频率计算
  * @param  handler 处理器指针
  * @note   通过频谱峰值检测计算信号主频，考虑混叠情况
  *         结果存入fft_fv字段（单位：Hz）
  */
void fft_calculate_mainfreq(FFT_Handler* handler);

/**
  * @brief  有效值计算（时域）
  * @param  handler 处理器指针
  * @note   使用arm_rms_f32计算时域有效值，结果存入fft_rms
  */
void fft_calculate_rms(FFT_Handler* handler);

/**
  * @brief  时域峰峰值计算
  * @param  handler 处理器指针
  * @note   通过查找时域信号最大最小值计算Vpp
  *         结果存入fft_vpp字段
  */
void fft_calculate_vpp(FFT_Handler* handler);

/**
  * @brief  频域峰峰值计算
  * @param  handler 处理器指针
  * @note   基于主频附近能量计算峰峰值，考虑窗函数补偿
  *         结果存入fft_vpp字段，精度优于时域法
  */
void fft_calculate_vppf(FFT_Handler* handler);

/**
  * @brief  波形类型检测
  * @param  handler 处理器指针
  * @note   通过谐波分析判断波形类型：
  *         0=正弦波，1=方波，2=三角波
  *         结果存入wave字段
  */
void fft_wave_detect(FFT_Handler* handler);

#ifdef __cplusplus
}
#endif

#endif /* __MY_FFT_H */