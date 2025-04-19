#ifndef __LCD_INIT_H
#define __LCD_INIT_H
#include "main.h"
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
//-----------显示方向在这里设置---------------
#define USE_HORIZONTAL 2 // ST7735S 0上下倒置，1正置，2逆时针旋转90度，3顺时针旋转90度，ST7735R 4正置，5上下倒置
//------------------------------------------

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 128
#define LCD_H 128 // 实际上并不控制宽和高

#else
#define LCD_W 128
#define LCD_H 128
#endif

//-----------------LCD端口定义----------------

#define LCD_RES_Clr() HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, GPIO_PIN_RESET) // RES
#define LCD_RES_Set() HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, GPIO_PIN_SET)

#define LCD_DC_Clr() HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET) // DC
#define LCD_DC_Set() HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET)

#define LCD_CS_Clr() HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET) // CS
#define LCD_CS_Set() HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET)

#define LCD_BLK_Clr() HAL_GPIO_WritePin(TFT_BL_GPIO_Port, TFT_BL_Pin, GPIO_PIN_RESET) // BLK
#define LCD_BLK_Set() HAL_GPIO_WritePin(TFT_BL_GPIO_Port, TFT_BL_Pin, GPIO_PIN_SET)

// 画笔颜色,在不同背景下同一个颜色的显示有差别
#define WHITE 0xFFFF      // 白色
#define BLACK 0x0000      // 黑色
#define BLUE 0x001F       // 蓝色
#define BRED 0XF81F       // 蓝红色
#define GRED 0XFFE0       // 绿红色
#define GBLUE 0X07FF      // 绿蓝色
#define RED 0xF800        // 红色
#define MAGENTA 0xF81F    // 品红色
#define GREEN 0x07E0      // 绿色
#define CYAN 0x7FFF       // 青色
#define YELLOW 0xFFE0     // 黄色
#define BROWN 0XBC40      // 棕色
#define BRRED 0XFC07      // 棕红色
#define GRAY 0X8430       // 灰色
#define DARKBLUE 0X01CF   // 深蓝色
#define LIGHTBLUE 0X7D7C  // 浅蓝色
#define GRAYBLUE 0X5458   // 灰蓝色
#define LIGHTGREEN 0X841F // 浅绿色
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE 0XA651  // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12     // 浅棕蓝色(选择条目的反色)
// GPT4.0添加新的颜色定义，这些颜色值假定您的显示系统使用16位色深，并且采用RGB565格式进行颜色编码。在RGB565格式中，颜色值被编码为一个16位的数值，其中高5位表示红色分量，接下来的6位表示绿色分量，最低的5位表示蓝色分量。
#define ORANGE 0xFD20      // 橙色
#define PINK 0xF81F        // 粉红色
#define PURPLE 0x8010      // 紫色
#define TEAL 0x0410        // 青绿色
#define NAVY 0x000F        // 海军蓝
#define MAROON 0x7800      // 栗色
#define OLIVE 0x7BE0       // 橄榄绿
#define SILVER 0xC618      // 银色
#define SKYBLUE 0x867D     // 天蓝色
#define SLATEGRAY 0x7412   // 石板灰
#define CORAL 0xFBEA       // 珊瑚色
#define SALMON 0xFC0E      // 鲑鱼色
#define VIOLET 0xEC1D      // 紫罗兰色
#define DEEPPINK 0xF8B2    // 深粉色
#define CHOCOLATE 0xD344   // 巧克力色
#define TOMATO 0xFB08      // 番茄色
#define GOLD 0xFEA0        // 金色
#define ORANGERED 0xFA20   // 橙红色
#define DARKGREEN 0x0320   // 深绿色
#define DARKKHAKI 0xBDAD   // 深卡其布色
#define LIGHTCORAL 0xF410  // 浅珊瑚色
#define PEACHPUFF 0xFED7   // 桃色
#define PALEGREEN 0x9FD3   // 苍绿色
#define PLUM 0xDD1B        // 李子色
#define TURQUOISE 0x471A   // 绿松石色
#define ROYALBLUE 0x435C   // 皇家蓝
#define SPRINGGREEN 0x7EF5 // 春绿色
#define MINTCREAM 0xF7FF   // 薄荷奶油色
#define LAVENDER 0xE73F    // 薰衣草色

void LCD_Writ_Bus(u8 dat);                            // 模拟SPI时序
void LCD_WR_DATA8(u8 dat);                            // 写入一个字节
void LCD_WR_DATA(u16 dat);                            // 写入两个字节
void LCD_WR_REG(u8 dat);                              // 写入一个指令
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2); // 设置坐标函数
void LCD_ST7735S_Init(void);                          // 主控ST7735S的LCD初始化
void LCD_ST7735R_Init(void);                          // 主控ST7735R的LCD初始化
void LCD_ST7789_Init(void);
void LCD_ST7789V3_Init(void);

#endif
