//
// Created by 20614 on 25-4-16.
//

#include "fft_disp.h"
#include "lcd.h"
#include "lcd_init.h"
#include "myfft.h"
//DEFINE后不要加分号
#define Screen_Width 260

//设定横向显示 占用小部分屏幕
/*
	函数说明：显示FFT过后的频谱
	输入：coefficient是幅度缩放比例
	修改横轴宽度请到.c操作
*/
void fft_freq_disp(FFT_Handler* FFT_Handle,float coefficient)
{
 	int idx_width = FFT_Handle->FFT_LENGTH / Screen_Width / 2; //规定每一个像素点间的FFT点数间隔,
    int i;
    for(i = 0; i < Screen_Width; i = i + 1)
    {
	    int temp1 = 220 - ((int)FFT_Handle->FFT_OutputBuf[i * idx_width] / coefficient);
    	int spot1;
    	if (temp1 > 0)//防溢出检查
    	{
    		spot1 = temp1;
    	}
    	else
    	{
    		spot1 = 0;
    	}
    	int spot1_idx = 30 + i;
    	LCD_DrawPoint(spot1_idx,spot1,YELLOW);

    	int temp2 = 220 - ((int)FFT_Handle->FFT_OutputBuf[(i+1) * idx_width] / coefficient);
    	int spot2;
    	if (temp2 > 0)
    	{
    		spot2 = temp2;
    	}
    	else
    	{
    		spot2 = 0;
    	}
    	  int spot2_idx = 30 + i + 1;
    	  LCD_DrawPoint(spot2_idx,spot2,YELLOW);

    	  LCD_DrawLine(spot1_idx,spot1,spot2_idx,spot2,BLUE);
    	  //LCD_DrawLine(spot1_idx,spot1,spot2_idx,spot2,YELLOW);
    	};
}
