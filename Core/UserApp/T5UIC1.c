#include "T5UIC1.h"
//初始化lcd

int DwinSend_Buff_Size=5;
uint8_t *DWINSend_Buff;
__IO  uint16_t Send_p = 0;
USART_TypeDef* Dwin_UART;

//帧头帧尾初始化
void Data_init()
{
	 
   DWINSend_Buff=(uint8_t *)malloc(DwinSend_Buff_Size);
 	
   /***********************************************************/		 
   DWINSend_Buff[DwinSend_Buff_Size -DwinSend_Buff_Size ]=0xAA;     //帧头//
   /***********************************************************/		
	 
	 /***********************************************************/	 
     DWINSend_Buff[DwinSend_Buff_Size -4]=0xCC;                     //帧//
	 DWINSend_Buff[DwinSend_Buff_Size -3]=0x33;                     //结//
	 DWINSend_Buff[DwinSend_Buff_Size -2]=0xC3;                     //束//
	 DWINSend_Buff[DwinSend_Buff_Size -1]=0x3C;                     //符//
	 /***********************************************************/	 
}
//迪文串口屏初始化
//传参：串口号
//例子:DWINLCD_Init(USART1);      //迪文屏采用串口1发送
void DWINLCD_Init(USART_TypeDef* USARTx)
{ 
	 DwinSend_Buff_Size=6; 
	 Data_init();                                   //更新数组     		
     Dwin_UART=USARTx;
	
	 DWINSend_Buff[DwinSend_Buff_Size -5]=0x00;     //握手指令
	 
	 Send_Data(DWINSend_Buff);                      //发送数据
	
	/**********判断回复消息，确认是否握手成功************/
}  		  
  
//发送指令数据 
void Send_Data(uint8_t *buff)
{
	 uint8_t i=DwinSend_Buff_Size;
	 Send_p = 0;
	 while(i--)
	 {
		 while((Dwin_UART->SR&0X40)==0);  //循环发送,直到发送完毕
		 Dwin_UART->DR = (DWINSend_Buff[Send_p++]&(uint16_t)0x01FF);    		 
	 } 
	 free(DWINSend_Buff);                 //释放内存     
}

//迪文屏背光亮度调节
//背光亮度值范围：0x00-0xFF
/*注意：0x00 背光关闭，0xFF 背光最亮，其中 0x01-0x1F 设置值背光可能会闪烁。*/
void DWIN_LightSet(uint8_t Light)
{
	
	DwinSend_Buff_Size=7;                         //指令数据长度为7（帧头+指令+数据+帧尾）
	Data_init();                                  //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=0x30;    //背光指令
	
	DWINSend_Buff[DwinSend_Buff_Size -5]=Light;   //背光值
	
	Send_Data(DWINSend_Buff);                     //发送数据
}

//迪文屏全局清屏
//
void DWINLCD_Clear(uint16_t Color)	 					  //全局清屏
{
	DwinSend_Buff_Size=8;                         //指令数据长度为8（帧头+指令+数据+帧尾）
	Data_init();                                  //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x01;    //清屏指令
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=Color>>8;       //清屏颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -5]=Color&0x00FF;   //清屏颜色低8位
	
	Send_Data(DWINSend_Buff);                            //发送数据
//   Delay_ms(10);
  HAL_Delay(10);
}

