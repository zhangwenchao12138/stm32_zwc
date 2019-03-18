/**
  ******************************************************************************
  * @file    飞行器机体总程序
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.5
  * @note    此程序为ARM的应用编程
  ******************************************************************************
  */


#include "sysinc.h"

#define ADC_V       0.000805664                        //adc读出的值转化为电压值
#define ADC_V_MIN   3.2                                //低电量警报值
#define ACCELERATOR_MAX   1000
#define PWM_DUTY_MAX      1000

u8 txbuf[33]={0};                                      //nrf2401需要发送的数组
u8 rxbuf[33]={0};

u16 adc_date=4,update_adc_flag = 0;										 //adc允许更新标志
int tep_adc;
int adc_jud=0;
short accelerator = 0;                                 //油门
u8 real_attitude_flag=0;
u8 real_adc_flag=0;
u8 real_pitch_pid_flag=0;
u8 real_roll_pid_flag=0;

u16 update_pitch_roll_flag=0,update_yaw_flag=0;
u16 i=0;
u16 k=0,j=0;
HMC_DATE hmc_date[1]={0};
MPU_DATE mpu_date[1]={0};

DIRECTION_FLY direction_fly[1]={0};

FLYING_ANGLE fly_angle[1]={0};

KALMAN_SAME_PARAMETER kalman_pitch[1]={0};
KALMAN_SAME_PARAMETER kalman_roll[1]={0};

FIRST_COMPLEMENTARY_PARAMETER first_comp[1]={0};

PID_CASCADE_DATE pid_date_pitch[1]={0};
PID_CASCADE_DATE pid_date_roll[1]={0};
PID_CASCADE_DATE pid_date_yaw[1]={0};

PWM_DUTY pwm_duty[1]={0};

//API
void system_init(void);
void hardware_init(void);
void varible_init(void);
void pwm_init(void);
void callback_fun(void);
void update_yaw_pid(void);
void update_pitch_roll_pid(void);
void update_control(void);
void update_adc(void);
void real_attitude_adc(void);
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
		update_control();
		update_adc();
	  real_attitude_adc();
		//k++;
  	//printf("%d,%d\n",k,j);
	}
}


 /**
 * @brief   	  控制更新函数
 */
void update_control(void)
{
		if(update_pitch_roll_flag >= 1 )
		{
			update_pitch_roll_flag = 0;
			Get_kalman_Pitch(kalman_pitch,mpu_date,fly_angle);
      Get_kalman_Roll(kalman_roll,mpu_date,fly_angle);
			if(update_yaw_flag >=10)
			{
			update_yaw_flag = 0;
			Get_firstcomp_Yaw(first_comp,mpu_date, hmc_date,fly_angle);
			update_yaw_pid();
			update_adc_flag++;
		}
			update_pitch_roll_pid();
		}		
}


 /**
 * @brief   	  电源测量更新
 */
void update_adc(void)
{
		if(update_adc_flag == 1)
		{
			 update_adc_flag = 0;
			 adc_jud++;
			 if(adc_jud%4 == 0)
			 {
					tep_adc  += Get_Adc_Average(ADC_CH4,5);
					if(adc_jud >= 20)
					{
						adc_jud = 0;
						adc_date = (tep_adc/5)*ADC_V;
						tep_adc = 0;
						update_adc_flag = 0;
						if(adc_date < ADC_V_MIN)
						{
								PAout(12) = !PAout(12);
								txbuf[0] = 5;
								txbuf[1] = 8;
						  	NRF24L01_TxPacket(SPIcs, txbuf);
						}
					}
		   }
		}
}

 /**
 * @brief   	  航向角更新
 */
void update_yaw_pid(void)
{
	PID_Cascade(0,fly_angle->yaw, pid_date_yaw);
}


 /**
 * @brief   	  俯仰角和横滚角更新
 */
void update_pitch_roll_pid(void)
{
	PID_Cascade(direction_fly->east_west_fly,fly_angle->pitch, pid_date_pitch);
	PID_Cascade(direction_fly->south_north_fly,fly_angle->roll, pid_date_roll);
	
	if(accelerator >= ACCELERATOR_MAX){accelerator = ACCELERATOR_MAX;}
	pwm_duty->pwm1 = PWM_BASE_F - pid_date_pitch->pid_in_control - accelerator +
									 pid_date_roll->pid_in_control + pid_date_yaw->pid_in_control;
	
	pwm_duty->pwm2 = PWM_BASE_F - pid_date_pitch->pid_in_control - accelerator - pid_date_roll->pid_in_control;
	
	pwm_duty->pwm3 = PWM_BASE_F + pid_date_pitch->pid_in_control - accelerator -
									 pid_date_roll->pid_in_control - pid_date_yaw->pid_in_control;
	
	pwm_duty->pwm4 = PWM_BASE_F + pid_date_pitch->pid_in_control - accelerator + pid_date_roll->pid_in_control;
	
  if(pwm_duty->pwm1 >= PWM_DUTY_MAX){pwm_duty->pwm1 = PWM_DUTY_MAX;}
	if(pwm_duty->pwm2 >= PWM_DUTY_MAX){pwm_duty->pwm2 = PWM_DUTY_MAX;}
	if(pwm_duty->pwm3 >= PWM_DUTY_MAX){pwm_duty->pwm3 = PWM_DUTY_MAX;}
	if(pwm_duty->pwm4 >= PWM_DUTY_MAX){pwm_duty->pwm4 = PWM_DUTY_MAX;}
	
	TIM_SetCompare(TIM2, PWM_CH1, pwm_duty->pwm1);
	TIM_SetCompare(TIM2, PWM_CH2, pwm_duty->pwm2);
	TIM_SetCompare(TIM2, PWM_CH3, pwm_duty->pwm3);
	TIM_SetCompare(TIM2, PWM_CH4, pwm_duty->pwm4);
}


 /**
 * @brief   	  nrf2401判别函数，决定是否开启姿态实时传送
 */
