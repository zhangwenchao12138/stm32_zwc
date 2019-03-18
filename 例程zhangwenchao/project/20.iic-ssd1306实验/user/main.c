
#include "sysinc.h"
extern unsigned char BMP1[];
int main()
{
	int i;
	float num =12.342;
	SYSTICK_DelayInit();
	NVIC_init();
  IIC_Init();
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	OLED_Init();
  for(i = 0; i<=5; i++)
	{
			OLED_P16x16Ch(i*16,0,i);
	}

  OLED_P6x8Str_Num(0,2,"@ADC1:",'V',num);
  OLED_P6x8Str_Num(0,3,"@ADC1:",'V',num);
  OLED_P6x8Str_Num(0,4,"@ADC1:",'V',num);
  OLED_P6x8Str_Num(0,5,"@ADC1:",'V',num);
		OLED_P6x8Str_Num(0,6,"@ADC1:",'V',num);
  //OLED_P6x8Str_Num(0,7,"@ADC1:",'V',num);
		SYSTICK_DelayMs(1000);
		SYSTICK_DelayMs(1000);
		SYSTICK_DelayMs(1000);
		SYSTICK_DelayMs(1000);
    Draw_BMP(0,0, 128, 7,BMP1);
		while(1){
		SYSTICK_DelayMs(1000);
		PCout(13)=1;
		SYSTICK_DelayMs(900);	 //延时300ms
		PCout(13)=0;	
		SYSTICK_DelayMs(900);	//延时300ms
	}
}
