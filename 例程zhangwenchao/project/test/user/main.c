#include "sysinc.h"

u8 txbuf[33]={1,7,3,99};
u8 rxbuf[33]={0};

u16 adc_date;

HMC_DATE hmc_date[1]={0};
MPU_DATE mpu_date[1]={0};

FLYING_ANGLE fly_angle[1]={0};

KALMAN_SAME_PARAMETER kalman_pitch[1]={0};
KALMAN_SAME_PARAMETER kalman_roll[1]={0};

FIRST_COMPLEMENTARY_PARAMETER first_comp[1]={0};

PID_CASCADE_DATE pid_date_pitch[1]={0};
PID_CASCADE_DATE pid_date_roll[1]={0};
PID_CASCADE_DATE pid_date_yaw[1]={0};

PWM_DUTY pwm_duty[1]={0};

void system_init(void);
void hardware_init(void);
void varible_init(void);
void pwm_init(void);
void callback_fun(void);
void update_pid(void);
void update_adc(void);
void timer3(void);
void exti6(void);


int main(void)
{		 
	system_init();
	hardware_init();
	varible_init();
	pwm_init();
	callback_fun();
  while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
}




void update_adc(void)
{
	adc_date=Get_Adc_Average(ADC_CH4,10);
}

void update_pid(void)
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


void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{
			Nine_Axis_Get(mpu_date,hmc_date);
			Get_kalman_Pitch(kalman_pitch,mpu_date,fly_angle);
      Get_kalman_Roll(kalman_roll,mpu_date,fly_angle);
			Get_firstcomp_Yaw(first_comp,mpu_date, hmc_date,fly_angle);
			//printf(" pitch = %f    %f     %f      \r\n",fly_angle->yaw,fly_angle->pitch,fly_angle->roll);			
			update_pid();			
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
		}
}


void exti6(void)
{ 			
	NRF24L01_RxPacket(SPIcs, rxbuf);
	printf(" rx =  %d\n", rxbuf[3]);
}




void system_init(void)
{
	NVIC_init();
	SYSTICK_DelayInit();
}


void hardware_init(void)
{	
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);

	IIC_Init();
	
	HMC5883_Init();
		 
	MPU6050_Init();
	
	NRF24L01_Init(Spics); 
	
	ADC_init(ADC_CH4);
	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_12, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP);
	
  TIM_ms_Init(TIM3, 10, TIM_IT_Update,2, ENABLE);	
}


void varible_init(void)
{
	hmc_date->hmc_offset_x = -4;
	hmc_date->hmc_offset_y = -50;
	hmc_date->hmc_offset_z = -40;
 	 
	MPU6050_Offset(mpu_date);
	
  PID_CASCADE_DATE_Init(pid_date_pitch);
  PID_CASCADE_DATE_Init(pid_date_roll);
  PID_CASCADE_DATE_Init(pid_date_yaw);

	kalman_same_parameter_init(kalman_pitch,10);
	kalman_same_parameter_init(kalman_roll,10);
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

void callback_fun(void)
{
	 	TIMER_CallbackInstall(HW_TIMER3, timer3);
		EXTI_CallbackInstall(EXTI_Pin_6, exti6);
}





