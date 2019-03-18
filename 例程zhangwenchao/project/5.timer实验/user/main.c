#include "sysinc.h"

void timer2()
{	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{ PBout(9)= !PBout(9);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
		}
}


 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	TIM_us_Init(TIM2, 30000, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER2, timer2);

	 PBout(9)=1;
	while(1)
	{			
		PCout(13)=0;
		SYSTICK_DelayMs(900);	 //��ʱ300ms
		PCout(13)=1;	
		SYSTICK_DelayMs(900);	//��ʱ300ms
	}
 }