//___________________________________画点 ____________________________________________//
//指令：帧头+指令+画点颜色+画点像素大小(Nx,Ny)+画点位置【(X1,Y1)+(X2,Y2)+.........】+帧尾
//置点；处理时间=0.4*Nx*Ny*置点数目 uS。
//Color：置点颜色。
//Nx：实际像素点 X 方向像素大小，0x01-0x0F。
//Ny：实际像素点 Y 方向像素大小，0x01-0x0F。
//（Xn，Yn）：置点坐标序列。
//举例：AA 02 F8 00 04 04 00 08 00 08  CC 33 C3 3C
/**************画点协议指令可以一次画多点（本函数只画一个位置的像素点）********************/
void DWINLCD_DrawPoint(uint16_t Color,uint8_t Nx,uint8_t Ny,uint16_t X1,uint16_t Y1)			  //画点
{
	DwinSend_Buff_Size=14;                         //指令数据长度为14（帧头+指令+数据+帧尾）
	Data_init();                                   //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=0x02;    //画点指令
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Color>>8;          //画点颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -11]=Color&0x00FF;      //画点颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=Nx;                //Nx：实际像素点 X 方向像素大小，0x01-0x0F
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ny;                 //Ny：实际像素点 Y 方向像素大小，0x01-0x0F
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=X1>>8;              //X1置点X坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=X1&0xFF;            //X1置点X坐标低八位
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=Y1>>8;              //Y1置点Y坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=Y1&0xFF;            //Y1置点Y坐标低八位
	
	Send_Data(DWINSend_Buff);                                //发送数据
}
//_________________________________画线________________________________________//

//指令：帧头+指令+画线颜色+画点起点位置(X1,Y1)+画点起点位置(X2,Y2)+帧尾
//Color：画线颜色
//起始坐标：（x1,y1）
//终点坐标：（x2,y2）
void DWINLCD_DrawLine(uint16_t Color,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)					//画线
{
	DwinSend_Buff_Size=16;                                    //指令数据长度为16（帧头+指令+数据+帧尾）
	Data_init();                                              //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -15]=0x03;               //画线指令
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //画线颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0x00FF;       //画线颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=x1>>8;              //X1坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -11]=x1&0xFF;            //X1坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -10]=y1>>8;              //Y1坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -9]=y1&0xFF;             //Y1坐标低八位
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=x2>>8;               //X2坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=x2&0xFF;             //X2坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -6]=y2>>8;               //Y2坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=y2&0xFF;             //Y2坐标低八位
	
	Send_Data(DWINSend_Buff);                                 //发送数据
}

//___________________________________画矩形框____________________________________________

//指令：帧头+指令+模式+画线颜色+矩形框左上角起点坐标(Xs,Ys)+矩形框右下角坐标(Xe,Ye)+帧尾
//模式:  (0)0x00 显示矩形框颜色
//       (1)0x01 显示矩形区域填充颜色
//       (2)0x02 XOR 矩形区域数据，多用于菜单选中/不选中着色
//Color：矩形框颜色/矩形框填充颜色
//(Xs,Ys)：矩形框左上角起点坐标
//(Xe,Ye)：矩形框右下角终点坐标
void DWINLCD_DrawRectangle(uint8_t Mode,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye)		   				//画矩形
{
	DwinSend_Buff_Size=17;                                    //指令数据长度为16（帧头+指令+数据+帧尾）
	Data_init();                                              //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=0x05;               //画矩形指令
	DWINSend_Buff[DwinSend_Buff_Size -15]=Mode;               //模式选择
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //画线颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0x00FF;       //画线颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Xs>>8;              //Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -11]=Xs&0xFF;            //Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys>>8;              //Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ys&0xFF;             //Ys坐标低八位
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xe>>8;               //Xe坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=Xe&0xFF;             //Xe坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ye>>8;               //Ye坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=Ye&0xFF;             //Ye坐标低八位
	
	Send_Data(DWINSend_Buff);                                 //发送数据
}
//____________________________________屏幕区域移动_________________________________________//
//指令：帧头+指令+模式+移动方向+填充颜色+矩形框左上角起点坐标(Xs,Ys)+矩形框右下角坐标(Xe,Ye)+帧尾
/****************************************************************************************************
Mode：    ROL=环移向左。  ROR=环移向右。  ROU=环移向上。  ROD=环移向下。
          PTL=平移向左。  PTR=平移向右。  PTU=平移向上。  PTD=平移向下。   
****************************************************************************************************/
//DIS：  移动距离，像素点阵数目，0x0000-水平分辨率/2，2Bytes。
//Color：填充颜色，仅当平移时有效。
//（Xs，Ys）：选定区域的左上角坐标。
//（Xe，Ye）：选定区域的右下角坐标。
void DWINLCD_Area_Movement(uint8_t Mode,uint16_t DIS,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye)	//矩形区域移动
{
	DwinSend_Buff_Size=19;                                    //指令数据长度为19（帧头+指令+Mode+数据+帧尾）
	Data_init();                                              //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -18]=0x09;               //矩形区域移动指令
	DWINSend_Buff[DwinSend_Buff_Size -17]=Mode;               //矩形区域移动模式指令   
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=DIS>>8;             //矩形区域移动距离高八位  
	DWINSend_Buff[DwinSend_Buff_Size -15]=DIS&0xFF;           //矩形区域移动距离低八位
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //填充颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0xFF;         //填充颜色低8位
	
     DWINSend_Buff[DwinSend_Buff_Size -12]=Xs>>8;              //Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -11]=Xs&0xFF;            //Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys>>8;              //Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ys&0xFF;             //Ys坐标低八位
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xe>>8;               //Xe坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=Xe&0xFF;             //Xe坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ye>>8;               //Ye坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=Ye&0xFF;             //Ye坐标低八位
	
	Send_Data(DWINSend_Buff);                                 //发送数据
}
//____________________________显示一个字符/汉字________________________________//
//指令:    帧头+指令+模式+字符颜色+字符背景颜色+字符左上角坐标(Xs,Ys)+字符+帧尾
//CWA:     字符宽度调整(CWA)设置     1=调整 0=不调整。
//BCD:     背景颜色显示(BCD)设置     1=显示 0=不显示。
//Size:    字符字号大小
//Color：  字符显示颜色。
//Bcolor： 字符背景显示颜色。
//(Xs,Ys)：文本显示位置左上角坐标
//Strings：要显示的字符。
/******************非 ASCII 字符按照 GB2312 编码格式汉字显示******************/

