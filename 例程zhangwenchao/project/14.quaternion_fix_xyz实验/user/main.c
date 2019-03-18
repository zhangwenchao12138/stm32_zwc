#include "sysinc.h"

ACGY acgy[1]={0};
extern float Yaw,Pitch,Roll;  



void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{ 
			MPU6050_GetData(acgy);
			MPU6050_Acc_Angle(acgy);
		  MPU6050_Gyr_W(acgy);
			
			IMUupdate(acgy->gyr[0],acgy->gyr[1], acgy->gyr[2],acgy->acc[0], acgy->acc[1], acgy->acc[2]);
		
			printf(" %f      %f     %f    \r\n ",acgy->angle_acc_x,(-1)*Pitch,0.9*acgy->angle_acc_x+(-0.1)*Pitch);
			
			
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
	
	TIM_ms_Init(TIM3, 5, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
  while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
}

