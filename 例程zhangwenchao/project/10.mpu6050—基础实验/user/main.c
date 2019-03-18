#include "sysinc.h"


ACGY acgy[1]={0};


 int main(void)
 {
	float k;
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
	
  while(1)
	{		
		MPU6050_GetData(acgy);
		k=acgy->gyr[0]/32.8;
		printf(" k = %f\r\n",k);
		for(int i =0; i<3; i++)
	{
		printf(" gyr:%d\r",acgy->gyr[i]);
	}printf("\n");
		for(int i =0; i<3; i++)
	{
		printf(" acc:%d\r",acgy->acc[i]);
		} printf("tem: %d",acgy->tmp); printf("\n");
		
		PCout(13)=0;
		SYSTICK_DelayUs(100000);
		PCout(13)=1;	
		SYSTICK_DelayUs(200000);
	}
 }

