#include "T5UIC1.h"
//��ʼ��lcd

int DwinSend_Buff_Size=5;
uint8_t *DWINSend_Buff;
__IO  uint16_t Send_p = 0;
USART_TypeDef* Dwin_UART;

//֡ͷ֡β��ʼ��
void Data_init()
{
	 
   DWINSend_Buff=(uint8_t *)malloc(DwinSend_Buff_Size);
 	
   /***********************************************************/		 
   DWINSend_Buff[DwinSend_Buff_Size -DwinSend_Buff_Size ]=0xAA;     //֡ͷ//
   /***********************************************************/		
	 
	 /***********************************************************/	 
     DWINSend_Buff[DwinSend_Buff_Size -4]=0xCC;                     //֡//
	 DWINSend_Buff[DwinSend_Buff_Size -3]=0x33;                     //��//
	 DWINSend_Buff[DwinSend_Buff_Size -2]=0xC3;                     //��//
	 DWINSend_Buff[DwinSend_Buff_Size -1]=0x3C;                     //��//
	 /***********************************************************/	 
}
//���Ĵ�������ʼ��
//���Σ����ں�
//����:DWINLCD_Init(USART1);      //���������ô���1����
void DWINLCD_Init(USART_TypeDef* USARTx)
{ 
	 DwinSend_Buff_Size=6; 
	 Data_init();                                   //��������     		
     Dwin_UART=USARTx;
	
	 DWINSend_Buff[DwinSend_Buff_Size -5]=0x00;     //����ָ��
	 
	 Send_Data(DWINSend_Buff);                      //��������
	
	/**********�жϻظ���Ϣ��ȷ���Ƿ����ֳɹ�************/
}  		  
  
//����ָ������ 
void Send_Data(uint8_t *buff)
{
	 uint8_t i=DwinSend_Buff_Size;
	 Send_p = 0;
	 while(i--)
	 {
		 while((Dwin_UART->SR&0X40)==0);  //ѭ������,ֱ���������
		 Dwin_UART->DR = (DWINSend_Buff[Send_p++]&(uint16_t)0x01FF);    		 
	 } 
	 free(DWINSend_Buff);                 //�ͷ��ڴ�     
}

//�������������ȵ���
//��������ֵ��Χ��0x00-0xFF
/*ע�⣺0x00 ����رգ�0xFF �������������� 0x01-0x1F ����ֵ������ܻ���˸��*/
void DWIN_LightSet(uint8_t Light)
{
	
	DwinSend_Buff_Size=7;                         //ָ�����ݳ���Ϊ7��֡ͷ+ָ��+����+֡β��
	Data_init();                                  //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=0x30;    //����ָ��
	
	DWINSend_Buff[DwinSend_Buff_Size -5]=Light;   //����ֵ
	
	Send_Data(DWINSend_Buff);                     //��������
}

//������ȫ������
//
void DWINLCD_Clear(uint16_t Color)	 					  //ȫ������
{
	DwinSend_Buff_Size=8;                         //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+����+֡β��
	Data_init();                                  //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x01;    //����ָ��
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=Color>>8;       //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -5]=Color&0x00FF;   //������ɫ��8λ
	
	Send_Data(DWINSend_Buff);                            //��������
//   Delay_ms(10);
  HAL_Delay(10);
}

