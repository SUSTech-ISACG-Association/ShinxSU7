#include "lcd.h"
#include "font.h"
// //////////////////////////////////////////////////////////////////////////////////
// // 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// // ALIENTEK MiniSTM32F103开发板
// // 2.4寸/2.8寸/3.5寸/4.3寸/7寸 TFT液晶驱动
// // 支持驱动IC型号包括:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
// //                   SPFD5408/1505/B505/C505/NT35310/NT35510/SSD1963等
// // 正点原子@ALIENTEK
// // 技术论坛:www.openedv.com
// // 创建日期:2010/7/4
// // 版本：V3.0
// // 版权所有，盗版必究。
// // Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// // All rights reserved
// //********************************************************************************
// // V1.2修改说明
// // 支持了SPFD5408的驱动,另外把液晶ID直接打印成HEX格式.方便查看LCD驱动IC.
// // V1.3
// // 加入了快速IO的支持
// // 修改了背光控制的极性（适用于V1.8及以后的开发板版本）
// // 对于1.8版本之前(不包括1.8)的液晶模块,请修改LCD_Init函数的LCD_LED=1;为LCD_LED=1;
// // V1.4
// // 修改了LCD_ShowChar函数 ，使用画点功能画字符。
// // 加入了横竖屏显示的支持
// // V1.5 20110730
// // 1,修改了B505液晶读颜色有误的bug.
// // 2,修改了快速IO及横竖屏的设置方式.
// // V1.6 20111116
// // 1,加入对LGDP4535液晶的驱动支持
// // V1.7 20120713
// // 1,增加LCD_RD_DATA函数
// // 2,增加对ILI9341的支持
// // 3,增加ILI9325的独立驱动代码
// // 4,增加LCD_Scan_Dir函数(慎重使用)
// // 6,另外修改了部分原来的函数,以适应9341的操作
// // V1.8 20120905
// // 1,加入LCD重要参数设置结构体lcddev
// // 2,加入LCD_Display_Dir函数,支持在线横竖屏切换
// // V1.9 20120911
// // 1,新增RM68042驱动（ID:6804），但是6804不支持横屏显示！！原因：改变扫描方式，
// // 导致6804坐标设置失效，试过很多方法都不行，暂时无解。
// // V2.0 20120924
// // 在不硬件复位的情况下,ILI9341的ID读取会被误读成9300,修改LCD_Init,将无法识别
// // 的情况（读到ID为9300/非法ID）,强制指定驱动IC为ILI9341，执行9341的初始化。
// // V2.1 20120930
// // 修正ILI9325读颜色的bug。
// // V2.2 20121007
// // 修正LCD_Scan_Dir的bug。
// // V2.3 20130120
// // 新增6804支持横屏显示
// // V2.4 20131120
// // 1,新增NT35310（ID:5310）驱动器的支持
// // 2,新增LCD_Set_Window函数,用于设置窗口,对快速填充,比较有用,但是该函数在横屏时,不支持6804.
// // V2.5 20140211
// // 1,新增NT35510（ID:5510）驱动器的支持
// // V2.6 20140504
// // 1,新增ASCII 24*24字体的支持(更多字体用户可以自行添加)
// // 2,修改部分函数参数,以支持MDK -O2优化
// // 3,针对9341/35310/35510,写时间设置为最快,尽可能的提高速度
// // 4,去掉了SSD1289的支持,因为1289实在是太慢了,读周期要1us...简直奇葩.不适合F4使用
// // 5,修正68042及C505等IC的读颜色函数的bug.
// // V2.7 20140710
// // 1,修正LCD_Color_Fill函数的一个bug.
// // 2,修正LCD_Scan_Dir函数的一个bug.
// // V2.8 20140721
// // 1,解决MDK使用-O2优化时LCD_ReadPoint函数读点失效的问题.
// // 2,修正LCD_Scan_Dir横屏时设置的扫描方式显示不全的bug.
// // V2.9 20141130
// // 1,新增对SSD1963 LCD的支持.
// // 2,新增LCD_SSD_BackLightSet函数
// // 3,取消ILI93XX的Rxx寄存器定义
// // V3.0 20150423
// // 修改SSD1963 LCD屏的驱动参数.
// //////////////////////////////////////////////////////////////////////////////////

