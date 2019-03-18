#include "sysinc.h"
float num[2];

 int main(void)
 {	
	u16 adcx;
	u16 adc5;
	float temp;
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	 	IIC_Init();
	OLED_Init();//ssd1306

	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	 
	ADC_init(ADC_CH4);

	while(1)
	{		
		adcx=Get_Adc_Average(ADC_CH4,10);
		printf("adc4 val: %d\r\t",adcx);
		temp=(float)adcx*(3.3/4096);
		printf("adc val: %f\n",temp);
		

	//	  OLED_P6x8Str_Num(2,3,"@input_V:",'V',(num[1]));

		SYSTICK_DelayMs(250);	 //延时300ms
		PCout(13)=	!PCout(13);	
	}
 }