//___________________________________���� ____________________________________________//
//ָ�֡ͷ+ָ��+������ɫ+�������ش�С(Nx,Ny)+����λ�á�(X1,Y1)+(X2,Y2)+.........��+֡β
//�õ㣻����ʱ��=0.4*Nx*Ny*�õ���Ŀ uS��
//Color���õ���ɫ��
//Nx��ʵ�����ص� X �������ش�С��0x01-0x0F��
//Ny��ʵ�����ص� Y �������ش�С��0x01-0x0F��
//��Xn��Yn�����õ��������С�
//������AA 02 F8 00 04 04 00 08 00 08  CC 33 C3 3C
/**************����Э��ָ�����һ�λ���㣨������ֻ��һ��λ�õ����ص㣩********************/
void DWINLCD_DrawPoint(uint16_t Color,uint8_t Nx,uint8_t Ny,uint16_t X1,uint16_t Y1)			  //����
{
	DwinSend_Buff_Size=14;                         //ָ�����ݳ���Ϊ14��֡ͷ+ָ��+����+֡β��
	Data_init();                                   //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=0x02;    //����ָ��
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Color>>8;          //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -11]=Color&0x00FF;      //������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=Nx;                //Nx��ʵ�����ص� X �������ش�С��0x01-0x0F
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ny;                 //Ny��ʵ�����ص� Y �������ش�С��0x01-0x0F
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=X1>>8;              //X1�õ�X����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=X1&0xFF;            //X1�õ�X����Ͱ�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=Y1>>8;              //Y1�õ�Y����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=Y1&0xFF;            //Y1�õ�Y����Ͱ�λ
	
	Send_Data(DWINSend_Buff);                                //��������
}
//_________________________________����________________________________________//

//ָ�֡ͷ+ָ��+������ɫ+�������λ��(X1,Y1)+�������λ��(X2,Y2)+֡β
//Color��������ɫ
//��ʼ���꣺��x1,y1��
//�յ����꣺��x2,y2��
void DWINLCD_DrawLine(uint16_t Color,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)					//����
{
	DwinSend_Buff_Size=16;                                    //ָ�����ݳ���Ϊ16��֡ͷ+ָ��+����+֡β��
	Data_init();                                              //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -15]=0x03;               //����ָ��
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0x00FF;       //������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=x1>>8;              //X1����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -11]=x1&0xFF;            //X1����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -10]=y1>>8;              //Y1����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -9]=y1&0xFF;             //Y1����Ͱ�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=x2>>8;               //X2����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=x2&0xFF;             //X2����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -6]=y2>>8;               //Y2����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=y2&0xFF;             //Y2����Ͱ�λ
	
	Send_Data(DWINSend_Buff);                                 //��������
}

//___________________________________�����ο�____________________________________________