void DWINLCD_ShowChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str)						      
{
	uint16_t strlenth=0;
	DwinSend_Buff_Size=17;                                    //指令数据长度为17（帧头+指令+Mode+数据+帧尾）
	Data_init();                                              //重新更新数组
	
	strlenth=strlen((const char*)str);
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=0x11;               //显示字符指令
	DWINSend_Buff[DwinSend_Buff_Size -15]=CWA%2<<7|BCD%2<<6|Size; //显示字符模式指令   
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //字符颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0xFF;         //字符颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Bcolor>>8;          //填充字符背景颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -11]=Bcolor&0xFF;        //填充字符背景颜色低8位
	
    DWINSend_Buff[DwinSend_Buff_Size -10]=Xs>>8;            //字符Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs&0xFF;             //字符Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -8]=Ys>>8;               //字符Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys&0xFF;             //字符Ys坐标低八位
	

	DWINSend_Buff[DwinSend_Buff_Size -6]=*str;                //字符内码/汉字高八位
	while((strlenth>1)&(*str>0x7E))                               //判断是否为汉字
		{
	    DWINSend_Buff[DwinSend_Buff_Size -5]=*(str+1); break; //汉字低八位
		}
	
	Send_Data(DWINSend_Buff);                                 //发送数据
	
	
}

//____________________________显示一个字符/汉字________________________________//

//指令：   帧头+指令+模式+字符颜色+字符背景颜色+字符左上角坐标(Xs,Ys)+字符+帧尾
//CWA:     字符宽度调整(CWA)设置     1=调整 0=不调整。
//BCD:     背景颜色显示(BCD)设置     1=显示 0=不显示。
//Size:    字符字号大小
//Color：  字符显示颜色。
//Bcolor： 字符背景显示颜色。
//(Xs,Ys)：文本显示位置左上角坐标
//Strings：要显示的字符。
/******************非 ASCII 字符按照 GB2312 编码格式汉字显示******************/
void DWINLCD_ShowXChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str)						      
{
	uint16_t strlenth;
	int i;
	strlenth=strlen((const char*)str);
	DwinSend_Buff_Size=15+strlenth;                           //指令数据长度为15+strlenth（帧头+指令+Mode+数据+帧尾）
	
	Data_init();                                              //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -14-strlenth]=0x11;                    //显示字符指令
	DWINSend_Buff[DwinSend_Buff_Size -13-strlenth]=CWA%2<<7|BCD%2<<6|Size;  //显示字符模式指令   
	
	DWINSend_Buff[DwinSend_Buff_Size -12-strlenth]=Color>>8;           //字符颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -11-strlenth]=Color&0xFF;         //字符颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=Bcolor>>8;          //填充字符背景颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Bcolor&0xFF;         //填充字符背景颜色低8位
	
    DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs>>8;               //字符Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Xs&0xFF;             //字符Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys>>8;               //字符Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=Ys&0xFF;             //字符Ys坐标低八位
	

	for(i=strlenth;i>0;i--)
	{		DWINSend_Buff[DwinSend_Buff_Size -4-i]=*(str++); }             //字符Ys坐标低八位
	
	Send_Data(DWINSend_Buff);                                 //发送数据
	
	
}

