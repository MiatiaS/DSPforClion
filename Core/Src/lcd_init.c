#include "lcd_init.h"
#include "spi.h"

/******************************************************************************
	  函数说明：LCD串行数据写入函数
	  入口数据：dat  要写入的串行数据
	  返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{

	LCD_CS_Clr();
	HAL_SPI_Transmit(&hspi2, &dat, 1, 100);
	// HAL_SPI_Transmit_DMA(&hspi1, &dat,sizeof(dat));//DMA刷新更慢。因为DMA擅长一次搬运大量数据，但是下面的代码是一个个搬运数据。具体应该设立一个显存。参见波特律动代码。
	// 使用方法：cubeMX界面中SPI>DMA>SPI1-TX 选bite normal
	LCD_CS_Set();
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  函数说明：LCD写入命令
	  入口数据：dat 写入的命令
	  返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr(); // 写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set(); // 写数据
}

/******************************************************************************
	  函数说明：设置起始和结束地址
	  入口数据：x1,x2 设置列的起始和结束地址
				y1,y2 设置行的起始和结束地址
	  返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 1);
		LCD_WR_DATA(y2 + 1);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 3);
		LCD_WR_DATA(y2 + 3);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 1);
		LCD_WR_DATA(x2 + 1);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 3)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 3);
		LCD_WR_DATA(x2 + 3);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 4)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 5)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
}
/*函数说明
void LCD_ST7735R_Init(void) 主控为ST7735R的LCD屏幕初始化
void LCD_ST7735S_Init(void) 主控为ST7735S的LCD屏幕初始化
还要在lcd_init.h中设置显示方向
*/
void LCD_ST7735R_Init(void)
{

	LCD_RES_Clr(); // 复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_BLK_Set(); // 打开背光
	HAL_Delay(100);

	LCD_WR_REG(0x11); // Sleep out
	HAL_Delay(120);	  // Delay 120ms

	// LCD Init For 1.44Inch LCD Panel with ST7735R.
	// ST7735R Frame Rate
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x2D);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x2D);

	LCD_WR_REG(0xB3);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x2D);

	LCD_WR_REG(0xB4); // Column inversion
	LCD_WR_DATA8(0x07);

	// ST7735R Power Sequence
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0xA2);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8A);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8A);
	LCD_WR_DATA8(0xEE);

	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x0E);

	LCD_WR_REG(0x36); // MX, MY, RGB mode
	if (USE_HORIZONTAL == 4)
		LCD_WR_DATA8(0xC0); // 标记0xC0  正置 ；0x00倒置 0xA0顺时针旋转90度
	else if (USE_HORIZONTAL == 5)
		LCD_WR_DATA8(0x00); // 标记0xC0  正置 ；0x00倒置 0xA0顺时针旋转90度

	// ST7735R Gamma Sequence
	LCD_WR_REG(0xe0);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x1a);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x2f);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x1f);
	LCD_WR_DATA8(0x1b);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xe1);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x1b);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x2c);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0x2e);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x3f);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0x2a);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x7f);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x9f);

	LCD_WR_REG(0xF0); // Enable test command
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xF6); // Disable ram power save mode
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x3A); // 65k mode
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0x29); // Display on
}

/*函数说明
void LCD_ST7735R_Init(void) 主控为ST7735R的LCD屏幕初始化
void LCD_ST7735S_Init(void) 主控为ST7735S的LCD屏幕初始化
还要在lcd_init.h中设置显示方向
*/
void LCD_ST7735S_Init(void)
{

	LCD_RES_Clr(); // 复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_BLK_Set(); // 打开背光
	HAL_Delay(100);

	LCD_WR_REG(0x11); // Sleep out
	HAL_Delay(120);	  // Delay 120ms
	//------------------------------------ST7735S Frame rate-------------------------------------------------//
	LCD_WR_REG(0xB1); // Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB2); // Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB3); // Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	//------------------------------------End ST7735S Frame rate-------------------------------------------//
	LCD_WR_REG(0xB4); // Dot inversion
	LCD_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence-----------------------------------------//
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0xA2);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence---------------------------------------//
	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x0a);
	LCD_WR_REG(0x36);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x08);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC8);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x78);
	else
		LCD_WR_DATA8(0xA8);
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0XE0);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	LCD_WR_REG(0XE1);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0x3A); // 65k mode
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x29); // Display on
}

void LCD_ST7789_Init(void)
{
	LCD_RES_Clr(); // 复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_BLK_Set(); // 打开背光
	HAL_Delay(100);

	LCD_WR_REG(0x01); // Software Reset
	HAL_Delay(120);

	LCD_WR_REG(0x11); // Sleep Out
	HAL_Delay(120);	  // DELAY120ms

	//-----------------------ST7789V Frame rate setting-----------------//
	//************************************************
	LCD_WR_REG(0x3A); // 65k mode
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x1A);
	LCD_WR_REG(0x36); // 屏幕显示方向设置
	LCD_WR_DATA8(0x00);
	//-------------ST7789V Frame rate setting-----------//
	LCD_WR_REG(0xb2); // Porch Setting
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xb7);	// Gate Control
	LCD_WR_DATA8(0x05); // 12.2v   -10.43v
	//--------------ST7789V Power setting---------------//
	LCD_WR_REG(0xBB); // VCOM
	LCD_WR_DATA8(0x3F);

	LCD_WR_REG(0xC0); // Power control
	LCD_WR_DATA8(0x2c);

	LCD_WR_REG(0xC2); // VDV and VRH Command Enable
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);	// VRH Set
	LCD_WR_DATA8(0x0F); // 4.3+( vcom+vcom offset+vdv)

	LCD_WR_REG(0xC4);	// VDV Set
	LCD_WR_DATA8(0x20); // 0v

	LCD_WR_REG(0xC6);	// Frame Rate Control in Normal Mode
	LCD_WR_DATA8(0X01); // 111Hz

	LCD_WR_REG(0xd0); // Power Control 1
	LCD_WR_DATA8(0xa4);
	LCD_WR_DATA8(0xa1);

	LCD_WR_REG(0xE8); // Power Control 1
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xE9); // Equalize time control
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	//---------------ST7789V gamma setting-------------//
	LCD_WR_REG(0xE0); // Set Gamma
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x30);

	LCD_WR_REG(0XE1); // Set Gamma
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x24);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x2F);

	LCD_WR_REG(0x20); // 反显
	HAL_Delay(120);
	LCD_WR_REG(0x29); // 开启显示
}

void LCD_ST7789V3_Init(void)
{

	LCD_RES_Clr(); // 复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_BLK_Set(); // 打开背光
	HAL_Delay(100);

	LCD_WR_REG(0x01); // Software Reset
	HAL_Delay(120);

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); // Sleep out
	HAL_Delay(120);	  // Delay 120ms
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x36);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x00);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC0);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x70);
	else
		LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA8(0x72);

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x3d); // 2b

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x19);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20); // VDV, 0x20:0v

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x0f); // 0x13:60Hz

	LCD_WR_REG(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6);
	LCD_WR_DATA8(0xA1); // sleep in后，gate输出为GND

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);
	/* 电压设置 */
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);
	/* 显示开 */
	LCD_WR_REG(0x21);
	LCD_WR_REG(0x29);
}