uint16_t POINT_COLOR = 0x0000;
uint16_t BACK_COLOR = 0xFFFF; // Background color

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 写寄存器函数
// data:寄存器值
void LCD_WR_REG(uint16_t data)
{
    LCD_RS_CLR; // 写地址
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}
// 写数据函数
// 可以替代LCD_WR_DATAX宏,拿时间换空间.
// data:寄存器值
void LCD_WR_DATAX(uint16_t data)
{
    LCD_RS_SET;
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}
// 读LCD数据
// 返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
    uint16_t t;
    GPIOB->CRL = 0X88888888; // PB0-7  上拉输入
    GPIOB->CRH = 0X88888888; // PB8-15 上拉输入
    GPIOB->ODR = 0X0000;     // 全部输出0

    LCD_RS_SET;
    LCD_CS_CLR;
    // 读取数据(读寄存器时,并不需要读2次)
    LCD_RD_CLR;
    if (lcddev.id == 0X8989)
        HAL_Delay(1); // delay_us(2);//FOR 8989,延时2us
    t = DATAIN;
    LCD_RD_SET;
    LCD_CS_SET;

    GPIOB->CRL = 0X33333333; // PB0-7  上拉输出
    GPIOB->CRH = 0X33333333; // PB8-15 上拉输出
    GPIOB->ODR = 0XFFFF;     // 全部输出高
    return t;
}
// 写寄存器
// LCD_Reg:寄存器编号
// LCD_RegValue:要写入的值
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}
// 读寄存器
// LCD_Reg:寄存器编号
// 返回值:读到的值
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg); // 写入要读的寄存器号
    return LCD_RD_DATA();
}
// 开始写GRAM
void LCD_WriteRAM_Prepare(void) { LCD_WR_REG(lcddev.wramcmd); }
// LCD写GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD_WR_DATA(RGB_Code); // 写十六位GRAM
}
// 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
// 通过该函数转换
// c:GBR格式的颜色值
// 返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}
// 当mdk -O1时间优化时需要设置
// 延时i
void opt_delay(uint8_t i)
{
    while (i--)
        ;
}
// 读取个某点的颜色值
// x,y:坐标
// 返回值:此点的颜色
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint16_t r, g, b;
    if (x >= lcddev.width || y >= lcddev.height)
        return 0; // 超过了范围,直接返回
    LCD_SetCursor(x, y);
    LCD_WR_REG(0X2E);        // 其他IC发送读GRAM指令
    GPIOB->CRL = 0X88888888; // PB0-7  上拉输入
    GPIOB->CRH = 0X88888888; // PB8-15 上拉输入
    GPIOB->ODR = 0XFFFF;     // 全部输出高

    LCD_RS_SET;
    LCD_CS_CLR;
    // 读取数据(读GRAM时,第一次为假读)
    LCD_RD_CLR;
    opt_delay(2); // 延时
    r = DATAIN;   // 实际坐标颜色
    LCD_RD_SET;
    // dummy READ
    LCD_RD_CLR;
    opt_delay(2); // 延时
    r = DATAIN;   // 实际坐标颜色
    LCD_RD_SET;
    LCD_RD_CLR;
    opt_delay(2); // 延时
    b = DATAIN;   // 读取蓝色值
    LCD_RD_SET;
    g = r & 0XFF; // 对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
    g <<= 8;

    LCD_CS_SET;
    GPIOB->CRL = 0X33333333;                                   // PB0-7  上拉输出
    GPIOB->CRH = 0X33333333;                                   // PB8-15 上拉输出
    GPIOB->ODR = 0XFFFF;                                       // 全部输出高
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); // ILI9341/NT35310/NT35510需要公式转换一下
}
// LCD开启显示
void LCD_DisplayOn(void) { LCD_WR_REG(0X29); }
// LCD关闭显示
void LCD_DisplayOff(void) { LCD_WR_REG(0X28); }
// 设置光标位置
// Xpos:横坐标
// Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
}
// 设置LCD的自动扫描方向
// 注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
// 所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
// dir:0~7,代表8个方向(具体定义见lcd.h)
// 9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;
    if ((lcddev.dir == 1 && lcddev.id != 0X6804 && lcddev.id != 0X1963) ||
        (lcddev.dir == 0 && lcddev.id == 0X1963)) // 横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
    {
        switch (dir) // 方向转换
        {
        case 0:
            dir = 6;
            break;
        case 1:
            dir = 7;
            break;
        case 2:
            dir = 4;
            break;
        case 3:
            dir = 5;
            break;
        case 4:
            dir = 1;
            break;
        case 5:
            dir = 0;
            break;
        case 6:
            dir = 3;
            break;
        case 7:
            dir = 2;
            break;
        }
    }
    switch (dir) {
    case L2R_U2D: // 从左到右,从上到下
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;
    case L2R_D2U: // 从左到右,从下到上
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;
    case R2L_U2D: // 从右到左,从上到下
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;
    case R2L_D2U: // 从右到左,从下到上
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;
    case U2D_L2R: // 从上到下,从左到右
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;
    case U2D_R2L: // 从上到下,从右到左
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;
    case D2U_L2R: // 从下到上,从左到右
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;
    case D2U_R2L: // 从下到上,从右到左
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }
    dirreg = 0X36;
    regval |= 0X08;
    LCD_WriteReg(dirreg, regval);
    if (regval & 0X20) {
        if (lcddev.width < lcddev.height) // 交换X,Y
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else {
        if (lcddev.width > lcddev.height) // 交换X,Y
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}
// Draw a point with color
// x,y: coordinate

void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare();
    LCD_WR_DATA(POINT_COLOR); // POINT_COLOR: Color of this point
}
// Draw a point with color(quicker)
// x,y: coordinate
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(lcddev.wramcmd); // 写指令
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    LCD_WR_DATA(color); // 写数据
}

