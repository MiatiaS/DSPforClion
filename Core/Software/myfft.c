/******************************************************************************/
/*                                FFT处理模块                                 */
/*                  版本：1.0                                                 */
/*                  作者：20614                                               */
/*                  说明：FFT计算、谐波分析、波形检测等功能实现               */
/******************************************************************************/

//------------------------------- 头文件包含 -----------------------------------
#include "math.h"
#include "arm_math.h"
#include "stdlib.h"
#include "myfft.h"
#include "fft.h"

//----------------------------- 静态函数声明 ------------------------------------
static void ultrafft(FFT_Handler* handler);                    // 优化FFT计算
static void check_fft_harmonic(FFT_Handler* handler, int order); // 谐波检测
static float floatfindmax(float* array, int length, int number); // 查找第N大值
static float floatfindmin(float* array, int length, int number); // 查找第N小值

//----------------------------- 全局变量声明 ------------------------------------
int fft_idx_max;  // 频谱最大幅值对应索引（用于频域峰值检测）

//--------------------------- FFT处理器初始化 ----------------------------------
/**
  * @brief  初始化FFT处理器
  * @param  fft_length FFT点数（必须为2的幂次）
  * @retval FFT处理器句柄
  */
FFT_Handler* FFT_Handler_Init(uint32_t fft_length)
{
    // 分配处理器内存
    FFT_Handler* handler = (FFT_Handler*)malloc(sizeof(FFT_Handler));
    handler->harmonic = (harmonic*)malloc(20 * sizeof(harmonic)); // 预分配20个谐波存储空间
    // 内存分配失败处理
    if (!handler) return NULL;
    // 初始化基础参数
    handler->FFT_LENGTH = fft_length;

    // 分配各缓冲区内存
    handler->adc_buf    = (uint16_t*)malloc(fft_length * sizeof(uint16_t));
    handler->adc_val    = (float*)malloc(fft_length * sizeof(float));
    handler->FFT_InputBuf  = (float*)malloc(2 * fft_length * sizeof(float));   // 复数输入缓存
    handler->FFT_OutputBuf = (float*)malloc(fft_length * sizeof(float));       // 幅值输出缓存
    handler->buffer     = (struct compx*)malloc(handler->FFT_LENGTH * sizeof(struct compx));

    // 缓冲区分配失败处理
    if (handler->buffer == NULL) return;

    // 初始化FFT算法实例（仅处理4096点）
    if (fft_length == 4096) {
        if (arm_cfft_radix4_init_f32(&handler->scfft, fft_length, 0, 1) != ARM_MATH_SUCCESS) {
            // 初始化失败后资源释放
            free(handler->FFT_InputBuf);
            free(handler->FFT_OutputBuf);
            free(handler);
            return NULL;
        }
    }
    return handler;
}

//--------------------------- FFT处理器资源释放 --------------------------------
/**
  * @brief  释放FFT处理器相关资源
  * @param  handler FFT处理器句柄
  */
void FFT_Handler_Free(FFT_Handler* handler)
{
    if (handler) {
        free(handler->FFT_InputBuf);
        free(handler->FFT_OutputBuf);
        free(handler);
    }
}

//--------------------------- 主FFT计算函数（对外接口）------------------------
/**
  * @brief  执行FFT计算及后续分析
  * @param  handler FFT处理器句柄
  */
void fft_calculate(FFT_Handler* handler)
{
    /* 准备复数输入数据（实部+虚部） */
    for (int i = 0; i < handler->FFT_LENGTH; i++) {
        handler->FFT_InputBuf[2 * i]     = handler->adc_val[i]; // 实部填充ADC数据
        handler->FFT_InputBuf[2 * i + 1] = 0;                   // 虚部初始化为0
    }

    /* 执行优化的FFT计算 */
    ultrafft(handler);

    /* 执行各类后续分析 */
    fft_calculate_mainfreq(handler);   // 计算主频
    fft_calculate_rms(handler);        // 计算有效值
    fft_calculate_vppf(handler);       // 频域峰峰值计算
}

