#include "sysinc.h"

u8 txbuf[33]={1,7,3,99};
u8 rxbuf[33]={0};

void exti0(void)
{ 			
	NRF24L01_TxPacket(SPIcs, txbuf);
}

void exti6(void)
{ 			
	NRF24L01_RxPacket(SPIcs, rxbuf);
	printf(" rx =  %d\n", rxbuf[3]);
}
 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_0, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_0, exti0);
  NRF24L01_Init(Spics); 
	EXTI_CallbackInstall(EXTI_Pin_6, exti6);
	
	 while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);	 //延时300ms
		PCout(13)=1;	
		SYSTICK_DelayMs(900);	//延时300ms
	}
 }