void LCD_Fast_DrawPoint_ck(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x >= lcddev.height || y < 0 || y >= lcddev.width)
        return;
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(lcddev.wramcmd); // 写指令
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    LCD_WR_DATA(color); // 写数据
}

// SSD1963 背光设置
// pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
    LCD_WR_REG(0xBE);        // 配置PWM输出
    LCD_WR_DATA(0x05);       // 1设置PWM频率
    LCD_WR_DATA(pwm * 2.55); // 2设置PWM占空比
    LCD_WR_DATA(0x01);       // 3设置C
    LCD_WR_DATA(0xFF);       // 4设置D
    LCD_WR_DATA(0x00);       // 5设置E
    LCD_WR_DATA(0x00);       // 6设置F
}
// Set LCD display direction
// dir: 0, vertical 1, horizontal
void LCD_Display_Dir(uint8_t dir)
{
    if (dir == 0) // 竖屏
    {
        lcddev.dir = 0; // 竖屏
        lcddev.width = 240;
        lcddev.height = 320;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    else // 横屏
    {
        lcddev.dir = 1; // 横屏
        lcddev.width = 320;
        lcddev.height = 240;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    LCD_Scan_Dir(DFT_SCAN_DIR); // 默认扫描方向
}
// 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
// sx,sy:窗口起始坐标(左上角)
// width,height:窗口宽度和高度,必须大于0!!
// 窗体大小:width*height.
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);
}
// 初始化lcd
// 该初始化函数可以初始化各种ALIENTEK出品的LCD液晶屏
// 本函数占用较大flash,用户可以根据自己的实际情况,删掉未用到的LCD初始化代码.以节省空间.
void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE(); // 开启GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE(); // 开启GPIOC时钟

    // PC6,7,8,9,10
    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    // PB0~15
    GPIO_Initure.Pin = GPIO_PIN_All; // PB推挽输出
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    //	__HAL_AFIO_REMAP_SWJ_DISABLE();				//禁止JTAG

    HAL_Delay(50); // delay 50 ms
    LCD_WriteReg(0x0000, 0x0001);
    HAL_Delay(50); // delay 50 ms
    lcddev.id = LCD_ReadReg(0x0000);
    if (lcddev.id < 0XFF || lcddev.id == 0XFFFF ||
        lcddev.id == 0X9300) // 读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
    {
        // 尝试9341 ID的读取
        LCD_WR_REG(0XD3);
        LCD_RD_DATA();             // dummy read
        LCD_RD_DATA();             // 读到0X00
        lcddev.id = LCD_RD_DATA(); // 读取93
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA(); // 读取41
        if (lcddev.id != 0X9341)    // 非9341,尝试是不是6804
        {
        }
    }
    // 	printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID
    LCD_WR_REG(0xCF);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0xC1);
    LCD_WR_DATAX(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATAX(0x64);
    LCD_WR_DATAX(0x03);
    LCD_WR_DATAX(0X12);
    LCD_WR_DATAX(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATAX(0x85);
    LCD_WR_DATAX(0x10);
    LCD_WR_DATAX(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATAX(0x39);
    LCD_WR_DATAX(0x2C);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x34);
    LCD_WR_DATAX(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATAX(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0xC0);   // Power control
    LCD_WR_DATAX(0x1B); // VRH[5:0]
    LCD_WR_REG(0xC1);   // Power control
    LCD_WR_DATAX(0x01); // SAP[2:0];BT[3:0]
    LCD_WR_REG(0xC5);   // VCM control
    LCD_WR_DATAX(0x30); // 3F
    LCD_WR_DATAX(0x30); // 3C
    LCD_WR_REG(0xC7);   // VCM control2
    LCD_WR_DATAX(0XB7);
    LCD_WR_REG(0x36); // Memory Access Control
    LCD_WR_DATAX(0x48);
    LCD_WR_REG(0x3A);
    LCD_WR_DATAX(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x1A);
    LCD_WR_REG(0xB6); // Display Function Control
    LCD_WR_DATAX(0x0A);
    LCD_WR_DATAX(0xA2);
    LCD_WR_REG(0xF2); // 3Gamma Function Disable
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0x26); // Gamma curve selected
    LCD_WR_DATAX(0x01);
    LCD_WR_REG(0xE0); // Set Gamma
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x2A);
    LCD_WR_DATAX(0x28);
    LCD_WR_DATAX(0x08);
    LCD_WR_DATAX(0x0E);
    LCD_WR_DATAX(0x08);
    LCD_WR_DATAX(0x54);
    LCD_WR_DATAX(0XA9);
    LCD_WR_DATAX(0x43);
    LCD_WR_DATAX(0x0A);
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0XE1); // Set Gamma
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x15);
    LCD_WR_DATAX(0x17);
    LCD_WR_DATAX(0x07);
    LCD_WR_DATAX(0x11);
    LCD_WR_DATAX(0x06);
    LCD_WR_DATAX(0x2B);
    LCD_WR_DATAX(0x56);
    LCD_WR_DATAX(0x3C);
    LCD_WR_DATAX(0x05);
    LCD_WR_DATAX(0x10);
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x3F);
    LCD_WR_DATAX(0x3F);
    LCD_WR_DATAX(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x01);
    LCD_WR_DATAX(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0xef);
    LCD_WR_REG(0x11); // Exit Sleep
    HAL_Delay(120);
    LCD_WR_REG(0x29); // display on

    LCD_Display_Dir(0); // 默认为竖屏
    LCD_LED = 1;        // 点亮背光
    LCD_Clear(WHITE);
}

