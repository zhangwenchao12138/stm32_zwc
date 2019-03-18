#include "sysinc.h"


void exti0(void)
{ 			
	SYSTICK_DelayMs(200);	 //��ʱ300ms
		PBout(9) = !PBout(9);
}
 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_0, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_0, exti0);

	while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);	 //��ʱ300ms
		PCout(13)=1;	
		SYSTICK_DelayMs(900);	//��ʱ300ms
	}
 }

