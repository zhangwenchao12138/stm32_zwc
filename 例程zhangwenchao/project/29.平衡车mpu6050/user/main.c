#include "sysinc.h"

ACGY acgy[1]={0};                //初始化一个结构体，这里面包含有目前我们用的所有内容

int main(void)
{
	NVIC_init();
	SYSTICK_DelayInit();	    	                                  //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);      //初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);               //初始化串口1，设置波特率为115200     

	IIC_Init();           //初始化IIC
	MPU6050_Init();       //初始化MPU6050
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");            //检查与mpu6050通信是否正常
	else  {printf(" NO MPU6050_Check\n");return 0;}               //如果不正常，直接退出程序

  while(1)
	{		
			MPU6050_Get_Basic_Data(acgy);                             //得到MPU6050的基本数据（加速度值与角速度值）
			MPU6050_Acc_Angle_Approximately(acgy);                    //由加速度近似求出角度
			MPU6050_Gyr_W(acgy);                                      //由角速度值转化为角速度（单位转化为度每秒）
			
			printf("acc_angle=%f   gyr=%f \n",acgy->angle_acc_x, acgy->w_gyr_x);  //串口输出X轴的倾斜角度值和X轴方向的角速度值。
			PCout(13)= !PCout(13);                           //LED灯闪烁
			SYSTICK_DelayMs(300);                            //延时300MS
	}
}