//ָ�֡ͷ+ָ��+ģʽ+������ɫ+���ο����Ͻ��������(Xs,Ys)+���ο����½�����(Xe,Ye)+֡β
//ģʽ:  (0)0x00 ��ʾ���ο���ɫ
//       (1)0x01 ��ʾ�������������ɫ
//       (2)0x02 XOR �����������ݣ������ڲ˵�ѡ��/��ѡ����ɫ
//Color�����ο���ɫ/���ο������ɫ
//(Xs,Ys)�����ο����Ͻ��������
//(Xe,Ye)�����ο����½��յ�����
void DWINLCD_DrawRectangle(uint8_t Mode,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye)		   				//������
{
	DwinSend_Buff_Size=17;                                    //ָ�����ݳ���Ϊ16��֡ͷ+ָ��+����+֡β��
	Data_init();                                              //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=0x05;               //������ָ��
	DWINSend_Buff[DwinSend_Buff_Size -15]=Mode;               //ģʽѡ��
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0x00FF;       //������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Xs>>8;              //Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -11]=Xs&0xFF;            //Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys>>8;              //Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ys&0xFF;             //Ys����Ͱ�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xe>>8;               //Xe����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=Xe&0xFF;             //Xe����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ye>>8;               //Ye����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=Ye&0xFF;             //Ye����Ͱ�λ
	
	Send_Data(DWINSend_Buff);                                 //��������
}
//____________________________________��Ļ�����ƶ�_________________________________________//
//ָ�֡ͷ+ָ��+ģʽ+�ƶ�����+�����ɫ+���ο����Ͻ��������(Xs,Ys)+���ο����½�����(Xe,Ye)+֡β
/****************************************************************************************************
Mode��    ROL=��������  ROR=�������ҡ�  ROU=�������ϡ�  ROD=�������¡�
          PTL=ƽ������  PTR=ƽ�����ҡ�  PTU=ƽ�����ϡ�  PTD=ƽ�����¡�   
****************************************************************************************************/
//DIS��  �ƶ����룬���ص�����Ŀ��0x0000-ˮƽ�ֱ���/2��2Bytes��
//Color�������ɫ������ƽ��ʱ��Ч��
//��Xs��Ys����ѡ����������Ͻ����ꡣ
//��Xe��Ye����ѡ����������½����ꡣ
void DWINLCD_Area_Movement(uint8_t Mode,uint16_t DIS,uint16_t Color,uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye)	//���������ƶ�
{
	DwinSend_Buff_Size=19;                                    //ָ�����ݳ���Ϊ19��֡ͷ+ָ��+Mode+����+֡β��
	Data_init();                                              //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -18]=0x09;               //���������ƶ�ָ��
	DWINSend_Buff[DwinSend_Buff_Size -17]=Mode;               //���������ƶ�ģʽָ��   
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=DIS>>8;             //���������ƶ�����߰�λ  
	DWINSend_Buff[DwinSend_Buff_Size -15]=DIS&0xFF;           //���������ƶ�����Ͱ�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //�����ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0xFF;         //�����ɫ��8λ
	
     DWINSend_Buff[DwinSend_Buff_Size -12]=Xs>>8;              //Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -11]=Xs&0xFF;            //Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys>>8;              //Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -9]=Ys&0xFF;             //Ys����Ͱ�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xe>>8;               //Xe����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=Xe&0xFF;             //Xe����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ye>>8;               //Ye����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=Ye&0xFF;             //Ye����Ͱ�λ
	
	Send_Data(DWINSend_Buff);                                 //��������
}
//____________________________��ʾһ���ַ�/����________________________________//
//ָ��:    ֡ͷ+ָ��+ģʽ+�ַ���ɫ+�ַ�������ɫ+�ַ����Ͻ�����(Xs,Ys)+�ַ�+֡β
//CWA:     �ַ���ȵ���(CWA)����     1=���� 0=��������
//BCD:     ������ɫ��ʾ(BCD)����     1=��ʾ 0=����ʾ��
//Size:    �ַ��ֺŴ�С
//Color��  �ַ���ʾ��ɫ��
//Bcolor�� �ַ�������ʾ��ɫ��
//(Xs,Ys)���ı���ʾλ�����Ͻ�����
//Strings��Ҫ��ʾ���ַ���
/******************�� ASCII �ַ����� GB2312 �����ʽ������ʾ******************/

void DWINLCD_ShowChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str)						      
{
	uint16_t strlenth=0;
	DwinSend_Buff_Size=17;                                    //ָ�����ݳ���Ϊ17��֡ͷ+ָ��+Mode+����+֡β��
	Data_init();                                              //���¸�������
	
	strlenth=strlen((const char*)str);
	
	DWINSend_Buff[DwinSend_Buff_Size -16]=0x11;               //��ʾ�ַ�ָ��
	DWINSend_Buff[DwinSend_Buff_Size -15]=CWA%2<<7|BCD%2<<6|Size; //��ʾ�ַ�ģʽָ��   
	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color>>8;           //�ַ���ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Color&0xFF;         //�ַ���ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Bcolor>>8;          //����ַ�������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -11]=Bcolor&0xFF;        //����ַ�������ɫ��8λ
	
    DWINSend_Buff[DwinSend_Buff_Size -10]=Xs>>8;            //�ַ�Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs&0xFF;             //�ַ�Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -8]=Ys>>8;               //�ַ�Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys&0xFF;             //�ַ�Ys����Ͱ�λ
	

	DWINSend_Buff[DwinSend_Buff_Size -6]=*str;                //�ַ�����/���ָ߰�λ
	while((strlenth>1)&(*str>0x7E))                               //�ж��Ƿ�Ϊ����
		{
	    DWINSend_Buff[DwinSend_Buff_Size -5]=*(str+1); break; //���ֵͰ�λ
		}
	
	Send_Data(DWINSend_Buff);                                 //��������
	
	
}

//____________________________��ʾһ���ַ�/����________________________________//