void real_attitude_adc(void)
{
	if(real_attitude_flag == 1)
	{
		txbuf[0] = 2;
		txbuf[1] = fly_angle->pitch;
		txbuf[2] = fly_angle->roll;
		txbuf[3] = fly_angle->yaw;
	}
	if(real_adc_flag == 1)
	{
		txbuf[0] = 1;
		txbuf[1] = (short)(adc_date*10);
	}
	if(real_pitch_pid_flag == 1)
	{
		txbuf[0] = 3;
		txbuf[1] = pid_date_pitch->pid_out_p;
		txbuf[2] = pid_date_pitch->pid_out_i;
		txbuf[3] = pid_date_pitch->pid_out_d;
		txbuf[4] = pid_date_pitch->pid_in_p;
		txbuf[5] = pid_date_pitch->pid_in_i;
		txbuf[6] = pid_date_pitch->pid_in_d;
	}
	if(real_roll_pid_flag == 1)
	{
		txbuf[0] = 4;
		txbuf[1] = pid_date_roll->pid_out_p;
		txbuf[2] = pid_date_roll->pid_out_i;
		txbuf[3] = pid_date_roll->pid_out_d;
		txbuf[4] = pid_date_roll->pid_in_p;
		txbuf[5] = pid_date_roll->pid_in_i;
		txbuf[6] = pid_date_roll->pid_in_d;
	}

}


 /**
 * @brief   	  定时器3的中断回调函数
 */
void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{		
			if(i++ >= 90){ PCout(13)=!PCout(13);i=0;}
			Six_Axis_Get(mpu_date);
			update_pitch_roll_flag = 1;
			update_yaw_flag++;
			if(update_yaw_flag >=10)
			{
				Sthree_Axis_Get(hmc_date);
							j++;
			}
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
		}
}


 /**
 * @brief   	  外部中断6的中断回调函数（用于接收nrf2401）
 */
void exti6(void)
{ 			
	NRF24L01_RxPacket(SPIcs, rxbuf);
	switch(rxbuf[0])
	{
		case ACCELERATOR: accelerator = 8*rxbuf[1];break;
		case HOVER_FLY	 :if(rxbuf[1]==1){hover_flying(direction_fly);}	  break;					
		case EAST_FLY	: if(rxbuf[1]==1){east_flying(direction_fly);}      break;
    case WEST_FLY	: if(rxbuf[1]==2){west_flying(direction_fly);}      break;
    case SOUTH_FLY	:	if(rxbuf[1]==3){south_flying(direction_fly);}		break;
    case NORTH_FLY : if(rxbuf[1]==4){north_flying(direction_fly);}    break;
		case START_REAL_ATTITUDE : if(rxbuf[1]==12){real_attitude_flag=1;} else {real_attitude_flag=0;}break;
		case GET_POWER_VAL:if(rxbuf[1]==13){real_adc_flag=1;} else {real_adc_flag=0;}                  break;    	  	 
		case GET_PITCH_PID:if(rxbuf[1]==14){real_pitch_pid_flag=1;} else {real_pitch_pid_flag=0;}      break;
		case GET_ROLL_PID :if(rxbuf[1]==15){real_roll_pid_flag=1;} else {real_roll_pid_flag=0;}        break;
		case UPDATE_OUT_P	:	update_out_p();break;
		case UPDATE_OUT_I	:	update_out_i();break;
		case UPDATE_OUT_D	:	update_out_d();break;
		case UPDATE_IN_P	: update_in_p(); break;
		case UPDATE_IN_I	: update_in_i(); break;
		case UPDATE_IN_D	: update_in_d(); break;	 
    case  NIGHT_LIGHT: ; break;
		default : break;
	}
	rxbuf[0]=0;
}


 /**
 * @brief   	  系统初始化
 */
void system_init(void)
{
	NVIC_init();
	SYSTICK_DelayInit();
}


 /**
 * @brief   	  硬件初始化
 */
void hardware_init(void)
{
	
	UART_QuickInit(HW_UART1, 19200, 2, 2, ENABLE);
	
	IIC_Init();
	
	HMC5883_Init();
		 
	MPU6050_Init();
	
	NRF24L01_Init(Spics); 
	
	ADC_init(ADC_CH4);
	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_12, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP);
	
		PCout(13) = 1;
		PAout(11) = 0;
		PAout(12) = 0;

  TIM_ms_Init(TIM3, 10, TIM_IT_Update,2, ENABLE);	
}


 /**
 * @brief   	  变量初始化
 */
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

 /**
 * @brief   	  脉宽初始化
 */
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


 /**
 * @brief   	  中断回调函数集中注册
 */
void callback_fun(void)
{
	 	TIMER_CallbackInstall(HW_TIMER3, timer3);
		EXTI_CallbackInstall(EXTI_Pin_6, exti6);
}