// Clear the screen with specific color
void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height; // 得到总点数
    LCD_SetCursor(0x00, 0x0000); // 设置光标位置
    LCD_WriteRAM_Prepare();      // 开始写入GRAM
    for (index = 0; index < totalpoint; index++)
        LCD_WR_DATA(color);
}
// Fill the area with color
// :(xend-xsta+1)*(yend-ysta+1)
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++) {
        LCD_SetCursor(sx, i);   // 设置光标位置
        LCD_WriteRAM_Prepare(); // 开始写入GRAM
        for (j = 0; j < xlen; j++)
            LCD_WR_DATA(color); // 设置光标位置
    }
}
// Fill the area with color
// :(xend-xsta+1)*(yend-ysta+1)
void LCD_Fill_Window(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    if (sx >= ex || sy >= ey) return;
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;
    LCD_Set_Window(sx, sy, ex-sx+1, ey-sy+1);
    LCD_SetCursor(sx, sy);  // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    for (i = sy; i <= ey; i++) {
        for (j = 0; j < xlen; j++)
            LCD_WR_DATA(color); // 设置光标位置
    }
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}
// Fill the area with color
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;
    width = ex - sx + 1;  // 得到填充的宽度
    height = ey - sy + 1; // 高度
    for (i = 0; i < height; i++) {
        LCD_SetCursor(sx, sy + i); // 设置光标位置
        LCD_WriteRAM_Prepare();    // 开始写入GRAM
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[i * width + j]); // 写入数据
    }
} // Fill the area with color
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
void LCD_myColor_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color, int scale)
{
    uint16_t height, width, width_tt;
    uint16_t i, j;
    width_tt = ex - sx + 1;
    width = width_tt * scale;       // 得到填充的宽度
    height = (ey - sy + 1) * scale; // 高度
    for (i = 0; i < height; i++) {
        uint16_t idv = (i / scale) * width_tt;
        LCD_SetCursor(sx, sy + i); // 设置光标位置
        LCD_WriteRAM_Prepare();    // 开始写入GRAM
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[idv + (j / scale)]); // 写入数据
    }
}
void LCD_myColor_Fill_Window(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color, int scale)
{
    uint16_t height, width, width_tt;
    uint16_t i, j;
    width_tt = ex - sx + 1;
    width = width_tt * scale;       // 得到填充的宽度
    height = (ey - sy + 1) * scale; // 高度
    LCD_Set_Window(sx, sy, width, height);
    LCD_SetCursor(sx, sy);  // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    for (i = 0; i < height; i++) {
        uint16_t idv = (i / scale) * width_tt;
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[idv + (j / scale)]); // 写入数据
    }
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}
// Draw a line
// x1,y1: coordinate of starting point
// x2,y2: coordinate of terminal point
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // 计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1; // 设置单步方向
    else if (delta_x == 0)
        incx = 0; // 垂直线
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // 水平线
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; // 选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++) // 画线输出
    {
        LCD_DrawPoint(uRow, uCol); // 画点
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}
// Draw a rectangle
//(x1,y1),(x2,y2): coordinates of the upper left point and bottom right point of a rectangle
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