//ָ�   ֡ͷ+ָ��+ģʽ+�ַ���ɫ+�ַ�������ɫ+�ַ����Ͻ�����(Xs,Ys)+�ַ�+֡β
//CWA:     �ַ���ȵ���(CWA)����     1=���� 0=��������
//BCD:     ������ɫ��ʾ(BCD)����     1=��ʾ 0=����ʾ��
//Size:    �ַ��ֺŴ�С
//Color��  �ַ���ʾ��ɫ��
//Bcolor�� �ַ�������ʾ��ɫ��
//(Xs,Ys)���ı���ʾλ�����Ͻ�����
//Strings��Ҫ��ʾ���ַ���
/******************�� ASCII �ַ����� GB2312 �����ʽ������ʾ******************/
void DWINLCD_ShowXChar(uint8_t CWA,uint8_t BCD,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs, uint16_t Ys,char*str)						      
{
	uint16_t strlenth;
	int i;
	strlenth=strlen((const char*)str);
	DwinSend_Buff_Size=15+strlenth;                           //ָ�����ݳ���Ϊ15+strlenth��֡ͷ+ָ��+Mode+����+֡β��
	
	Data_init();                                              //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -14-strlenth]=0x11;                    //��ʾ�ַ�ָ��
	DWINSend_Buff[DwinSend_Buff_Size -13-strlenth]=CWA%2<<7|BCD%2<<6|Size;  //��ʾ�ַ�ģʽָ��   
	
	DWINSend_Buff[DwinSend_Buff_Size -12-strlenth]=Color>>8;           //�ַ���ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -11-strlenth]=Color&0xFF;         //�ַ���ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=Bcolor>>8;          //����ַ�������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Bcolor&0xFF;         //����ַ�������ɫ��8λ
	
    DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs>>8;               //�ַ�Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Xs&0xFF;             //�ַ�Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys>>8;               //�ַ�Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=Ys&0xFF;             //�ַ�Ys����Ͱ�λ
	

	for(i=strlenth;i>0;i--)
	{		DWINSend_Buff[DwinSend_Buff_Size -4-i]=*(str++); }             //�ַ�Ys����Ͱ�λ
	
	Send_Data(DWINSend_Buff);                                 //��������
	
	
}

//____________________________��ʾ1������________________________________//
//BCD:     ����ɫ��ʾ���� 1=��ʾ 0=����ʾ��
//SymNum��  1=�������� 0=�޷�������
//Dis0:     1= ��Ч0��ʾ          0= ��Ч0����ʾ��
//NDis:     1= ��Ч0��ʾΪ0       0= ��Ч0��ʾΪ�ո�
//Size��    �ֺŴ�С��0x00-0x09��ͬ 0x11 ָ�
//Color��   �ַ���ʾ��ɫ��
//Bcolor��  �ַ�������ʾ��ɫ��
//��x��y��: ������ʾ�����Ͻ����ꡣ
//Datas��1λ����
void DWINLCD_ShowNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint16_t Xs,uint16_t Ys,uint8_t Num)						           //��ʾһ������
{	
	DwinSend_Buff_Size=18;                                    //ָ�����ݳ���Ϊ17��֡ͷ+ָ��+Mode+����+֡β��
	Data_init();                                              //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -17]=0x14;               //��ʾ����ָ��
	DWINSend_Buff[DwinSend_Buff_Size -16]=BCD%2<<7|SymNum%2<<6|Dis0%2<<5|NDis%2<<4|Size;       //��ʾ����ģʽָ��   
	
	DWINSend_Buff[DwinSend_Buff_Size -15]=Color>>8;           //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -14]=Color&0xFF;         //������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Bcolor>>8;          //����ַ�������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -12]=Bcolor&0xFF;        //����ַ�������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -11]=0x01;               //��ʾ������λ����
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=0x00;               //��ʾ��С��λ����
	
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs>>8;               //����Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -8]=Xs&0xFF;             //����Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys>>8;               //����Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -6]=Ys&0xFF;             //����Ys����Ͱ�λ

	DWINSend_Buff[DwinSend_Buff_Size -5]=Num;
	
	Send_Data(DWINSend_Buff);                                 //��������
	
}

