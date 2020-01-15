//
// Created by Whitolf on 2019/12/15.
//

#include "oled.h"
#include "oledfont.h"
#include "spi.h"

uint8_t OLED_GRAM[128][8];

void OLED_CS_Set() { HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_Pin, GPIO_PIN_SET); }
void OLED_CS_Clr() { HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_Pin, GPIO_PIN_RESET); }
void OLED_DC_Set() { HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_Pin, GPIO_PIN_SET); }
void OLED_DC_Clr() { HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_Pin, GPIO_PIN_RESET); }
void OLED_RST_Set() { HAL_GPIO_WritePin(OLED_RES_PORT, OLED_RES_Pin, GPIO_PIN_SET); }
void OLED_RST_Clr() { HAL_GPIO_WritePin(OLED_RES_PORT, OLED_RES_Pin, GPIO_PIN_RESET); }


/*********************写一位数据到SPI*******************/
void OLED_WB(uint8_t data) {
    HAL_SPI_Transmit(OLED_SPI_PORT, &data, 1, 10);
    //HAL_Delay(1);
}

/*******************一个字节数据写入***********************/
void OLED_WrDat(uint8_t dat) {
    OLED_DC_Set();
    OLED_WB(dat);
}

/********************一条指令写入**********************/
void OLED_WrCmd(uint8_t cmd) {
    OLED_DC_Clr();
    OLED_WB(cmd);
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(((x&0xf0)>>4)|0x10);
    OLED_WrCmd((x&0x0f)|0x01);
}

//开启OLED显示   
void OLED_Display_On(void)
{
    OLED_WrCmd(0X8D);  //SET DCDC命令
    OLED_WrCmd(0X14);  //DCDC ON
    OLED_WrCmd(0XAF);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WrCmd(0X8D);  //SET DCDC命令
    OLED_WrCmd(0X10);  //DCDC OFF
    OLED_WrCmd(0XAE);  //DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!         
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WrCmd(0xb0+i);    //设置页地址（0~7）
        OLED_WrCmd(0x02);      //设置显示位置—列低地址
        OLED_WrCmd(0x10);      //设置显示位置—列高地址
        for(n=0;n<128;n++)OLED_WrDat(0);
    } //更新显示
}

/********************************************************************************
* @函数名：         OLED_ShowChar
* @函数描述：       在指定位置显示占宽8*16的单个ASCII字符
* @函数作者：       矛盾聚合体
* @输入参数：
                    参数名    参数类型  参数描述
                    @x：      uint8_t         列坐标，0~127
                    @y：      uint8_t         行坐标，0~7
                    @chr：    uint8_t         要显示的ASCII字符
                    @flag：   uint8_t         反白标志，非0时反白显示
* @返回值：         void
* @其他：
********************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t flag)
{
    unsigned char c=0,i=0;
    c=chr-' ';//得到偏移后的值
    if(x>Max_Column-1){x=0;y=y+2;}
    OLED_Set_Pos(x,y);
    for(i=0;i<8;i++)
    {
        if(flag==1)OLED_WrDat(~F8X16[c*16+i]);
        else OLED_WrDat(F8X16[c*16+i]);
    }

    OLED_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
    {
        if(flag==1)OLED_WrDat(~F8X16[c*16+i+8]);
        else OLED_WrDat(F8X16[c*16+i+8]);
    }
}

//m^n函数
u_int32_t oled_pow(uint8_t m,uint8_t n)
{
    u_int32_t result=1;
    while(n--)result*=m;
    return result;
}

/********************************************************************************
* @函数名：         OLED_ShowNum
* @函数描述：       显示数字
* @函数作者：       矛盾聚合体
* @输入参数：
                  参数名     参数类型         参数描述
                  @x：      uint8_t         列坐标，0~127
                  @y：      uint8_t         行坐标，0~7
                  @num：    uint8_t         要显示的数字
                  @led：    uint8_t         要显示的数字长度
* @返回值：        void
* @其他：
********************************************************************************/
void OLED_ShowNum(uint8_t x,uint8_t y,u_int32_t num,uint8_t len)
{
    uint8_t t,temp,size=16;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ',0);
                continue;
            }else enshow=1;

        }
        OLED_ShowChar(x+(size/2)*t,y,temp+'0',0);
    }
}

//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {
        OLED_ShowChar(x,y,chr[j],0);
        x+=8;
        if(x>120){x=0;y+=2;}
        j++;
    }
}

