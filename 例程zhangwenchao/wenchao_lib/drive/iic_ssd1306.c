/**
  ******************************************************************************
  * @file    ssd1306.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.21
  * @note    �˳���ΪARM�е�1306�ײ�Ӧ�ñ��
  ******************************************************************************
  */
	
#include "sysinc.h"
#include "matrix.h"

/**
 * @brief    		д����
 * @param[in]   IIC_Data ��Ҫд������� 
 */
void OLED_WrDat(unsigned char IIC_Data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);
	IIC_Wait_Ack();

	IIC_Send_Byte(0x40);			//write data
	IIC_Wait_Ack();

	IIC_Send_Byte(IIC_Data);
	IIC_Wait_Ack();
	IIC_Stop();
}

/**
 * @brief    		д����
 * @param[in]   IIC_Data ��Ҫд�������
 */
void OLED_WrCmd(u8 cmd)
{
	IIC_Start();
	IIC_Send_Byte(0x78);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(0x00);		
	IIC_Wait_Ack();

	IIC_Send_Byte(cmd);
	IIC_Wait_Ack();
	IIC_Stop();
}	

/**
 * @brief    		����д�������λ��
 * @param[in]   x x����λ��
 * @param[in]   y y����λ��
 */
void OLED_Set_Pos(u8 x, u8 y)
{
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
}

/**
 * @brief    		ȫ��д��
 * @param[in]   blean  ��Ҫд�������
 */
void OLED_Fill(u8 blean) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_MAX_WIDTH;x++)
		OLED_WrDat(blean);
	}
}

/**
 * @brief    		����
 */
void OLED_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_MAX_WIDTH;x++)
		OLED_WrDat(0);
	}
}

/**
 * @brief    		��ʼ��
 */
void OLED_Init(void)
{
	SYSTICK_DelayMs(500);
  OLED_WrCmd(0xAE); //display off
	OLED_WrCmd(0x20);	//Set Memory Addressing Mode	
	OLED_WrCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	//OLED_WrCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	OLED_WrCmd(0xc8);	//Set COM Output Scan Direction
	OLED_WrCmd(0x00); //---set low column address
	OLED_WrCmd(0x10); //---set high column address
	OLED_WrCmd(0x40); //--set start line address
	OLED_WrCmd(0x81); //--set contrast control register
	OLED_WrCmd(0xff); //���ȵ��� 0x00~0xff
	OLED_WrCmd(0xa1); //--set segment re-map 0 to 127
	OLED_WrCmd(0xa6); //--set normal display
	OLED_WrCmd(0xa8); //--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3F); //
	OLED_WrCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_WrCmd(0xd3); //-set display offset
	OLED_WrCmd(0x00); //-not offset
	OLED_WrCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0xf0); //--set divide ratio
	OLED_WrCmd(0xd9); //--set pre-charge period
	OLED_WrCmd(0x22); //
	OLED_WrCmd(0xda); //--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb); //--set vcomh
	OLED_WrCmd(0x20); //0x20,0.77xVcc
	OLED_WrCmd(0x8d); //--set DC-DC enable
	OLED_WrCmd(0x14); //
	OLED_WrCmd(0xaf); //--turn on oled panel	OLED_Fill(0x00);  
	OLED_Fill(0X00);
} 

/**
 * @brief    		д���ַ�������ʽ6x8
 * @param[in]   x x����λ��
 * @param[in]   y y����λ��
 * @param[in]   ch[] �ַ���
 */
void OLED_P6x8Str(u8 x, u8 y,u8 ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}

/**
 * @brief    		д���ַ�������ʽ8x16
 * @param[in]   x x����λ��
 * @param[in]   y y����λ��
 * @param[in]   ch[] �ַ���
 */
void OLED_P8x16Str(u8 x, u8 y,u8 ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}

/**
 * @brief    		д���ַ�������ʽ16x16
 * @code
 *      				������ʾ��50ʱ���Ƽ���
 *      				  for(int i = 0; i<=5; i++)
 *                {
 *			            OLED_P16x16Ch(i*16,0,i);
 *              	}
 * @endcode
 * @param[in]   x x����λ��
 * @param[in]   y y����λ��
 * @param[in]   N ��N������
 */
void OLED_P16x16Ch(u8 x, u8 y, u8 N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}

/**
 * @brief    		��ͼ
 * @code
 *      				������ʾ��BMP1
 *      				Draw_BMP(0,0, 128, 7,BMP1);
 * @endcode
 * @param[in]   x0 ��ʼx����λ��
 * @param[in]   y0 ��ʼy����λ��
 * @param[in]   x1 ��ʼx����λ��
 * @param[in]   y1 ��ʼy����λ��
 * @param[in]   BMP[] 
 */
void Draw_BMP(u8 x0, u8 y0, u8 x1, u8 y1,u8 BMP[])
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

/**
 * @brief    		����ʾ
 */
void OLED_ON(void)
{
	OLED_WrCmd(0X8D);  //���õ�ɱ�
	OLED_WrCmd(0X14);  //������ɱ�
	OLED_WrCmd(0XAF);  //OLED����
}


/**
 * @brief    		����ʾ
 */
void OLED_OFF(void)
{
	OLED_WrCmd(0X8D);  //���õ�ɱ�
	OLED_WrCmd(0X10);  //�رյ�ɱ�
	OLED_WrCmd(0XAE);  //OLED����
}


/**
 * @brief1    		д����Ҫ�������ֱ�������ʾ�����磺@ADC1��1.23V
 * @brief2    		֧��С��1000������ʾ(��λ������λС��)
 * @code
 *      				������ʾ��@ADC1��1.23V
 *      				float num = 1.23;
 *      				OLED_P6x8Str_Num(0,2,"@ADC1:",'V',num);
 * @endcode
 * @param[in]   x x����λ��
 * @param[in]   y y����λ��
 * @param[in]   ch[] �ַ���
 * @param[in]   ch2[] ��λ
 * @param[in]   num ����
 */
void OLED_P6x8Str_Num(u8 x, u8 y,u8 ch[],u8 ch2,float num)
{
	unsigned char c=0,i=0,j=0;
	short a,number[7]={0,0,0,-2,0,0,0};
	u32 d;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
	d = 1000*num;
	for(a=0; a<3; a++)
	{
		number[6-a] = d%10;
		d=d/10;
	}
	for(a=4; a<7; a++)
	{
		number[6-a] = d%10;
		d=d/10;
	}
  if(num>=100){a=0;}
  else if(num>=10){a=1;}
  else if(num>=0){a=2;}
	for( ; a<7; a++)
	{
			c =number[a]+16;
			if(x>126){x=0;y++;}
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
			OLED_WrDat(F6x8[c][i]);
			x+=6;
			j++;
	}
	for(int h=0; h<7; h++)
			c = ch2 -32;
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
			OLED_WrDat(F6x8[c][i]);
}

