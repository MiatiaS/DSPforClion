#ifndef __MY_FFT_H
#define __MY_FFT_H

#ifdef __cplusplus
 extern "C" {
#endif

     /* 依赖头文件 */
#include "arm_math.h"
#include "fft.h"
#include <stdint.h>
     /* 实例代码 */
     /*
      *
      *
     *#include "my_fft.h"

// 1. 初始化
FFT_Handler* fft = FFT_Handler_Init(1024);

// 2. 配置参数
fft->adc_rate = 50000;     // 50kHz采样率
fft->adc_val = sample_buf; // 指向采样缓冲区

// 3. 执行计算
fft_calculate(fft);

// 4. 读取结果
printf("Frequency: %.2f Hz\n", fft->fft_fv);
printf("Vpp: %.2f V\n", fft->fft_vpp);

// 5. 释放资源
FFT_Handler_Free(fft);
      *
      *
      **/
     /* FFT处理器结构体声明 */
     typedef struct {
         /* 配置参数 */
         float adc_rate;         // 采样频率(单位:Hz)
         int FFT_LENGTH;         // FFT点数

         /* 输入数据指针 */

         /* 输出结果 */
         float fft_fv;           // 基波频率(单位:Hz)
         float fft_vpp;          // 时域峰峰值
         float fft_rms;
         float typek;            // 基波与三次谐波比值

         /* 内部缓冲区（由init函数分配）*/
         float* FFT_InputBuf;    // FFT输入缓冲区(实部+虚部)
         float* FFT_OutputBuf;   // FFT幅度输出缓冲区

         struct compx* buffer ;

         uint16_t* adc_buf;
         float*    adc_val;         // 指向ADC采样数据数组

         //模拟量
         /* FFT实例 */
         arm_cfft_radix4_instance_f32 scfft;  // CMSIS-DSP FFT实例
     } FFT_Handler;

     /* 接口函数声明 */

     /**
       * @brief  初始化FFT处理器
       * @param  fft_length FFT点数（建议使用2的幂）
       * @retval 成功返回处理器指针，失败返回NULL
       */
     FFT_Handler* FFT_Handler_Init(uint32_t fft_length);

     /**
       * @brief  释放FFT处理器资源
       * @param  handler 处理器指针
       */
     void FFT_Handler_Free(FFT_Handler* handler);

     /**
       * @brief  执行FFT计算
       * @param  handler 处理器指针
       * @note 计算结果存储在handler的输出字段中：
       *       - fft_fv: 基波频率
       *       - fft_vpp: 时域峰峰值
       *       - typek: 基波/三次谐波比值
       */
     void fft_calculate(FFT_Handler* handler);

     void fft_calculate_harmonic(FFT_Handler* handler);
     void fft_calculate_mainfreq(FFT_Handler* handler);
     void fft_calculate_rms(FFT_Handler* handler);
     void fft_calculate_vpp(FFT_Handler* handler);
     static void fft_myfly(FFT_Handler* handler);
     void ultrafft(FFT_Handler* handler);


     void fft_calculate_over2(FFT_Handler* handler);


#ifdef __cplusplus
 }
#endif

#endif /* __MY_FFT_H */