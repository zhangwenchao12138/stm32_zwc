/**
  ******************************************************************************
  * @file    adc.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.10
  * @note    此程序为ARM中ADC的底层编程
  ******************************************************************************
  */

#include "sysinc.h"


 /**
 * @brief   		ADC初始化函数
 * @code
 *      				// 初始化ADC通道4
 *      				ADC_init(ADC_CH4);
 * @endcode
 * @param[in]   ADC_CHx ADC通道号：   值：ADC_CH0~ADC_CH15
 */
void ADC_init(u8 ADC_CHx)
{    
	switch(ADC_CHx)
	{
		case ADC_CH0: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFFFFFFF0; break;
		case ADC_CH1: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFFFFFF0F; break;
		case ADC_CH2: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFFFFF0FF; break;
		case ADC_CH3: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFFFF0FFF; break;
		case ADC_CH4: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFFF0FFFF; break;
		case ADC_CH5: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XFF0FFFFF; break;
		case ADC_CH6: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0XF0FFFFFF; break;
		case ADC_CH7: RCC->APB2ENR|=1<<2; GPIOA->CRL&=0X0FFFFFFF; break;
		case ADC_CH8: RCC->APB2ENR|=1<<3; GPIOB->CRL&=0XFFFFFFF0; break;
		case ADC_CH9: RCC->APB2ENR|=1<<3; GPIOB->CRL&=0XFFFFFF0F; break;
		case ADC_CH10:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFFFFFFF0; break;
		case ADC_CH11:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFFFFFF0F; break;
		case ADC_CH12:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFFFFF0FF; break;
		case ADC_CH13:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFFFF0FFF; break;
		case ADC_CH14:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFFF0FFFF; break;
		case ADC_CH15:RCC->APB2ENR|=1<<4; GPIOC->CRL&=0XFF0FFFFF; break;
	}
																																								//通道10/11设置			 
	RCC->APB2ENR|=1<<9;    																												//ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   																												//ADC1复位
	RCC->APB2RSTR&=~(1<<9);																												//复位结束	    
	RCC->CFGR&=~(3<<14);  																												//分频因子清零	
																																								//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
																																								//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 
	ADC1->CR1&=0XF0FFFF;   																												//工作模式清零
	ADC1->CR1|= 0<<16;
	ADC1->CR1|= ~(1<<8);
	ADC1->CR2|= 1<<20;
	ADC1->CR2&= ~(1<<1);
	ADC1->CR2|= 7<<17;
	ADC1->CR2&=~(1<<11); 
	ADC1->SMPR2&=~(7<<3);
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1|=0<<20;  
	ADC1->CR2|=1<<0;	
	ADC1->CR2|=1<<3;
	while(ADC1->CR2&(1<<3));
	ADC1->CR2|=1<<2;
	while(ADC1->CR2&(1<<2));
}				  

 /**
 * @brief   		得到ADC转换值函数
 * @code
 *      				// 得到ADC通道4值
 *      				Get_Adc(ADC_CH4);
 * @endcode
 * @param[in]   ADC_CHx ADC通道号：   值：ADC_CH0~ADC_CH15
 */
u16 Get_Adc(u8 ADC_CHx)   
{
																															//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;																			//规则序列1 通道ch
	ADC1->SQR3|=ADC_CHx;		  			    
	ADC1->CR2|=1<<22;       																		//启动规则转换通道 
	while(!(ADC1->SR&1<<1));																		//等待转换结束	 	   
	return ADC1->DR;																						//返回adc值	
}

 /**
 * @brief   		得到ADC转换平均值函数
 * @code
 *      				// 得到ADC通道4值，次数为20
 *      				Get_Adc_Average(ADC_CH4，20);
 * @endcode
 * @param[in]   ADC_CHx ADC通道号：   值：ADC_CH0~ADC_CH15
 * @param[in]   times  平均次数        
 */
u16 Get_Adc_Average(u8 ADC_CHx,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ADC_CHx);
		//SYSTICK_DelayMs(5);	 
	}
	return temp_val/times;
} 