//--------------------------- 优化的FFT计算实现 --------------------------------
/**
  * @brief  优化的FFT计算流程
  * @param  handler FFT处理器句柄
  */
static void ultrafft(FFT_Handler* handler)
{
    /* 准备复数输入数据 */
    for (int i = 0; i < handler->FFT_LENGTH; i++) {
        handler->buffer[i].real = handler->adc_val[i];  // 实部为ADC采样值
        handler->buffer[i].imag = 0.0f;                // 虚部清零
    }

    /* 执行基4 FFT计算 */
    cfft(handler->buffer, handler->FFT_LENGTH);

    /* 重组FFT结果到输入缓冲区 */
    for (int i = 0; i < handler->FFT_LENGTH; i++) {
        handler->FFT_InputBuf[2 * i]     = handler->buffer[i].real;
        handler->FFT_InputBuf[2 * i + 1] = handler->buffer[i].imag;
    }

    /* 计算复数幅值（频谱幅值） */
    for (int i = 0; i < handler->FFT_LENGTH; i++) {
        handler->FFT_OutputBuf[i] = sqrtf(
            handler->buffer[i].real * handler->buffer[i].real +
            handler->buffer[i].imag * handler->buffer[i].imag
        );
    }
}

//--------------------------- 主频计算 -----------------------------------------
/**
  * @brief  计算信号主频
  * @param  handler FFT处理器句柄
  */
void fft_calculate_mainfreq(FFT_Handler* handler)
{
    // 去除直流分量（前两个点）
    for (int i = 0; i < 2; i++) {
        handler->FFT_OutputBuf[i] = 0.0f;
    }

    // 查找最大幅值及其索引
    uint32_t index_max;
    arm_max_f32(handler->FFT_OutputBuf, handler->FFT_LENGTH / 2, &handler->fft_vpp, &index_max);

    // 计算实际频率值
    handler->fft_fv = ((float)index_max / handler->FFT_LENGTH) * handler->adc_rate;

    // 频率超过Nyquist频率处理
    if (handler->fft_fv > handler->adc_rate / 2) {
        handler->fft_fv -= handler->adc_rate / 2;
    }
}

//--------------------------- 有效值计算（时域法）-----------------------------
/**
  * @brief  计算信号有效值（RMS）
  * @param  handler FFT处理器句柄
  */
void fft_calculate_rms(FFT_Handler* handler)
{
    arm_rms_f32(handler->adc_val, handler->FFT_LENGTH, &handler->fft_rms);
}

//--------------------------- 峰峰值计算（时域法）-----------------------------
/**
  * @brief  时域峰峰值计算
  * @param  handler FFT处理器句柄
  */
void fft_calculate_vpp(FFT_Handler* handler)
{
    float max, min;
    int idx_max, idx_min;

    arm_max_f32(handler->adc_val, handler->FFT_LENGTH, &max, &idx_max);
    arm_min_f32(handler->adc_val, handler->FFT_LENGTH, &min, &idx_min);

    handler->fft_vpp = (max - min);  // 经验值滤波，抖动约5mV
}

//--------------------------- 峰峰值计算（频域法）-----------------------------
/**
  * @brief  基于频域分析的峰峰值计算
  * @param  handler FFT处理器句柄
  */
void fft_calculate_vppf(FFT_Handler* handler)
{
    float fft_max;
    arm_max_f32(handler->FFT_OutputBuf, handler->FFT_LENGTH / 2, &fft_max, &fft_idx_max);

    // 取主峰附近±2个点计算能量
    int ctr_cnt = 2;
    float num_total = 0.0f;
    float num_sqrt = 0.0f;

    for (int i = -ctr_cnt; i < ctr_cnt + 1; i++) {
        num_total += handler->FFT_OutputBuf[fft_idx_max + i] * handler->FFT_OutputBuf[fft_idx_max + i];
    }

    // 能量累加后开方，并转换为峰峰值
    arm_sqrt_f32(num_total, &num_sqrt);
    handler->fft_vpp = num_sqrt / handler->FFT_LENGTH * 2 * 2 * 2; // 幅值转峰峰值系数
}