//____________________________显示1个数字________________________________//
//BCD:     背景色显示设置 1=显示 0=不显示。
//SymNum：  1=带符号数 0=无符号数。
//Dis0:     1= 无效0显示          0= 无效0不显示。
//NDis:     1= 无效0显示为0       0= 无效0显示为空格。
//Size：    字号大小，0x00-0x09，同 0x11 指令。
//Color：   字符显示颜色。
//Bcolor：  字符背景显示颜色。
//（x，y）: 变量显示的左上角坐标。
//Datas：1位数据
void DWINLCD_ShowNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs,uint16_t Ys,uint8_t Num)						           //显示一个数字
{	
	DwinSend_Buff_Size=18;                                    //指令数据长度为17（帧头+指令+Mode+数据+帧尾）
	Data_init();                                              //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -17]=0x14;               //显示数字指令
	DWINSend_Buff[DwinSend_Buff_Size -16]=BCD%2<<7|SymNum%2<<6|Dis0%2<<5|NDis%2<<4|Size;       //显示数字模式指令   
	
	DWINSend_Buff[DwinSend_Buff_Size -15]=Color>>8;           //数字颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color&0xFF;         //数字颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Bcolor>>8;          //填充字符背景颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Bcolor&0xFF;        //填充字符背景颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -11]=0x01;               //显示的整数位数。
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=0x00;               //显示的小数位数。
	
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs>>8;               //数字Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xs&0xFF;             //数字Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys>>8;               //数字Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ys&0xFF;             //数字Ys坐标低八位

	DWINSend_Buff[DwinSend_Buff_Size -5]=Num;
	
	Send_Data(DWINSend_Buff);                                 //发送数据
	
}

//____________________________显示多个数字________________________________//
//BCD:  背景颜色显示开关     1=显示       0=不显示
//SymNum:     有无符号数     1=有符号数   0=无符号数
//Dis0:       1= 无效0显示          0= 无效0不显示。
//NDis:       1= 无效0显示为0       0= 无效0显示为空格。
//Size:       字符字号大小         12#/16#/20#/24#/28#/32#/40#/48#/56#/64#
//Color：     字符显示颜色         
//Bcolor：    字符背景显示颜色
//Num_I:      显示的整数位数，   1-20
//Num_F:      显示的小数位数，   0-20
//(x，y):     变量显示的左上角坐标
//Datas：     多位数据             最多 8 字节
/*******************Num_I+Num_F 之和不能超过 20****************************/
void DWINLCD_ShowXNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint8_t Num_I,uint8_t Num_F,uint16_t Xs,uint16_t Ys,uint32_t XNum)	//显示多位数字
{
	
	uint8_t strlenth=4;
	DwinSend_Buff_Size=21;                                          //指令数据长度为17（帧头+指令+Mode+数据+帧尾）
	Data_init();                                                    //重新更新数组
	
	DWINSend_Buff[DwinSend_Buff_Size -16-strlenth]=0x14;               //显示数字指令
		
	DWINSend_Buff[DwinSend_Buff_Size -15-strlenth]=BCD%2<<7|SymNum%2<<6|Dis0%2<<5|NDis%2<<4|Size;      //显示数字模式指令   
	
	
	DWINSend_Buff[DwinSend_Buff_Size -14-strlenth]=Color>>8;           //数字颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -13-strlenth]=Color&0xFF;         //数字颜色低8位
	
	DWINSend_Buff[DwinSend_Buff_Size -12-strlenth]=Bcolor>>8;          //填充字符背景颜色高8位	
	DWINSend_Buff[DwinSend_Buff_Size -11-strlenth]=Bcolor&0xFF;        //填充字符背景颜色低8位
	
	if(Num_I<=20&Num_F<=20&Num_I+Num_F<=20)                            //Num_I+Num_F 之和不能超过 20
	{
		DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=Num_I;            //显示的整数位数，0x01-0x14(1-20)。
	  DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Num_F;             //显示的小数位数，0x00-0x14(1-20)。
	}
	else
	{
		DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=1;                //显示的整数位数超出规定默认显示1位
	  DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=0;                 //显示的小数位数超出规定默认不显示
	}
	DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs>>8;               //数字Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Xs&0xFF;             //数字Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys>>8;               //数字Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=Ys&0xFF;             //数字Ys坐标低八位

	DWINSend_Buff[DwinSend_Buff_Size -strlenth-4]= XNum>>24;           //数字高
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-3]= XNum>>16&0xff;      //数字高
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-2]= XNum>>8&0xff;       //数字低
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-1]= XNum&0xFF;          //数字低
	
	Send_Data(DWINSend_Buff);                                          //发送数据
}