/********************************************************************************
* @函数名：         OLED_ShowCHineseWord
* @函数描述：       在指定位置显示占宽16*16的单个汉字
* @函数作者：       矛盾聚合体
* @输入参数：
                    参数名            参数类型          参数描述
                    @x：              uint8_t         列坐标，0~127
                    @y：              uint8_t         行坐标，0~7
                    @str：            uint8_t*        要显示的汉字
                    @font_height:     uint8_t         单个字符串高度
                    @font_width:      uint8_t         单个字符串宽度
                    @flag：           uint8_t         反白标志，非0时反白显示
* @返回值：         void
* @其他：           
********************************************************************************/
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t* str,u_int8_t font_height,u_int8_t font_width,uint8_t flag)
{
    u_int32_t index;
    u_int32_t t, i;
    for(index=0; index<sizeof(Hzk)/132; index++)
    {
        if((Hzk[index].name[0] == str[0])&&(Hzk[index].name[1] == str[1])&&(Hzk[index].name[2] == str[2]))//对比汉字区码位码
        {
            ;
            for(i=0;i<(font_height/8);i++)				/*font_height最大值为32，这张屏只有8个页（行），每页4个位*/
            {
                OLED_Set_Pos(x,y+i);
                for(t=0;t<font_width;t++)		/*font_width最大值为128，屏幕只有这么大*/
                {
                    if(flag==0)OLED_WrDat(Hzk[index].dat[i*font_height+t]);
                    else OLED_WrDat(~Hzk[index].dat[i*font_height+t]);
                }
            }
        }
    }
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

    if(y1%8==0) y=y1/8;
    else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        OLED_Set_Pos(x0,y);
        for(x=x0;x<x1;x++)
        {
            OLED_WrDat(BMP[j++]);
        }
    }
}

/********************************************************************************
* @函数名：         OLED_ShowText
* @函数描述：       在指定位置显示中英文字符串
* @函数作者：       矛盾聚合体
* @输入参数：
                    参数名            参数类型          参数描述
                    @x：              uint8_t         列坐标，0~127
                    @y：              uint8_t         行坐标，0~7
                    @str：            uint8_t*        要显示的字符串
                    @font_height:     uint8_t         单个字符串高度
                    @font_width:      uint8_t         单个字符串宽度
                    @flag：           uint8_t         反白标志，非0时反白显示
* @返回值：         void
* @其他：           
********************************************************************************/
void OLED_ShowText(uint8_t x,uint8_t y,uint8_t* str,u_int8_t font_height,u_int8_t font_width,uint8_t flag)
{
    uint8_t tempstr[4] = {'\0'};
    while(*str!='\0')
    {
        if(*str&0x80)
        {
            tempstr[0]=*str++;
            tempstr[1]=*str++;
            tempstr[2]=*str++;
            OLED_ShowCHinese(x,y,tempstr,font_height,font_width,flag);
            x+=font_width;
            if(*str&0x80)//判断下一个字符是中文还是英文
            {
                if(x>=112){y++;y++;x=0;}//修改地址 
            }
            else
            {
                if(x>=120){y++;y++;x=0;}//修改地址
            }

        }
        else
        {
            OLED_ShowChar(x,y,*str++,flag);
            x+=8;
            if(*str&0x80)
            {
                if(x>=112){y++;y++;x=0;}//修改地址 
            }
            else
            {
                if(x>=120){y++;y++;x=0;}//修改地址
            }
        }
    }
}


//初始化SSD1306                                            
void OLED_Init(void)
{
    OLED_RST_Set();
    HAL_Delay(100);
    OLED_RST_Clr();
    HAL_Delay(100);
    OLED_RST_Set();

    OLED_WrCmd(0xAE);//--turn off oled panel
    OLED_WrCmd(0x02);//---set low column address
    OLED_WrCmd(0x10);//---set high column address
    OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WrCmd(0x81);//--set contrast control register
    OLED_WrCmd(0xCF); // Set SEG Output Current Brightness
    OLED_WrCmd(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WrCmd(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WrCmd(0xA6);//--set normal display
    OLED_WrCmd(0xA8);//--set multiplex ratio(1 to 64)
    OLED_WrCmd(0x3f);//--1/64 duty
    OLED_WrCmd(0xD3);//-set display offset        Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WrCmd(0x00);//-not offset
    OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WrCmd(0xD9);//--set pre-charge period
    OLED_WrCmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WrCmd(0xDA);//--set com pins hardware configuration
    OLED_WrCmd(0x12);
    OLED_WrCmd(0xDB);//--set vcomh
    OLED_WrCmd(0x40);//Set VCOM Deselect Level
    OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WrCmd(0x02);//
    OLED_WrCmd(0x8D);//--set Charge Pump enable/disable
    OLED_WrCmd(0x14);//--set(0x10) disable
    OLED_WrCmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
    OLED_WrCmd(0xA6);// Disable Inverse Display On (0xa6/a7)
    OLED_WrCmd(0xAF);//--turn on oled panel

    OLED_WrCmd(0xAF); /*display ON*/
    OLED_Clear();
    OLED_Set_Pos(0,0);
} 