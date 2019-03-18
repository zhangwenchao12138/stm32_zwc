#include "sysinc.h"

HMC_DATE hmc_date[1]={0};
MPU_DATE mpu_date[1]={0};

FLYING_ANGLE fly_angle[1]={0};

KALMAN_SAME_PARAMETER kalman_pitch[1]={0};
KALMAN_SAME_PARAMETER kalman_roll[1]={0};
//KALMAN_SAME_PARAMETER kalman_yaw[1]={0};
FIRST_COMPLEMENTARY_PARAMETER first_comp[1]={0};
PID_CASCADE_DATE pid_date_pitch[1]={0};
PID_CASCADE_DATE pid_date_roll[1]={0};
PID_CASCADE_DATE pid_date_yaw[1]={0};
PWM_DUTY pwm_duty[1]={0};

void timer3(void);
void first_init(void);
void hardware_init(void);
void kalman_init(void);
void pwm_init(void);
void pid(void);
void pid_init(void);


int main(void)
{		 
	first_init();
	hardware_init();
	kalman_init();
	pwm_init();
  TIM_ms_Init(TIM3, 10, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);

  while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
}




void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{
			Nine_Axis_Get(mpu_date,hmc_date);
			Get_kalman_Pitch(kalman_pitch,mpu_date,fly_angle);
      Get_kalman_Roll(kalman_roll,mpu_date,fly_angle);
			Get_firstcomp_Yaw(first_comp,mpu_date, hmc_date,fly_angle);
			//printf(" pitch = %f    %f     %f      \r\n",fly_angle->yaw,fly_angle->pitch,fly_angle->roll);			
			pid();			
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
		}
}

void first_init(void)
{
  NVIC_init();
	SYSTICK_DelayInit();
	IIC_Init();
	 
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
}

void hardware_init(void)
{
	HMC5883_Init();
	//HMC5883_offset(hmc_date);
	hmc_date->hmc_offset_x = -4;
	hmc_date->hmc_offset_y = -50;
	hmc_date->hmc_offset_z = -40;
 
	//PCout(13)=1;	
	//SYSTICK_DelayMs(2000);
	 
	MPU6050_Init();
	MPU6050_Offset(mpu_date);
}


void kalman_init(void)
{
	kalman_same_parameter_init(kalman_pitch,10);
	kalman_same_parameter_init(kalman_roll,10);
	//kalman_same_parameter_init(kalman_yaw,10);
	First_complementary_parameter_init(first_comp,10);
}

void pwm_init(void)
{
	TIM_PWM_Init(TIM2, PWM_CH1, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH2, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH4, 1000, ENABLE);
	TIM_SetCompare(TIM2, PWM_CH1, 0);
	TIM_SetCompare(TIM2, PWM_CH2, 0);
	TIM_SetCompare(TIM2, PWM_CH3, 0);
	TIM_SetCompare(TIM2, PWM_CH4, 0);
}

void pid(void)
{
	PID_Cascade(0,fly_angle->pitch, pid_date_pitch);
	PID_Cascade(0,fly_angle->roll, pid_date_roll);
	PID_Cascade(0,fly_angle->yaw, pid_date_yaw);
	
	pwm_duty->pwm1 = PWM_BASE_F - pid_date_pitch->pid_in_control + 
									 pid_date_roll->pid_in_control + pid_date_yaw->pid_in_control;
	
	pwm_duty->pwm2 = PWM_BASE_F - pid_date_pitch->pid_in_control - pid_date_roll->pid_in_control;
	
	pwm_duty->pwm3 = PWM_BASE_F + pid_date_pitch->pid_in_control - 
									 pid_date_roll->pid_in_control - pid_date_yaw->pid_in_control;
	
	pwm_duty->pwm4 = PWM_BASE_F + pid_date_pitch->pid_in_control + pid_date_roll->pid_in_control;

	TIM_SetCompare(TIM2, PWM_CH1, pwm_duty->pwm1);
	TIM_SetCompare(TIM2, PWM_CH2, pwm_duty->pwm2);
	TIM_SetCompare(TIM2, PWM_CH3, pwm_duty->pwm3);
	TIM_SetCompare(TIM2, PWM_CH4, pwm_duty->pwm4);

}

void pid_init(void)
{
 PID_CASCADE_DATE_Init(pid_date_pitch);
 PID_CASCADE_DATE_Init(pid_date_roll);
 PID_CASCADE_DATE_Init(pid_date_yaw);
}

