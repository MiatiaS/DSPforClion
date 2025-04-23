//
// Created by 20614 on 25-4-16.
//

#include "fft_disp.h"
#include "lcd.h"
#include "lcd_init.h"
#include "myfft.h"
//DEFINE后不要加分号
#define Screen_Width 300

//设定横向显示 占用小部分屏幕
/*
	函数说明：显示FFT过后的频谱
	输入：coefficient是幅度缩放比例 xaxis是位数 如4096是12
	修改横轴宽度请到.c操作
*/
void fft_freq_disp(FFT_Handler* FFT_Handle,int coefficient)
{
 	int idx_width = FFT_Handle->FFT_LENGTH / Screen_Width; //规定每一个像素点间的FFT点数间隔
    int i;
    for(i = 0; i < Screen_Width; i++)
        {
    	  int spot1 = 155 - ((int)FFT_Handle->FFT_OutputBuf[i] / coefficient);
    	  int spot1_idx = (int)(i /(FFT_Handle->FFT_LENGTH / 128)  );
    	  LCD_DrawPoint(spot1_idx,spot1,YELLOW);
    	  int spot2 = 155 - ((int)FFT_Handle->FFT_OutputBuf[i+1] / coefficient);
    	  int spot2_idx = (int)(i+1 /(FFT_Handle->FFT_LENGTH / 128)  );
    	  LCD_DrawPoint(spot2_idx,spot2,YELLOW);
    	  LCD_DrawLine(spot1_idx,spot1,spot2_idx,spot2,YELLOW);
    	};
}