//____________________________��ʾ�������________________________________//
//BCD:  ������ɫ��ʾ����     1=��ʾ       0=����ʾ
//SymNum:     ���޷�����     1=�з�����   0=�޷�����
//Dis0:       1= ��Ч0��ʾ          0= ��Ч0����ʾ��
//NDis:       1= ��Ч0��ʾΪ0       0= ��Ч0��ʾΪ�ո�
//Size:       �ַ��ֺŴ�С         12#/16#/20#/24#/28#/32#/40#/48#/56#/64#
//Color��     �ַ���ʾ��ɫ         
//Bcolor��    �ַ�������ʾ��ɫ
//Num_I:      ��ʾ������λ����   1-20
//Num_F:      ��ʾ��С��λ����   0-20
//(x��y):     ������ʾ�����Ͻ�����
//Datas��     ��λ����             ��� 8 �ֽ�
/*******************Num_I+Num_F ֮�Ͳ��ܳ��� 20****************************/
void DWINLCD_ShowXNum(uint8_t BCD,uint8_t SymNum,uint8_t Dis0,uint8_t NDis,uint8_t Size,uint16_t Color, uint16_t Bcolor,uint8_t Num_I,uint8_t Num_F,uint16_t Xs,uint16_t Ys,uint32_t XNum)	//��ʾ��λ����
{
	
	uint8_t strlenth=4;
	DwinSend_Buff_Size=21;                                          //ָ�����ݳ���Ϊ17��֡ͷ+ָ��+Mode+����+֡β��
	Data_init();                                                    //���¸�������
	
	DWINSend_Buff[DwinSend_Buff_Size -16-strlenth]=0x14;               //��ʾ����ָ��
		
	DWINSend_Buff[DwinSend_Buff_Size -15-strlenth]=BCD%2<<7|SymNum%2<<6|Dis0%2<<5|NDis%2<<4|Size;      //��ʾ����ģʽָ��   
	
	
	DWINSend_Buff[DwinSend_Buff_Size -14-strlenth]=Color>>8;           //������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -13-strlenth]=Color&0xFF;         //������ɫ��8λ
	
	DWINSend_Buff[DwinSend_Buff_Size -12-strlenth]=Bcolor>>8;          //����ַ�������ɫ��8λ	
	DWINSend_Buff[DwinSend_Buff_Size -11-strlenth]=Bcolor&0xFF;        //����ַ�������ɫ��8λ
	
	if(Num_I<=20&Num_F<=20&Num_I+Num_F<=20)                            //Num_I+Num_F ֮�Ͳ��ܳ��� 20
	{
		DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=Num_I;            //��ʾ������λ����0x01-0x14(1-20)��
	  DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Num_F;             //��ʾ��С��λ����0x00-0x14(1-20)��
	}
	else
	{
		DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=1;                //��ʾ������λ�������涨Ĭ����ʾ1λ
	  DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=0;                 //��ʾ��С��λ�������涨Ĭ�ϲ���ʾ
	}
	DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs>>8;               //����Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Xs&0xFF;             //����Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys>>8;               //����Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=Ys&0xFF;             //����Ys����Ͱ�λ

	DWINSend_Buff[DwinSend_Buff_Size -strlenth-4]= XNum>>24;           //���ָ�
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-3]= XNum>>16&0xff;      //���ָ�
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-2]= XNum>>8&0xff;       //���ֵ�
	DWINSend_Buff[DwinSend_Buff_Size -strlenth-1]= XNum&0xFF;          //���ֵ�
	
	Send_Data(DWINSend_Buff);                                          //��������
}

