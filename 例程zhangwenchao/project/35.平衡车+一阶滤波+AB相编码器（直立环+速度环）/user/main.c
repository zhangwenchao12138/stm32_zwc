#include "sysinc.h"

#include "balance_car.h"

ACGY acgy[1]={0};                                          //初始化一个结构体，这里面包含有目前我们用的所有内容
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};                //初始化 一阶滤波 参数结构体
KALMAN_DIFFERENT_PARAMETER kalman[1] = {0};                //初始化 卡尔曼滤波参数结构体

int timerInterruptCounter = 0;                             //定时器中断计数，每计数八次运行一次速度环控制函数
int Velocity_pwm = 0;                                      //速度环控制求得的PWM值
int angle_pwm  = 0;                                        //直立环控制求得的PWM值

#define MAXPWM 10000                                       //PWM限幅参数，即pwm给到电机时的最大值

#define EXTITRIGGER_TYPE  EXTI_Trigger_Rising              //外部中断模式采用上升沿触发方式

int motor1_encoder=0, motor2_encoder=0;                    //编码器计数，在这里作为速度值
int time=0;																								 //当进入5ms定时器中断时，timer加1，给while中的程序提供准确时钟（因为有中断，所以while中的演示函数就会不准）

void exti_moter1(void);                                    //外部中断 -- 编码器计数函数
void exti_moter2(void);																		 //外部中断 -- 编码器计数函数
void motor_ctrl(int motor1_pwm, int motor2_pwm);           //电机控制函数

void timer3()                                              //定时器中断，5ms进入一次，是平衡车控制核心
{	
					MPU6050_Get_Basic_Data(acgy);                    //得到MPU6050的基本数据（加速度值与角速度值）
					MPU6050_Acc_Angle_Approximately(acgy);           //由加速度近似求出角度
					MPU6050_Gyr_W(acgy);                             //由角速度值转化为角速度（单位转化为度每秒）
				//	First_order_complementary_filter(fcp , acgy->angle_acc_x, acgy->w_gyr_x);
					Kalman_Different_Q_Filter(kalman, acgy->angle_acc_x, acgy->w_gyr_x);  //一节互补滤波器得到可靠角度
					time++;                                          //给while循环里面提供一个5ms的时间基准
					angle_pwm  = (int)(AngleController(kalman->angle, kalman->angle_dot));  //直立环控制返回pwm

					if(++timerInterruptCounter > 8){                 //每八次进入一次，求得速度环pwm
						Velocity_pwm = VelocityController(motor1_encoder, motor2_encoder);   //速度环求得pwm
						timerInterruptCounter = 0;
						motor1_encoder = 0;                            //清零，重新计数，作为速度值
						motor2_encoder = 0;
					}
					int pwm = angle_pwm  + Velocity_pwm;             //求得总pwm
					if(pwm > MAXPWM) pwm = MAXPWM;                   //限幅
					else if(pwm < -MAXPWM) pwm = -MAXPWM;
					motor_ctrl(pwm, pwm );                           //电机控制
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	 														 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP); //初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);          //初始化串口1，设置波特率为115200

	IIC_Init();                                              //初始化IIC
	MPU6050_Init();                                          //初始化MPU6050	 
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //检查与mpu6050通信是否正常
	else  {printf(" NO MPU6050_Check\n");return 0;}           //如果不正常，直接退出程序
	
	First_complementary_parameter_init(fcp, 5);
	kalman_different_parameter_init(kalman,5);               //kalman参数初始化
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 0, 3);              //编码器外部中断初始化
	EXTI_CallbackInstall(EXTI_Pin_6, exti_moter1);	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_IPU);

	
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_7, 0, 3);              //编码器外部中断初始化
	EXTI_CallbackInstall(EXTI_Pin_7, exti_moter2);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPU);
	
	TIM_us_Init(TIM3, 5000, TIM_IT_Update,2, ENABLE);	        //定时器3提供一个5ms的中断
 	TIMER_CallbackInstall(HW_TIMER3, timer3);                 //注册中断函数，每当定时器3中断时，调用timer3函数，就是最上面的那个函数

  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);                //左轮 PWM初始化，频率1000HZ
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP);	//TB6612的输入引脚，控制电机旋转方向
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP);  //TB6612的输入引脚，控制电机旋转方向
	PBout(14)= 1;
	PBout(15)= 0;

	TIM_PWM_Init(TIM2, PWM_CH4, 1000, ENABLE);                //右轮 PWM初始化，频率1000HZ
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_12, GPIO_Mode_Out_PP);  //TB6612的输入引脚，控制电机旋转方向
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_13, GPIO_Mode_Out_PP);  //TB6612的输入引脚，控制电机旋转方向
	PBout(12)= 1;
	PBout(13)= 0;
	
	TIM_SetCompare(TIM2, PWM_CH3, 0);                         //初始化左轮的占空比为0----转速为0
	TIM_SetCompare(TIM2, PWM_CH4, 0);													//初始化右轮的占空比为0----转速为0


  while(1)
	{		
		if(time >= 100){                                        //每中断100次，也就是500ms进入一次
			PCout(13)= !PCout(13);                                //LED闪烁
			time = 0;                                             //清零
		}
	}
}
 
 /**
 * @brief   		电机控制函数
 * @code
 *      				//初始化电机A和B正转，转速为占空比50%的转速
 *      				motor_ctrl(5000, 5000);
 * @endcode
 * @param[in]   motor1_pwm  A电机占空比，输入-10000~+10000,当电机输入为负的时候，电机反转，当电机输入为正的时候，电机正转
 * @param[in]   motor1_pwm  A电机占空比，输入-10000~+10000,当电机输入为负的时候，电机反转，当电机输入为正的时候，电机正转
 */
void motor_ctrl(int motor1_pwm, int motor2_pwm){
	if(motor1_pwm >= 0){   //当pwm > 0时，通过控制TB6612的AIN1和AIN2的高低电平来控制电机的正反转。
		PBout(14)= 1;        //AIN1
		PBout(15)= 0;        //AIN2
		TIM_SetCompare(TIM2, PWM_CH3, motor1_pwm);  //控制电机转速
	}else{
		PBout(14)= 0;
		PBout(15)= 1;
		TIM_SetCompare(TIM2, PWM_CH3, -1*motor1_pwm);
	}
	
	if(motor2_pwm >= 0){  //当pwm > 0时，通过控制TB6612的BIN1和BIN2的高低电平来控制电机的正反转。
		PBout(12)= 0;
		PBout(13)= 1;
		TIM_SetCompare(TIM2, PWM_CH4, motor2_pwm);
	}else{
		PBout(12)= 1;
		PBout(13)= 0;
		TIM_SetCompare(TIM2, PWM_CH4, -1*motor2_pwm);
	}
}

void exti_moter1(void)
{ 			
	if(PAin(7) == 1){
		motor1_encoder++;
	}else{
		motor1_encoder--;
	}
}

void exti_moter2(void)
{ 			
	if(PBin(6) == 1){
		motor2_encoder++;	
	}else{
		motor2_encoder--;;	
	}
}
