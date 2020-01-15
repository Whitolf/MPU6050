//
// Created by Whitolf on 2019/12/15.
//

#ifndef MPU6050_OLED_H
#define MPU6050_OLED_H

#include <sched.h>

//说明:
//----------------------------------------------------------------
//GND    电源地
//VCC  接3.3v电源
//D0   接PA5  (SPI1_SCK===>LCD_SCK)   串行时钟线
//D1   接PA7  (SPI1_MOSI===>LCD_MOSI) 串行数据线
//RES  接PB0   硬复位 OLED
//DC   接PG15  命令/数据标志（写0—命令/写1—数据）；
//CS   接PA4   OLED片选信号

//根据数据手册提供对应的宏定义


#define SIZE 16
#define Max_Column        128
#define        OLED_CS_PORT OLED_CS_GPIO_Port
#define        OLED_CS_PIN OLED_CS_PIN

#define        OLED_DC_PORT OLED_DC_GPIO_Port
#define        OLED_DC_PIN OLED_DC_PIN

#define        OLED_RES_PORT OLED_RES_GPIO_Port
#define        OLED_RES_PIN OLED_RES_PIN

#define        OLED_SPI_PORT &hspi1

typedef struct
{
    const unsigned char name[4];
    const unsigned char dat[128];
}chinese;



void OLED_CS_Set();
void OLED_CS_Clr();

void OLED_DC_Set();
void OLED_DC_Clr();

void OLED_RST_Set();
void OLED_RST_Clr();

//OLED初始化


//OLED控制用函数
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t flag);
void OLED_ShowNum(uint8_t x,uint8_t y,u_int32_t num,uint8_t len);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t* str,u_int8_t font_height,u_int8_t font_width,uint8_t flag);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_ShowText(uint8_t x,uint8_t y,uint8_t* str,u_int8_t font_height,u_int8_t font_width,uint8_t flag);



#endif //MPU6050_OLED_H
