/**
  ******************************************************************************
  * @file    i2c.h
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.21
  * @note    此程序为ARM中IIC的底层编程
  ******************************************************************************
  */														
	
#include "sysinc.h"


 /**
 * @brief   		I2C初始化
 */
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;  																		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 																					//PB10,PB11 输出高
}

 /**
 * @brief   		产生IIC起始信号
 */
void IIC_Start(void)
{
	SDA_OUT();     																																					//sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	SYSTICK_DelayUs(4);
 	IIC_SDA=0;																																							//START:when CLK is high,DATA change form high to low 
	SYSTICK_DelayUs(4);
	IIC_SCL=0;																																							//钳住I2C总线，准备发送或接收数据 
}	  

/**
 * @brief   		产生IIC停止信号
 */
void IIC_Stop(void)
{
	SDA_OUT();																																							//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;																																							//STOP:when CLK is high DATA change form low to high
	SYSTICK_DelayUs(4);
	IIC_SCL=1; 
	IIC_SDA=1;																																							//发送I2C总线结束信号
	SYSTICK_DelayUs(4);
}

/**
 * @brief    		等待应答信号到来
 * @return			返回值：1，接收应答失败     0，接收应答成功
 */
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      																																				//SDA设置为输入  
	IIC_SDA=1;
	SYSTICK_DelayUs(1);
	IIC_SCL=1;
	SYSTICK_DelayUs(1);	
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;																																					//时钟输出0 	   
	return 0;  
} 

/**
 * @brief    		产生ACK应答
 */
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	SYSTICK_DelayUs(2);
	IIC_SCL=1;
	SYSTICK_DelayUs(2);
	IIC_SCL=0;
}

/**
 * @brief    		不产生ACK应答
 */
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	SYSTICK_DelayUs(2);
	IIC_SCL=1;
	SYSTICK_DelayUs(2);
	IIC_SCL=0;
}					 				     

/**
 * @brief    		IIC发送一个字节
 * @param[in]   txd : 需要发送的数据
 * @date：      返回从机有无应答  1，有应答  0，无应答			
 */
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 	    
    IIC_SCL=0;																																										//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
																																																	//IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		SYSTICK_DelayUs(2);
		IIC_SCL=1;
		SYSTICK_DelayUs(2);
		IIC_SCL=0;	
		SYSTICK_DelayUs(2);
    }	 
} 	    

/**
 * @brief    		IIC读一个字节
 * @param[in]   ack    ack=1时，发送ACK，ack=0，发送nACK   
 */
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();																																														//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    IIC_SCL=0; 
		SYSTICK_DelayUs(2);
		IIC_SCL=1;
    receive<<=1;
    if(READ_SDA)receive++;   
		SYSTICK_DelayUs(1);
  }					 
    if (!ack)
        IIC_NAck();																																									//发送nACK
    else
        IIC_Ack(); 																																									//发送ACK   
    return receive;
}