//____________________________��ʾ��ά��________________________________//
//QR_Pixel:  ��ά��ÿ����ռ�����ص��С��0x01-0x0F��1-16��
//(Nx��Ny):  ��ά����ʾ�����Ͻ�����
//str��      ��λ���� 
/**************��ά���СΪ��46*QR_Pixel��*��46*QR_Pixle������************/
void DWINLCD_QR_Code(uint8_t QR_Pixel,uint16_t Xs,uint16_t Ys,char*str)	    //��ʾ��ά��
{
	uint16_t strlenth;
	int i;
	strlenth=strlen((const char*)str);
	DwinSend_Buff_Size=11+strlenth;                       //ָ�����ݳ���Ϊ15+strlenth��֡ͷ+ָ��+Mode+����+֡β��
	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -10-strlenth]=0x21;               //��ʾ��ά��ָ��
 
	DWINSend_Buff[DwinSend_Buff_Size -9-strlenth]=Xs>>8;               //��ά��Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -8-strlenth]=Xs&0xFF;             //��ά��Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -7-strlenth]=Ys>>8;               //��ά��Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -6-strlenth]=Ys&0xFF;             //��ά��Ys����Ͱ�λ
	
if(QR_Pixel<=6)                                                      //����ʵ����Ļ��С�����������ص� 
  DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=QR_Pixel;            //��ά�����ص��С
 else
	DWINSend_Buff[DwinSend_Buff_Size -5-strlenth]=0x01;               //��ά�����ص��С����Ĭ��Ϊ1
 
	for(i=strlenth;i>0;i--)
	{		DWINSend_Buff[DwinSend_Buff_Size -4-i]=*(str++); }             //��ά��������ʾ	

	Send_Data(DWINSend_Buff);                                          //��������
	
}

//____________________________��ʾͼƬ________________________________//

/****JPEG ͼƬ��ʾ��480*272 �ֱ��� 4:1:1 ��ʽѹ������ʱ��Ϊ 250mS��****/
//JPEG_ID��   0x00-0x0F����Ӧ JPEG �洢��ͼƬ��ʼ ID��
void DWINLCD_JPEG_Display(uint16_t JPEG_ID)	                //��ʾͼƬ
{
	
	DwinSend_Buff_Size=8;                                 //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x22;            //��ʾͼƬָ��
	
  DWINSend_Buff[DwinSend_Buff_Size -6]=JPEG_ID>>8;      //
	DWINSend_Buff[DwinSend_Buff_Size -5]=JPEG_ID&0xFF;

	Send_Data(DWINSend_Buff);                             //��������
	Delay_ms(300);                                        //�ȴ���Ļ����ʱ��
}

//____________________________��ʾ�������________________________________//
//Dis_CFG ��  0x00=0   �ȣ�   ����ת��       0x01=90  ����ת��
//            0x02=180 �ȣ� �ӽǷ�ת��       0x03=270 ����ת��
void DWINLCD_Display_Dir(uint8_t Dis_CFG)								    //������Ļ��ʾ����
{
		
	DwinSend_Buff_Size=9;                                 //ָ�����ݳ���Ϊ9��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -8]=0x34;            //��Ļ��ʾ�������ָ��
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x5A;            
	DWINSend_Buff[DwinSend_Buff_Size -6]=0xA5;            
	
  DWINSend_Buff[DwinSend_Buff_Size -5]=Dis_CFG%4;       //��Ļ��ʾ�������ָ��
	
	Send_Data(DWINSend_Buff);                             //��������
}

//____________________________������չ���ڲ�����________________________________

/********�ϵ�Ĭ��ֵ�� 0x0088 ����Ӧ 115200bps �����ʡ�***************************/

void  DWINLCDBode_Set(uint16_t Set)                          //������չ���ڲ�����
{
	
			
	DwinSend_Buff_Size=8;                                 //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x38;            //������չ���ڲ�����ָ��
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=15667200/Set/256;            
	DWINSend_Buff[DwinSend_Buff_Size -5]=15667200/Set%256;            
	
	Send_Data(DWINSend_Buff);                             //��������
}

//____________________________��Բ________________________________
//Color: Բ����ɫ
//x0:    Բ��Բ�ĺ�����
//y0:    Բ��Բ��������
//r:     Բ�ΰ뾶

