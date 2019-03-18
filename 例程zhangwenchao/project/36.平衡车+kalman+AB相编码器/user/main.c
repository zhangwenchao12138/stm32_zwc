#include "sysinc.h"

#include "balance_car.h"

ACGY acgy[1]={0};
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};
KALMAN_DIFFERENT_PARAMETER kalman[1] = {0};

int conter = 0;
int tmppwmv = 0;
int tmppwm = 0;
int tmppwmt = 0;

#define MAXPWM 9800

#define EXTITRIGGER_TYPE  EXTI_Trigger_Rising

int i=0, k=0;
int time=0;

void uart3(uint16_t Res)
{		
  //UART_WriteByte(HW_UART3, Res);
	set_flag_move((int)Res);
}

void exti_moter1(void)
{ 			
	if(PAin(7) == 1){
		i++;
	}else{
		i--;
	}
}

void exti_moter2(void)
{ 			
	if(PBin(6) == 1){
		k++;	
	}else{
		k--;;	
	}
}
void motor_ctrl(int motor1_pwm, int motor2_pwm);
void timer3()
{	
		MPU6050_Get_Basic_Data(acgy);
		MPU6050_Acc_Angle_Approximately(acgy);
		MPU6050_Gyr_W(acgy);
		Kalman_Different_Q_Filter(kalman, acgy->angle_acc_x, acgy->w_gyr_x);
		time++;
		tmppwm = (int)(AngleController(kalman->angle, kalman->angle_dot));
		if(++conter > 8){
				tmppwmv = VelocityController(i, i);
				tmppwmt = TurnController(acgy->w_gyr_z);
				conter=0;
				i = 0;
				k = 0;
		}				

		int pwm = tmppwm - tmppwmv;
		int pwmL = pwm + tmppwmt;
		int pwmR = pwm - tmppwmt;
		
		if(pwmL > MAXPWM) pwmL = MAXPWM;
		else if(pwmL < -MAXPWM) pwmL = -MAXPWM;
		
		if(pwmR > MAXPWM) pwmR = MAXPWM;
		else if(pwmR < -MAXPWM) pwmR = -MAXPWM;
		
		motor_ctrl(pwmL, pwmR );
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);
	UART_QuickInit(HW_UART3, 9600, 2, 2, ENABLE);
	UART_CallbackInstall(HW_UART3, uart3);

	IIC_Init();
	MPU6050_Init();
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");
	else  {printf(" NO MPU6050_Check\n");return 0;}
	
	First_complementary_parameter_init(fcp, 5);
	kalman_different_parameter_init(kalman,5);
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 0, 3);
	EXTI_CallbackInstall(EXTI_Pin_6, exti_moter1);	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_IPU);

	
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_7, 0, 3);
	EXTI_CallbackInstall(EXTI_Pin_7, exti_moter2);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPU);
	
	TIM_us_Init(TIM3, 5000, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);

  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP);
	PBout(14)= 1;
	PBout(15)= 0;

	TIM_PWM_Init(TIM2, PWM_CH4, 1000, ENABLE);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_12, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_13, GPIO_Mode_Out_PP);
	PBout(12)= 1;
	PBout(13)= 0;
	
	TIM_SetCompare(TIM2, PWM_CH3, 0);
	TIM_SetCompare(TIM2, PWM_CH4, 0);


  while(1)
	{		
		//printf(" %f  %f\n",kalman->angle, kalman->angle_dot);
		printf("en:  %d   %d \n", i,k);
		if(time >= 100){
			PCout(13)= !PCout(13);
			time = 0;
		}
	}
}
 
void motor_ctrl(int motor1_pwm, int motor2_pwm){
	if(motor1_pwm >= 0){
		PBout(14)= 1;
		PBout(15)= 0;
		TIM_SetCompare(TIM2, PWM_CH3, motor1_pwm);
	}else{
		PBout(14)= 0;
		PBout(15)= 1;
		TIM_SetCompare(TIM2, PWM_CH3, -1*motor1_pwm);
	}
	
	if(motor2_pwm >= 0){
		PBout(12)= 0;
		PBout(13)= 1;
			TIM_SetCompare(TIM2, PWM_CH4, motor2_pwm);
	}else{
		PBout(12)= 1;
		PBout(13)= 0;
			TIM_SetCompare(TIM2, PWM_CH4, -1*motor2_pwm);
	}
}