//--------------------------- 波形类型检测 ------------------------------------
/**
  * @brief  波形类型检测（正弦波/方波/三角波）
  * @param  handler FFT处理器句柄
  */
void fft_wave_detect(FFT_Handler* handler)
{
    // 获取基波和三次谐波幅值
    check_fft_harmonic(handler, 3);
    float fft_Val1 = floatfindmax(handler->FFT_OutputBuf, handler->FFT_LENGTH/2, 1);
    float fft_Val2 = floatfindmax(handler->FFT_OutputBuf, handler->FFT_LENGTH/2, 2);
    float type_k = fft_Val1 / fft_Val2;

    // 根据幅值比判断波形类型
    if (type_k < 5) {
        handler->wave = 1;   // 方波（基波与三次谐波比约3:1）
    } else if (type_k > 5 && type_k < 13) {
        handler->wave = 2;   // 三角波（幅值比约9:1）
    } else if (type_k > 13) {
        handler->wave = 0;   // 正弦波（三次谐波接近0）
    }
}

//--------------------------- 谐波分析 ----------------------------------------
/**
  * @brief  谐波分量检测
  * @param  handler FFT处理器句柄
  * @param  order   要检测的谐波次数
  */
static void check_fft_harmonic(FFT_Handler* handler, int order)
{
    float fft_max;
    int fft_idx_max;       // 主频索引
    float fft_current_max = 0.0f;
    int true_idx = 0;      // 真实谐波位置

    // 获取主频位置
    arm_max_f32(handler->FFT_OutputBuf, handler->FFT_LENGTH / 2, &fft_max, &fft_idx_max);

    // 在理论谐波位置附近搜索最大幅值
    int search_start = (fft_idx_max * order) - 3;
    int search_end = (fft_idx_max * order) + 3;

    for (int i = search_start; i < search_end; i++) {
        if (i < 0 || i >= handler->FFT_LENGTH/2) continue;

        if (handler->FFT_OutputBuf[i] > fft_current_max) {
            fft_current_max = handler->FFT_OutputBuf[i];
            true_idx = i;
        }
    }

    // 记录谐波信息
    handler->harmonic[order].amplitude = fft_current_max;
    handler->harmonic[order].idx = true_idx;
    handler->harmonic[order].order = order;
}

//--------------------------- 辅助函数：查找第N大值 ---------------------------
/**
  * @brief  查找数组中第N大的值（带保护间隔）
  * @param  array   数据数组
  * @param  length  数组长度
  * @param  number  要查找的第N大值序号
  * @retval 第N大的值
  */
static float floatfindmax(float* array, int length, int number)
{
    int* flag = (int*)malloc(length * sizeof(int));
    memset(flag, 0, length * sizeof(int));
    float max_value = 0.0f;

    for (int n = 0; n < number; n++) {
        float current_max = 0.0f;
        int current_index = -1;

        // 遍历查找当前最大值
        for (int i = 0; i < length; i++) {
            if (flag[i] == 0 && array[i] > current_max) {
                current_max = array[i];
                current_index = i;
            }
        }
        if (current_index == -1) break;

        // 标记附近±10个点（避免频谱泄露干扰）
        for (int i = current_index - 10; i <= current_index + 10; i++) {
            if (i >= 0 && i < length) flag[i] = 1;
        }

        max_value = current_max;
    }

    free(flag);
    return max_value;
}

//--------------------------- 辅助函数：查找第N小值 ---------------------------
/**
  * @brief  查找数组中第N小的值（暂未实现）
  * @param  array   数据数组
  * @param  length  数组长度
  * @param  number  要查找的第N小值序号
  * @retval 第N小的值
  */
static float floatfindmin(float* array, int length, int number)
{
    // 实现思路与floatfindmax类似，此处省略具体实现
    return 0.0f;
}