//____________________________显示二维码________________________________//
//QR_Pixel:  二维码每个点占用像素点大小：0x01-0x0F（1-16）
//(Nx，Ny):  二维码显示的左上角坐标
//str：      多位数据 
/**************二维码大小为（46*QR_Pixel）*（46*QR_Pixle）点阵************/
void DWINLCD_QR_Code(uint8_t QR_Pixel,uint16_t Xs,uint16_t Ys,char*str)	    //显示二维码
{
	uint16_t strlenth;
	int i;
	strlenth=strlen((const char*)str);
	DwinSend_Buff_Size=11+strlenth;                       //指令数据长度为15+strlenth（帧头+指令+Mode+数据+帧尾）
	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=0x21;               //显示二维码指令
 
	DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Xs>>8;               //二维码Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs&0xFF;             //二维码Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Ys>>8;               //二维码Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys&0xFF;             //二维码Ys坐标低八位
	
if(QR_Pixel<=6)                                                      //根据实际屏幕大小设置上限像素点 
  DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=QR_Pixel;            //二维码像素点大小
 else
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=0x01;               //二维码像素点大小超出默认为1
 
	for(i=strlenth;i>0;i--)
	{		DWINSend_Buff[DwinSend_Buff_Size -4-i]=*(str++); }             //二维码内容显示	

	Send_Data(DWINSend_Buff);                                          //发送数据
	
}

//____________________________显示图片________________________________//

/****JPEG 图片显示；480*272 分辨率 4:1:1 格式压缩处理时间为 250mS。****/
//JPEG_ID：   0x00-0x0F，对应 JPEG 存储的图片起始 ID。
void DWINLCD_JPEG_Display(uint16_t JPEG_ID)	                //显示图片
{
	
	DwinSend_Buff_Size=8;                                 //指令数据长度为8（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x22;            //显示图片指令
	
  DWINSend_Buff[DwinSend_Buff_Size -6]=JPEG_ID>>8;      //
	DWINSend_Buff[DwinSend_Buff_Size -5]=JPEG_ID&0xFF;

	Send_Data(DWINSend_Buff);                             //发送数据
	Delay_ms(300);                                        //等待屏幕处理时间
}

//____________________________显示方向调整________________________________//
//Dis_CFG ：  0x00=0   度，   不旋转。       0x01=90  度旋转。
//            0x02=180 度， 视角翻转。       0x03=270 度旋转。
void DWINLCD_Display_Dir(uint8_t Dis_CFG)								    //设置屏幕显示方向
{
		
	DwinSend_Buff_Size=9;                                 //指令数据长度为9（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -8]=0x34;            //屏幕显示方向调整指令
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x5A;            
	DWINSend_Buff[DwinSend_Buff_Size -6]=0xA5;            
	
  DWINSend_Buff[DwinSend_Buff_Size -5]=Dis_CFG%4;       //屏幕显示方向调整指令
	
	Send_Data(DWINSend_Buff);                             //发送数据
}