void DWINLCD_Draw_Circle(uint16_t Color,uint16_t x0,uint16_t y0,uint8_t r)			   //��Բ
{
	int a,b;
	a=0;	  
	while(a<=b)
	{
		b=sqrt(r*r-a*a);
		while(a==0){ b=b-1;break;}
		DWINLCD_DrawPoint(Color,1,1,x0+a,y0+b);		               //��������1	
		DWINLCD_DrawPoint(Color,1,1,x0+b,y0+a);		               //��������2
		DWINLCD_DrawPoint(Color,1,1,x0+b,y0-a);		               //��������3	
		DWINLCD_DrawPoint(Color,1,1,x0+a,y0-b);		               //��������4
			
		DWINLCD_DrawPoint(Color,1,1,x0-a,y0-b);		              //��������5			
		DWINLCD_DrawPoint(Color,1,1,x0-b,y0-a);		              //��������6
		DWINLCD_DrawPoint(Color,1,1,x0-b,y0+a);		              //��������7	
		DWINLCD_DrawPoint(Color,1,1,x0-a,y0+b);		              //��������8	
		a++;   					    
	}
}


//____________________________Բ�����________________________________
//FColor: Բ�������ɫ
//x0:     Բ��Բ�ĺ�����
//y0:     Բ��Բ��������
//r:      Բ�ΰ뾶

void DWINLCD_CircleFill(uint16_t FColor,uint16_t x0,uint16_t y0,uint8_t r)			   //Բ�����
{
	int a,b;

for(;r>0;r--)  
	{  a=0;	
		while(a<=b)
		{
			b=sqrt(r*r-a*a);
			while(a==0){ b=b-1;break;}
			DWINLCD_DrawPoint(FColor,2,2,x0+a,y0+b);		               //��������1	
			DWINLCD_DrawPoint(FColor,2,2,x0+b,y0+a);		               //��������2
			DWINLCD_DrawPoint(FColor,2,2,x0+b,y0-a);		               //��������3	
			DWINLCD_DrawPoint(FColor,2,2,x0+a,y0-b);		               //��������4
				
			DWINLCD_DrawPoint(FColor,2,2,x0-a,y0-b);		               //��������5			
			DWINLCD_DrawPoint(FColor,2,2,x0-b,y0-a);		               //��������6
			DWINLCD_DrawPoint(FColor,2,2,x0-b,y0+a);		               //��������7	
			DWINLCD_DrawPoint(FColor,2,2,x0-a,y0+b);		               //��������8	
			a=a+2;   					    
		}
  }
}
//____________________________��ʾICOͼ��________________________________
//��Xs��Ys��: ͼ����ʾ�����Ͻ����ꡣ
//ͼ�걳����ʾIBD: 0=�����˳�����ʾ�� 1=������ʾ        \\���ñ����˳�����ʾʱ����������Ϊ����ɫ//
//����ͼƬ�ָ�BIR: 0=����ͼƬ���ָ� ��1=�Զ�ʹ��        \\0#������ʾ��ͼƬ�������ָ�//
//�����˳�ǿ��BFI: 0=��ͨ��1=��ǿ                       \\������ͼ�걳����ʾ=0ʱ��Ч��//
//ͼ���洢λ��ISI: 0X00-0X0F
//Icon_IDs��  ��Ҫ��ʾ��ͼ��ID��1��ID��1��Byte��ʾ�� 0x00-0xFF
void DWINLCD_ICO_Display(uint16_t Xs, uint16_t Ys,uint8_t IBD,uint8_t BIR,uint8_t BFI,uint8_t ISI,uint8_t Icon_IDs)	     //��ʾͼ��
{
	
	DwinSend_Buff_Size=12;                                //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -11]=0x23;            //��ʾͼ����
	
	DWINSend_Buff[DwinSend_Buff_Size -10]=Xs>>8;               //��ά��Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -9]=Xs&0xFF;             //��ά��Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -8]=Ys>>8;               //��ά��Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -7]=Ys&0xFF;             //��ά��Ys����Ͱ�λ
  	DWINSend_Buff[DwinSend_Buff_Size -6]=IBD%2<<7|BIR%2<<6|BFI%2<<5|ISI;      //��ά��Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=Icon_IDs;

	Send_Data(DWINSend_Buff);                             //��������
}

