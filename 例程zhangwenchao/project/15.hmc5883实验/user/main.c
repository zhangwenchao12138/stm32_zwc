#include "sysinc.h"

HMC_DATE hmc_date[1]={0};


void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{
			
			HMC5883_GetLevel_Angle(hmc_date);
			printf(" angle = %f     %d        %d    \r\n ",hmc_date->hmc_level_angle,hmc_date->hmc_date_x,hmc_date->hmc_date_y);			

			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
		}
}



 int main(void)
 {		 
	NVIC_init();
	SYSTICK_DelayInit();	    	   	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	IIC_Init();
	HMC5883_Init();
	HMC5883_offset(hmc_date);
	TIM_ms_Init(TIM3, 10, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
	printf(" %d      %d     \r\n ",hmc_date->hmc_offset_x,hmc_date->hmc_offset_y);			

  while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
}