//____________________________设置扩展串口波特率________________________________

/********上电默认值是 0x0088 ，对应 115200bps 波特率。***************************/

void  DWINLCDBode_Set(uint16_t Set)                          //调节扩展串口波特率
{
	
			
	DwinSend_Buff_Size=8;                                 //指令数据长度为8（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x38;            //设置扩展串口波特率指令
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=15667200/Set/256;            
	DWINSend_Buff[DwinSend_Buff_Size -5]=15667200/Set%256;            
	
	Send_Data(DWINSend_Buff);                             //发送数据
}

//____________________________画圆________________________________
//Color: 圆形颜色
//x0:    圆形圆心横坐标
//y0:    圆形圆心纵坐标
//r:     圆形半径

void DWINLCD_Draw_Circle(uint16_t Color,uint16_t x0,uint16_t y0,uint8_t r)			   //画圆
{
	int a,b;
	a=0;	  
	while(a<=b)
	{
		b=sqrt(r*r-a*a);
		while(a==0){ b=b-1;break;}
		DWINLCD_DrawPoint(Color,1,1,x0+a,y0+b);		               //画点扇区1	
		DWINLCD_DrawPoint(Color,1,1,x0+b,y0+a);		               //画点扇区2
		DWINLCD_DrawPoint(Color,1,1,x0+b,y0-a);		               //画点扇区3	
		DWINLCD_DrawPoint(Color,1,1,x0+a,y0-b);		               //画点扇区4
			
		DWINLCD_DrawPoint(Color,1,1,x0-a,y0-b);		              //画点扇区5			
		DWINLCD_DrawPoint(Color,1,1,x0-b,y0-a);		              //画点扇区6
		DWINLCD_DrawPoint(Color,1,1,x0-b,y0+a);		              //画点扇区7	
		DWINLCD_DrawPoint(Color,1,1,x0-a,y0+b);		              //画点扇区8	
		a++;   					    
	}
}


//____________________________圆型填充________________________________
//FColor: 圆形填充颜色
//x0:     圆形圆心横坐标
//y0:     圆形圆心纵坐标
//r:      圆形半径

void DWINLCD_CircleFill(uint16_t FColor,uint16_t x0,uint16_t y0,uint8_t r)			   //圆型填充
{
	int a,b;

for(;r>0;r--)  
	{  a=0;	
		while(a<=b)
		{
			b=sqrt(r*r-a*a);
			while(a==0){ b=b-1;break;}
			DWINLCD_DrawPoint(FColor,2,2,x0+a,y0+b);		               //画点扇区1	
			DWINLCD_DrawPoint(FColor,2,2,x0+b,y0+a);		               //画点扇区2
			DWINLCD_DrawPoint(FColor,2,2,x0+b,y0-a);		               //画点扇区3	
			DWINLCD_DrawPoint(FColor,2,2,x0+a,y0-b);		               //画点扇区4
				
			DWINLCD_DrawPoint(FColor,2,2,x0-a,y0-b);		               //画点扇区5			
			DWINLCD_DrawPoint(FColor,2,2,x0-b,y0-a);		               //画点扇区6
			DWINLCD_DrawPoint(FColor,2,2,x0-b,y0+a);		               //画点扇区7	
			DWINLCD_DrawPoint(FColor,2,2,x0-a,y0+b);		               //画点扇区8	
			a=a+2;   					    
		}
  }
}
//____________________________显示ICO图标________________________________
//（Xs，Ys）: 图标显示的左上角坐标。
//图标背景显示IBD: 0=背景滤除不显示， 1=背景显示        \\设置背景滤除不显示时，背景必须为纯黑色//
//背景图片恢复BIR: 0=背景图片不恢复 ，1=自动使用        \\0#虚拟显示区图片做背景恢复//
//背景滤除强度BFI: 0=普通，1=增强                       \\（仅当图标背景显示=0时有效）//
//图标库存储位置ISI: 0X00-0X0F
//Icon_IDs：  需要显示的图标ID，1个ID用1个Byte表示， 0x00-0xFF
void DWINLCD_ICO_Display(uint16_t Xs, uint16_t Ys,uint8_t IBD,uint8_t BIR,uint8_t BFI,uint8_t ISI,uint8_t Icon_IDs)	     //显示图标
{
	
	DwinSend_Buff_Size=12;                                //指令数据长度为8（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -11]=0x23;            //显示图标令
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=Xs>>8;               //二维码Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs&0xFF;             //二维码Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -8]=Ys>>8;               //二维码Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys&0xFF;             //二维码Ys坐标低八位
  	DWINSend_Buff[DwinSend_Buff_Size -6]=IBD%2<<7|BIR%2<<6|BFI%2<<5|ISI;      //二维码Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=Icon_IDs;

	Send_Data(DWINSend_Buff);                             //发送数据
}

