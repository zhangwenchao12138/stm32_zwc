#include "sysinc.h"

ACGY acgy[1]={0};
float k;
TWO_COMPLEMENTARY_PARAMETER two_comp[1]={0};

void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{ 
		MPU6050_GetData(acgy);
		MPU6050_Acc_Angle(acgy);
		k=acgy->gyr[1]/16.4*(-1);
		printf(" %f  ",acgy->angle_acc_x);
		Two_order_complementary_filter(two_comp,acgy->angle_acc_x, k);
		
		printf("    %f     %f     \r\n ",two_comp->angle2,k);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
		}
}



 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
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

