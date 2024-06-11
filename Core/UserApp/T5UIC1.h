#ifndef __T5UIC1_H
#define __T5UIC1_H	

#include "main.h"
#include "stdlib.h"
#include "string.h"	
#include "math.h"



//屏幕矩形区域移动宏定义
/***************************************************************************************************************
        移动方向： 0x00=环移向左。  0x01=环移向右。  0x02=环移向上。  0x03=环移向下。
                   0x80=平移向左。  0x81=平移向右。  0x82=平移向上。  0x83=平移向下。  
***************************************************************************************************************/
#define ROL       0x00      //环移向左
#define ROR       0x01      //环移向右
#define ROU       0x02      //环移向上
#define ROD       0x03      //环移向下

#define PTL       0x80      //平移向左
#define PTR       0x81      //平移向右
#define PTU       0x82      //平移向上
#define PTD       0x83      //平移向下

//画笔颜色
#define White         	 0xFFFF
#define Black         	 0x0000	  
#define Blue         	 0x001F  
#define BRed             0XF81F
#define GRed 			       0XFFE0
#define GBlue			       0X07FF
#define Red           	 0xF800
#define Magenta       	 0xF81F
#define Green         	 0x07E0
#define CYAN          	 0x7FFF
#define Yellow        	 0xFFE0
#define BROWN 	    		 0XBC40 //棕色
#define BRRED 		    	 0XFC07 //棕红色
#define GRAY  		    	 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色 
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)




//字符/汉字字号大小
#define    A12   0x00        //字符字号12*06汉字字号12*12
#define    A16   0x01	       //字符字号16*08汉字字号16*16
#define    A20   0x02        //字符字号20*10汉字字号20*20
#define    A24   0x03	       //字符字号24*12汉字字号24*24
#define    A28   0x04        //字符字号28*14汉字字号28*28
#define    A32   0x05        //字符字号32*16汉字字号32*32
#define    A40   0x06        //字符字号40*20汉字字号40*40
#define    A48   0x07        //字符字号48*24汉字字号48*48
#define    A56   0x08        //字符字号56*28汉字字号56*56
#define    A64   0x09        //字符字号64*32汉字字号64*64


void Data_init(void);                          //协议帧 帧头帧尾初始化	    															  
void DWINLCD_Init(USART_TypeDef* USARTx);		   //屏幕握手初始化
void Send_Data(uint8_t *buff);                 //协议指令发送

void DWIN_LightSet(uint8_t Light);                  //背光亮度调节
void DWINLCD_Clear(uint16_t Color);	 							 //全局清屏
void DWINLCD_DrawPoint(uint16_t Color,uint8_t Nx,uint8_t Ny,uint16_t X1,uint16_t Y1);		               //画点
void DWINLCD_DrawLine(uint16_t Color,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);               //画线
void DWINLCD_DrawRectangle(uint8_t Mode,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye);  //画矩形 /矩形填充/局部清屏
void DWINLCD_Area_Movement(uint8_t Mode,uint16_t DIS,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye);	    //矩形区域移动
void DWINLCD_ShowChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str);        //显示一个字符/汉字	
void DWINLCD_ShowXChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str);       //显示多个字符						      
void DWINLCD_ShowNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs,uint16_t Ys,uint8_t Num);		              //显示一个数字
void DWINLCD_ShowXNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint8_t Num_I,uint8_t Num_F,uint16_t Xs,uint16_t Ys,uint32_t XNum);	//显示多位数字
void DWINLCD_QR_Code(uint8_t QR_Pixel,uint16_t Xs,uint16_t Ys,char*str);	    //显示二维码
void DWINLCD_JPEG_Display(uint16_t JPEG_ID);	                      //显示图片
void DWINLCD_Display_Dir(uint8_t Dis_CFG);								          //设置屏幕显示方向
void DWINLCD_ICO_Display(uint16_t Xs, uint16_t Ys,uint8_t IBD,uint8_t BIR,uint8_t BFI,uint8_t ISI,uint8_t Icon_IDs);	   //显示图标
void DWINLCD_ICOplay(uint16_t Xs, uint16_t Ys,uint8_t SC,uint8_t LM,uint8_t IconID,uint8_t Icon_lib,uint8_t Icon_IDs,uint8_t Icon_IDe,uint8_t Delay_time);//ICO图标动画play
void DWINLCD_Cartoon_Set(uint8_t Cartoon_Set,uint8_t on_off);     //ICO图标动画自动显示指令控制


/****************************本API为扩展指令，非手册内指令函数*****************************************/
void DWINLCD_Draw_Circle(uint16_t Color,uint16_t x0,uint16_t y0,uint8_t r);		  //画圆
void DWINLCD_CircleFill(uint16_t FColor,uint16_t x0,uint16_t y0,uint8_t r);			//圆型填充

						  		 
#endif

