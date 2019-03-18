#include "sysinc.h"

 int main(void)
 {
	NVIC_init();                 //�ж����ȼ���ʼ��
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	
	 
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP); //����ָʾ�Ƴ�ʼ��
	 
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP); 
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP);  
	PBout(14)= 1;
	PBout(15)= 0;

  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);  //PWM��ʼ����Ƶ��1000HZ
	TIM_SetCompare(TIM2, PWM_CH3, 5000);        //ռ�ձ�Ϊ50%

  while(1)
	{		
		PCout(13)=1;
		SYSTICK_DelayMs(1000);	//��ʱ1000ms
		PCout(13)=0;
		SYSTICK_DelayMs(1000);	//��ʱ1000ms
	}
}
 