// Draw a circle
//(x,y): center
// r    : radius
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1); // 判断下个点位置的标志
    while (a <= b) {
        LCD_DrawPoint(x0 + a, y0 - b); // 5
        LCD_DrawPoint(x0 + b, y0 - a); // 0
        LCD_DrawPoint(x0 + b, y0 + a); // 4
        LCD_DrawPoint(x0 + a, y0 + b); // 6
        LCD_DrawPoint(x0 - a, y0 + b); // 1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b); // 2
        LCD_DrawPoint(x0 - b, y0 - a); // 7
        a++;
        // 使用Bresenham算法画圆
        if (di < 0)
            di += 4 * a + 6;
        else {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
// Display a char
// x,y: starting coordinate
// num: char to be displayed
// size: font size 12/16/24
// mode: display characters on the displayed content and do not wipe away the displayed content(1)
//       display characters on the displayed content and wipe away the displayed content(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数
    num = num - ' '; // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for (t = 0; t < csize; t++) {
        if (size == 12)
            temp = asc2_1206[num][t]; // 调用1206字体
        else if (size == 16)
            temp = asc2_1608[num][t]; // 调用1608字体
        else if (size == 24)
            temp = asc2_2412[num][t]; // 调用2412字体
        else
            return; // 没有的字库
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if (y >= lcddev.height)
                return; // 超区域了
            if ((y - y0) == size) {
                y = y0;
                x++;
                if (x >= lcddev.width)
                    return; // 超区域了
                break;
            }
        }
    }
}
// m^n函数
// 返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

