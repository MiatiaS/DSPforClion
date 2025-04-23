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
        #include "myfft.h"
        #include "fft.h"
        #include "math.h"
        #include "arm_math.h"
        #include "stdlib.h"

        // 定义FFT处理结构体


        // 辅助函数声明
        static float floatfindmax(float* array, int length, int number);
        static float floatfindmin(float* array, int length, int number);

        // 初始化FFT处理器
        FFT_Handler* FFT_Handler_Init(int fft_length)
        {
            InitTableFFT(fft_length);
            FFT_Handler* handler = (FFT_Handler*)malloc(sizeof(FFT_Handler));
            if (!handler) return NULL;

            handler->FFT_LENGTH = fft_length;

            // 分配中间缓冲区内存
            handler->FFT_InputBuf  = (float*)malloc(2 * fft_length * sizeof(float));
            handler->FFT_OutputBuf = (float*)malloc(fft_length * sizeof(float));
            handler->buffer = (struct compx*)malloc(handler->FFT_LENGTH * sizeof(struct compx));
            if (handler->buffer == NULL)
                return;
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
            for (int i = 0; i < handler->FFT_LENGTH; i++)
            {
                handler->FFT_InputBuf[2 * i] = handler->adc_val[i]; // 实部
                handler->FFT_InputBuf[2 * i + 1] = 0;               // 虚部
            }

            // 执行FFT计算
           //arm_cfft_radix4_f32(&handler->scfft, handler->FFT_InputBuf);
            //这时候的FFT_InputBuf同时保存着实部和虚部的信息
            // 计算幅度
            //arm_cmplx_mag_f32(handler->FFT_InputBuf, handler->FFT_OutputBuf, handler->FFT_LENGTH);

            for (int i = 0; i < handler->FFT_LENGTH; i++)
            {
                handler->buffer[i].real = handler->adc_val[i];  // 从ADC获取实数输入
                handler->buffer[i].imag = 0.0f;                // 虚部初始化为0
            }
            cfft(handler->buffer, handler->FFT_LENGTH);
            for (int i = 0; i < handler->FFT_LENGTH; i++)
            {
                handler->FFT_InputBuf[2 * i] = handler->buffer[i].real;
                handler->FFT_InputBuf[2 * i + 1] = handler->buffer[i].imag;
            }
            for (int i = 0; i < handler->FFT_LENGTH; i++)
            {
                handler->FFT_OutputBuf[i] = sqrtf(
                    handler->buffer[i].real * handler->buffer[i].real +
                    handler->buffer[i].imag * handler->buffer[i].imag
                );
            }

            fft_calculate_mainfreq(handler);
            fft_calculate_harmonic(handler);
            fft_calculate_rms(handler);
            fft_calculate_vpp(handler);

        }





        /*
         *@brief：计算信号主峰与次峰的比值
         *@输入：FFT_Handler句柄
         *@返回：更新句柄typek参数
         */
        void fft_calculate_harmonic(FFT_Handler* handler)
        {
            float fundamental = floatfindmax(handler->FFT_OutputBuf, handler->FFT_LENGTH, 1);
            float third_harmonic = floatfindmax(handler->FFT_OutputBuf, handler->FFT_LENGTH, 2);
            handler->typek = (third_harmonic != 0) ? (fundamental / third_harmonic) : 0;
        }

        /*
         *@brief：计算信号基波频率，需要ADC采样率准确
         *@输入：FFT_Handler句柄
         *@返回：更新句柄fft_fv参数
         */
        void fft_calculate_mainfreq(FFT_Handler* handler)
        {
            handler->FFT_OutputBuf[0] = 0.0; // 去除直流分量,这是为了找第一个峰值

            // 查找最大值索引
            uint32_t index_max;
            arm_max_f32(handler->FFT_OutputBuf, handler->FFT_LENGTH / 2, &handler->fft_vpp, &index_max);

            // 计算频率
            handler->fft_fv = ((float)index_max / handler->FFT_LENGTH) * handler->adc_rate;
            if (handler->fft_fv > handler->adc_rate / 2)
            {
                handler->fft_fv -= handler->adc_rate / 2;
            }

        }
        void fft_calculate_rms(FFT_Handler* handler)
        {
            //对时域进行计算
            arm_rms_f32(handler->adc_val,handler->FFT_LENGTH,&handler->fft_rms);
            //如果频谱泄露控制的好，可以使用频域计算
        }

        void fft_calculate_vpp(FFT_Handler* handler)
        {
            float max,min;
            int idx_max,idx_min;
            arm_max_f32(handler->adc_val,handler->FFT_LENGTH,&max,&idx_max);
            arm_min_f32(handler->adc_val,handler->FFT_LENGTH,&min,&idx_min);
            handler->fft_vpp = (max - min) ;  //经验值滤波 抖动在5mv左右，如果再通过均值滤波就好了
        }

        void fft_cauculate_vppf(FFT_Handler* handler)
        {
            float max;
            int idx_max;
            arm_max_f32(handler->FFT_OutputBuf,handler->FFT_LENGTH,&max,&idx_max);

        }



        // 辅助函数：查找第n个最大值（带间隔保护） 返回下标
        //此处检查到最大值后，会把左右各10个点过滤掉，不再计算，减少频谱泄露带来的影响
        static float floatfindmax(float* array, int length, int number) {
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




        /*****实验性代码，蝶形运算
         *此部分代码不依赖fft_calculate函数运行
         *******/
        void fft_calculate_over2(FFT_Handler* handler)
        {
            fft_myfly(handler);
        }
        /************生成旋转因子***************/
        static void precomute_twiddle_factors(float32_t *twiddle,uint32_t fft_length)
        {
            for (int i = 0;i <fft_length/2;i++)
            {
                float32_t angle = -2 * PI * i /fft_length;
                twiddle[2*i] = cosf(angle);
                twiddle[2*i+1] = sinf(angle);
            }

        };

        static void fft_myfly(FFT_Handler* handler) {
            int N = 8192; // 总长度
            int N_OVER_2 = 4096;

            // 分配奇偶序列缓冲区
            float32_t* Inputbuf_even = malloc(2 * N_OVER_2 * sizeof(float32_t));
            float32_t* Inputbuf_odd = malloc(2 * N_OVER_2 * sizeof(float32_t));

            // 奇偶分解
            for (int m = 0; m < N_OVER_2; m++) {
                Inputbuf_even[2*m] = handler->adc_val_doubled[2*m]; // 偶数实部
                Inputbuf_even[2*m + 1] = 0; // 虚部为0
                Inputbuf_odd[2*m] = handler->adc_val_doubled[2*m + 1]; // 奇数实部
                Inputbuf_odd[2*m + 1] = 0; // 虚部为0
            }

            // 执行4096点FFT
            arm_cfft_instance_f32* scfft1 ;
            arm_cfft_instance_f32* scfft2 ;
            arm_cfft_init_f32(scfft1,handler->FFT_LENGTH);
            arm_cfft_init_f32(scfft2,handler->FFT_LENGTH);
            arm_cfft_f32(&scfft1, Inputbuf_even, 0, 1);
            arm_cfft_f32(&scfft2, Inputbuf_odd, 0, 1);

            // 预计算8192点旋转因子
            float32_t twiddle[N_OVER_2 * 2]; // 复数存储
            precomute_twiddle_factors(twiddle, N);

            // 蝶形合并
            for (int k = 0; k < N_OVER_2; k++) {
                float32_t tw_real = twiddle[2*k];
                float32_t tw_imag = twiddle[2*k + 1];

                float32_t o_real = Inputbuf_odd[2*k];
                float32_t o_imag = Inputbuf_odd[2*k + 1];

                // 计算 twiddle * O[k]
                float32_t temp_real = o_real * tw_real - o_imag * tw_imag;
                float32_t temp_imag = o_real * tw_imag + o_imag * tw_real;

                // 合并结果
                // 前半部分: X[k] = E[k] + temp
                handler->FFT_InputBuf_over2[2*k] = Inputbuf_even[2*k] + temp_real;
                handler->FFT_InputBuf_over2[2*k + 1] = Inputbuf_even[2*k + 1] + temp_imag;

                // 后半部分: X[k + N_OVER_2] = E[k] - temp
                handler->FFT_InputBuf_over2[2*(k + N_OVER_2)] = Inputbuf_even[2*k] - temp_real;
                handler->FFT_InputBuf_over2[2*(k + N_OVER_2) + 1] = Inputbuf_even[2*k + 1] - temp_imag;
            }

            free(Inputbuf_even);
            free(Inputbuf_odd);
        }