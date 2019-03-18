#include "sysinc.h"

ACGY acgy[1]={0};
KALMAN_SAME_PARAMETER kalman_same[1]={0};

void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{ 
		MPU6050_GetData(acgy);
		MPU6050_Acc_Angle(acgy);
		MPU6050_Gyr_W(acgy);
		printf(" %f  ",acgy->angle_acc_x);
		Kalman_Same_Q_Filter(kalman_same,acgy->angle_acc_x, (-1)*acgy->w_gyr_y);
		
		printf("    %f     %f\r\n ",kalman_same->angle_00,(-1)*acgy->w_gyr_y);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־ 
		}
}



 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	IIC_Init();
	MPU6050_Init();
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");
	else  {printf("NO MPU6050_Check\n");return 0;}
	
	MPU6050_Offset(acgy);
	
	for(int i =0; i<3; i++)
	{
		printf(" gyr_off:%d\r",acgy->gyr_offset[i]);
	}printf("\n");
	
	TIM_ms_Init(TIM3, 10, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
  while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(500);
		PCout(13)=1;	
		SYSTICK_DelayMs(500);
	}
}