// Display number without the leading zeros
// x,y: starting coordinate
// len: number of digits
// size: font size 12/16/24
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}

// Display number with the leading zeros
// x,y: starting coordinate
// len: number of digits
// size: font size 12/16/24
// mode:
//[7]:0 without leading zeros; 1 with leading zeros.
//[6:1]: reserve
//[0]: similar as it in LCD_ShowChar, 0 for wipe away and 1 for opposite
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                if (mode & 0X80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}
// Display a string
// x,y: starting coordinate
// width, height: display area size
// size: font size // x,y: starting coordinate
// *p: string
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if (x >= width) {
            x = x0;
            y += size;
        }
        if (y >= height)
            break; // 退出
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
// Display a string
// x,y: starting coordinate
// width, height: display area size
// size: font size // x,y: starting coordinate
// *p: string
void LCD_ShowString_trans(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if (x >= width) {
            x = x0;
            y += size;
        }
        if (y >= height)
            break; // 退出
        LCD_ShowChar(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

void LCD_myColor_Fill_trans(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint16_t *color,
                            const uint16_t *mask, int scale)
{
    uint16_t height = ey - sy + 1, width = ex - sx + 1, i, j, k, l, idv, idx;
    int16_t dpx = sx, dpy = sy;

    if (scale == 1) {
        for (i = 0; i < height; ++i) {
            idv = i * width;
            for (j = 0; j < width; ++j) {
                idx = idv + j;
                if ((mask[idx >> 4]) & (1 << (idx & 15))) {
                    LCD_Fast_DrawPoint_ck(dpy, dpx, color[idx]);
                }
                dpx += scale;
                if (dpx >= lcddev.width){
                    break;
                }
            }
            dpx = sx;
            dpy += scale;
            if (dpy >= lcddev.height){
                break;
            }
        }
        return;
    }
    else if (scale == 2) {
        for (i = 0; i < height; ++i) {
            idv = i * width;
            for (j = 0; j < width; ++j) {
                idx = idv + j;
                if ((mask[idx >> 4]) & (1 << (idx & 15))) {
                    LCD_Fast_DrawPoint_ck(dpy, dpx, color[idx]);
                    LCD_Fast_DrawPoint_ck(dpy + 1, dpx, color[idx]);
                    LCD_Fast_DrawPoint_ck(dpy, dpx + 1, color[idx]);
                    LCD_Fast_DrawPoint_ck(dpy + 1, dpx + 1, color[idx]);
                }
                dpx += scale;
                if (dpx >= lcddev.width){
                    break;
                }
            }
            dpx = sx;
            dpy += scale;
            if (dpy >= lcddev.width){
                break;
            }
        }
        return;
    }
    else {
        for (i = 0; i < height; ++i) {
            idv = i * width;
            for (j = 0; j < width; ++j) {
                idx = idv + j;
                if ((mask[idx >> 4]) & (1 << (idx & 15))) {
                    for (k = dpy; k < dpy + scale; ++k) {
                        for (l = dpx; l < dpx + scale; ++l) {
                            LCD_Fast_DrawPoint_ck(k, l, color[idx]);
                        }
                    }
                }
                dpx += scale;
                if (dpx >= lcddev.width){
                    break;
                }
            }
            dpx = sx;
            dpy += scale;
            if (dpy >= lcddev.width){
                break;
            }
        }
        return;
    }
}