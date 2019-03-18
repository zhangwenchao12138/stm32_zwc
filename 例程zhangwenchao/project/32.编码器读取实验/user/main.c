#include "sysinc.h"

#include "balance_car.h"

#define EXTITRIGGER_TYPE  EXTI_Trigger_Rising

int i=0, k=0;
void exti_moter1(void)
{ 			
	if(PAin(7) == 1){
		i++;
	}else{
		i--;
	}
	printf("i = %d \n", i);
}

void exti_moter2(void)
{ 			
	if(PBin(6) == 1){
		k++;	
	}else{
		k--;;	
	}
	printf("k = %d \n", k);
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_6, exti_moter1);	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_IPU);

	
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_7, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_7, exti_moter2);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPU);

  while(1)
	{		
			PCout(13)= !PCout(13);
			SYSTICK_DelayMs(500);
	}
}
 

