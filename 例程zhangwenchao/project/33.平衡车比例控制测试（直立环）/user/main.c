/**
  ******************************************************************************
  * @file         资源分配表
  * @author  			Harry
	* @qq      			3502596529
  * @version 			V2.0
  * @date    			2018.10.5
  * @note    			测试程序 -- 比例控制
  * @config_pwm   timer2
  *               @arg PWM_CH3                 PA2      TB6612的PWMA的输入引脚，用于控制电机A转速
  *               @arg PWM_CH4                 PA3			TB6612的PWMA的输入引脚，用于控制电机B转速  
	* @config_gpio  TB6612的输入引脚，控制电机旋转方向
  *               @arg AIN1                    PB14     控制电机A旋转方向
  *               @arg AIN2                    PB15			控制电机A旋转方向
	*               @arg BIN1                    PB13     控制电机B旋转方向
  *               @arg BIN2                    PB12			控制电机B旋转方向
  * @config_iic   模拟iic(mpu6050)
  *               @arg IIC_SCL                 PB8
  *               @arg IIC_SDA                 PB9 
  * @config_led   指示灯
  *               @arg 运行指示灯              PC13
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  ******************************************************************************
  */

#include "sysinc.h"

ACGY acgy[1]={0};                                          //初始化一个结构体，这里面包含有目前我们用的所有内容
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};                //初始化 一阶滤波 参数结构体
 
int time=0;                                                //用作中断次数计时，用来给while循环中提供准确的时间

#define MAXPWM 9800                                        //用作占空比限制，占空比最大只能到9600（本来最大是10000，为了防止电机用力过大，做限制）
#define KP 500                                             //角度的比例参数，pwm = KP * fcp->angle;

void motor_ctrl(int motor1_pwm, int motor2_pwm);           //声明两个电机的控制函数


//定时器中断函数，每5ms进入一次
void timer3()
{	
		MPU6050_Get_Basic_Data(acgy);                          //得到MPU6050的基本数据（加速度值与角速度值）
		MPU6050_Acc_Angle_Approximately(acgy);                 //由加速度近似求出角度
		MPU6050_Gyr_W(acgy);                                   //由角速度值转化为角速度（单位转化为度每秒）
			
		First_order_complementary_filter(fcp, acgy->angle_acc_x, acgy->w_gyr_x);  //一节互补滤波器得到可靠角度

		time++;                                                //给while循环里面提供一个5ms的时间基准
	
		int pwm = KP * fcp->angle;                             //给角度乘上比例系数作为电机控制的输入
	
		if(pwm > MAXPWM) pwm = MAXPWM;                         //限制正PWM的最大值
		else if(pwm < -MAXPWM) pwm = -MAXPWM;                  //限制负PWM的最小值
	
		motor_ctrl(pwm, pwm );                                 //电机控制函数，详见下面函数解释
}

 int main(void)
 {
	NVIC_init();                                            //
	SYSTICK_DelayInit();	    	                               //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);   //初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);            //初始化串口1，设置波特率为115200  

	IIC_Init();                                                //初始化IIC
	MPU6050_Init();                                            //初始化MPU6050	 
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //检查与mpu6050通信是否正常
	else  {printf(" NO MPU6050_Check\n");return 0;}           //如果不正常，直接退出程序
	
	First_complementary_parameter_init(fcp, 5);

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


