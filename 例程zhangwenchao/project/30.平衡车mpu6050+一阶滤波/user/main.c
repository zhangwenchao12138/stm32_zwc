#include "sysinc.h"

ACGY acgy[1]={0};                                          //初始化一个结构体，这里面包含有目前我们用的所有内容
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};                //初始化 一阶滤波 参数结构体
 
int time=0;                                                //用作中断次数计时，用来给while循环中提供准确的时间

double gyr_angle = 0;

void timer3()
{	
		MPU6050_Get_Basic_Data(acgy);                          //得到MPU6050的基本数据（加速度值与角速度值）
		MPU6050_Acc_Angle_Approximately(acgy);                 //由加速度近似求出角度
		MPU6050_Gyr_W(acgy);                                   //由角速度值转化为角速度（单位转化为度每秒）
		First_order_complementary_filter(fcp, acgy->angle_acc_x, acgy->w_gyr_x);  //一节互补滤波器
		gyr_angle += acgy->w_gyr_x*0.005;
		time++;
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	                              //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);  //初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);           //初始化串口1，设置波特率为115200     

	IIC_Init();                                               //初始化IIC
	MPU6050_Init();                                           //初始化MPU6050
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //检查与mpu6050通信是否正常
	else  {printf(" NO MPU6050_Check\n");return 0;}           //如果不正常，直接退出程序
	
	First_complementary_parameter_init(fcp, 5);               //初始化一阶滤波参数值
	
	TIM_us_Init(TIM3, 5000, TIM_IT_Update,2, ENABLE);	        //定时器3提供一个5ms的中断
 	TIMER_CallbackInstall(HW_TIMER3, timer3);                 //注册中断函数，每当定时器3中断时，调用timer3函数，就是最上面的那个函数

  while(1)
	{		
		printf(" %f  %f %f\n",fcp->angle, acgy->angle_acc_x, gyr_angle);          //输出一阶互补滤波后的角度值和原始计算角度值
		if(time >= 100){                                        //每中断100次，也就是500ms进入一次
			PCout(13)= !PCout(13);                                //LED闪烁
			time = 0;                                             //清零
		}
	}
}