//____________________________ICO图标动画play________________________________
//（Xs，Ys）: 图标显示的左上角坐标。
//开关控制SC：1=本组动画开启          0=本组动画关闭；         可由 0x29 指令控制
//启动模式LM：1=启动从起始图标开始    0=启动从上次停止位置开始
//本组动画是第几组动画：本组动画图标指令位置，0x00-0x0F，一共有 16 组动画指令。
//Icon_lib：  图标库存储位置，0x00-0x0F。
//Icon_IDs：  动画起始图标位置，0x00-0xFF。
//Icon_IDe：  动画终止图标位置，0x00-0xFF。
//Delay_time：动画图标显示时间间隔，0x00-0xFF，单位 10mS。
void DWINLCD_ICOplay(uint16_t Xs, uint16_t Ys,uint8_t SC,uint8_t LM,uint8_t IconID,uint8_t Icon_lib,uint8_t Icon_IDs,uint8_t Icon_IDe,uint8_t Delay_time)	     //ICO图标动画play
{
	
	DwinSend_Buff_Size=15;                                //指令数据长度为8（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                          //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -14]=0x28;            //显示图标令
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Xs>>8;              //ICO_Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -12]=Xs&0xFF;             //ICO_Xs坐标低八位
	DWINSend_Buff[DwinSend_Buff_Size -11]=Ys>>8;               //ICO_Ys坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys&0xFF;             //ICO_Ys坐标低八位
	
  DWINSend_Buff[DwinSend_Buff_Size -9]=SC%2<<7|LM%2<<6|IconID;      //二维码Xs坐标高八位
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Icon_lib;
	DWINSend_Buff[DwinSend_Buff_Size -7]=Icon_IDs;
  DWINSend_Buff[DwinSend_Buff_Size -6]=Icon_IDe;
	
	 DWINSend_Buff[DwinSend_Buff_Size -5]=Delay_time;
	
	Send_Data(DWINSend_Buff);                             //发送数据
}
//____________________________ICO图标动画自动显示指令控制________________________________
//Cartoon_Set：ICON 动画指令开关控制；
//on_off:  每个 bit 对应一组指令，1=开启on，0=关闭off；
//.15 对应第 15 组动画指令， .0 对应第 0 组动画指令。
//举例：AA 29 00 05 CC 33 C3 3C 开启第 0 组、第 2 组动画指令。
void DWINLCD_Cartoon_Set(uint8_t Cartoon_Set,uint8_t on_off)	     //ICO图标动画play
{
	
	DwinSend_Buff_Size=8;                                //指令数据长度为8（帧头+指令+Mode+数据+帧尾）	
	Data_init();                                         //重新更新数组
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x29;            //ICO图标动画自动显示指令控制
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=(on_off<<(Cartoon_Set-1))>>8;           //ICO_Xs坐标高八位
	DWINSend_Buff[DwinSend_Buff_Size -5]=(on_off<<(Cartoon_Set-1))&0xFF;         //ICO_Xs坐标低八位
  	
	Send_Data(DWINSend_Buff);                             //发送数据
}