//____________________________ICOͼ�궯��play________________________________
//��Xs��Ys��: ͼ����ʾ�����Ͻ����ꡣ
//���ؿ���SC��1=���鶯������          0=���鶯���رգ�         ���� 0x29 ָ�����
//����ģʽLM��1=��������ʼͼ�꿪ʼ    0=�������ϴ�ֹͣλ�ÿ�ʼ
//���鶯���ǵڼ��鶯�������鶯��ͼ��ָ��λ�ã�0x00-0x0F��һ���� 16 �鶯��ָ�
//Icon_lib��  ͼ���洢λ�ã�0x00-0x0F��
//Icon_IDs��  ������ʼͼ��λ�ã�0x00-0xFF��
//Icon_IDe��  ������ֹͼ��λ�ã�0x00-0xFF��
//Delay_time������ͼ����ʾʱ������0x00-0xFF����λ 10mS��
void DWINLCD_ICOplay(uint16_t Xs, uint16_t Ys,uint8_t SC,uint8_t LM,uint8_t IconID,uint8_t Icon_lib,uint8_t Icon_IDs,uint8_t Icon_IDe,uint8_t Delay_time)	     //ICOͼ�궯��play
{
	
	DwinSend_Buff_Size=15;                                //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                          //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -14]=0x28;            //��ʾͼ����
	
	DWINSend_Buff[DwinSend_Buff_Size -13]=Xs>>8;              //ICO_Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -12]=Xs&0xFF;             //ICO_Xs����Ͱ�λ
	DWINSend_Buff[DwinSend_Buff_Size -11]=Ys>>8;               //ICO_Ys����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -10]=Ys&0xFF;             //ICO_Ys����Ͱ�λ
	
  DWINSend_Buff[DwinSend_Buff_Size -9]=SC%2<<7|LM%2<<6|IconID;      //��ά��Xs����߰�λ
	
	DWINSend_Buff[DwinSend_Buff_Size -8]=Icon_lib;
	DWINSend_Buff[DwinSend_Buff_Size -7]=Icon_IDs;
  DWINSend_Buff[DwinSend_Buff_Size -6]=Icon_IDe;
	
	 DWINSend_Buff[DwinSend_Buff_Size -5]=Delay_time;
	
	Send_Data(DWINSend_Buff);                             //��������
}
//____________________________ICOͼ�궯���Զ���ʾָ�����________________________________
//Cartoon_Set��ICON ����ָ��ؿ��ƣ�
//on_off:  ÿ�� bit ��Ӧһ��ָ�1=����on��0=�ر�off��
//.15 ��Ӧ�� 15 �鶯��ָ� .0 ��Ӧ�� 0 �鶯��ָ�
//������AA 29 00 05 CC 33 C3 3C ������ 0 �顢�� 2 �鶯��ָ�
void DWINLCD_Cartoon_Set(uint8_t Cartoon_Set,uint8_t on_off)	     //ICOͼ�궯��play
{
	
	DwinSend_Buff_Size=8;                                //ָ�����ݳ���Ϊ8��֡ͷ+ָ��+Mode+����+֡β��	
	Data_init();                                         //���¸�������
		
	DWINSend_Buff[DwinSend_Buff_Size -7]=0x29;            //ICOͼ�궯���Զ���ʾָ�����
	
	DWINSend_Buff[DwinSend_Buff_Size -6]=(on_off<<(Cartoon_Set-1))>>8;           //ICO_Xs����߰�λ
	DWINSend_Buff[DwinSend_Buff_Size -5]=(on_off<<(Cartoon_Set-1))&0xFF;         //ICO_Xs����Ͱ�λ
  	
	Send_Data(DWINSend_Buff);                             //��������
}
