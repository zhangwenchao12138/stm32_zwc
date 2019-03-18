/**
  ******************************************************************************
  * @file    			开关电源STM32F103C8T6资源分配表
  * @author  			张文超 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.16
  * @note    			一阶滞后滤波平滑ADC值，单环PID控制PWM
  * @config_pwm   timer2
  *               @arg PWM_CH1_buck            PA0
  *               @arg PWM_CH2_boost           PA1
  * @config_ctl   ctl_boost
  *               @arg ctl_boost               PA11
  * @config_adc   adc1
  *               @arg ADC_CH4_input_v         PA4
  *               @arg ADC_CH5_output_v        PA5
  *               @arg ADC_CH6_output_A        PA6
  * @config_iic   模拟iic(ssd1306)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_exti  外部中断
  *               @arg exti3 模式              PB4
  *               @arg exti4 选择              PB4
  *               @arg exti5 调高              PB5
  *               @arg exti6 调低              PB6
  * @config_led   指示灯
  *               @arg 运行指示灯              PC13
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44号引脚     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */


#include "sysinc.h"

#define ADC_tans_num  0.012085

FIRST_ORDER_PARAMETER first_order[3][1]={0};
PID_DATE pid_date[2][1]={0};

float num[3]={0};
float flag_led=0;
short target_v = 20;
float target_a = 1;
u8 flag_boost_buck = 0;                           // 1:boost 2:buck
u8 flag_screen = 1;                               // 1:实时显示电压值  2:调节电压 3:调节电流
//u8 flag_v_a = 1;                                  // 1:电压模式  2:电流模式

void display(void);
void timer3(void);
void adc_init(void);
void update_adc(void);
void pwm_init(void);
void update_pwm(void);
void pid_init(void);
void update_pid(void);
void judge_flag(void);
void exti4(void);
void exti5(void);
void exti6(void);

int main()
{

	NVIC_init();
 	SYSTICK_DelayInit();
  IIC_Init();
	OLED_Init();
  adc_init();
	pwm_init();
	pid_init();
	TIM_us_Init(TIM3, 30000, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP); 
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_4, 1, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_5, 1, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_6, 1, 1);
	EXTI_CallbackInstall(EXTI_Pin_4, exti4);
	EXTI_CallbackInstall(EXTI_Pin_5, exti5);
	EXTI_CallbackInstall(EXTI_Pin_6, exti6);
	PAout(11) = 0;
	display();
	while(1)
	{
		SYSTICK_DelayMs(10);	
	}
}


void exti4(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(4) == 0)
	{
		flag_screen++;
		if(flag_screen >=4)
		{
			flag_screen = 1;
		}
	}		
	if(flag_screen == 1)
	{
		OLED_CLS();
		display();
	}
	else if(flag_screen == 2)
	{
		OLED_CLS();
		
		for(int i = 0; i<=4; i++)
	  {
			OLED_P16x16Ch(22+i*16,0,i);
	  }
		OLED_P6x8Str(36, 3, "CHANGE_V");
		OLED_P6x8Str_Num(16,5,"TARGET_V:",'V',target_v);
		OLED_P6x8Str(0, 6, "+");
		OLED_P6x8Str(120, 6, "-");
	}
	else if(flag_screen == 3)
	{
		
		OLED_CLS();
		for(int i = 0; i<=4; i++)
	  {
			OLED_P16x16Ch(22+i*16,0,i);
	  }		
		OLED_P6x8Str(36, 3, "CHANGE_A");
		OLED_P6x8Str_Num(16,5,"TARGET_A:",'A',target_a);
		OLED_P6x8Str(0, 6, "+");
		OLED_P6x8Str(120, 6, "-");
	}
}

void exti5(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(5) == 0)
	{
		if(flag_screen == 3){target_a += 0.1;OLED_P6x8Str_Num(20,5,"VALUE_A:",'A',target_a);}
		else if(flag_screen == 2){target_v += 1;OLED_P6x8Str_Num(20,5,"VALUE_V:",'V',target_v);}
	}
}

void exti6(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(6) == 0)
	{
		if(flag_screen == 3){target_a -= 0.1;OLED_P6x8Str_Num(20,5,"VALUE_A:",'A',target_a);}
		else if(flag_screen == 2){target_v -= 1;OLED_P6x8Str_Num(20,5,"VALUE_V:",'V',target_v);}
	}
}

void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)   
	{	
		update_adc();
		update_pid();
		update_pwm();
		judge_flag();
		
		flag_led++;
		if(flag_led >= 50){if(flag_screen ==1){display();} PCout(13) = ~PCout(13); flag_led = 0;}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);   
	}
}

void judge_flag(void)
{
	if(target_v < first_order[0]->num)
	{
		flag_boost_buck =2;
	  PAout(11) = 0;
	}
	else 	if(target_v >= first_order[0]->num)
	{
		flag_boost_buck =1;
		PAout(11) = 1;
	}
}

void update_adc(void)
{
	num[0] = Get_Adc_Average(ADC_CH4, 10);
	num[1] = Get_Adc_Average(ADC_CH5, 10);
	num[2] = Get_Adc_Average(ADC_CH6, 10);
	for(int i=0; i<2; i++)
	{
		num[i]*=ADC_tans_num;
		First_Order_filter(first_order[i], num[i]);
	}
		num[2] = num[2]*T_ANGLE-1.25;
		First_Order_filter(first_order[2], num[2]);
}

void update_pwm(void)
{
	if(flag_boost_buck == 1){TIM_SetCompare(TIM2, PWM_CH1,2000+pid_date[0]->pid_control);}
	else if(flag_boost_buck == 2){TIM_SetCompare(TIM2, PWM_CH2,2000+pid_date[1]->pid_control);}
}

void update_pid(void)
{
	
	if(flag_boost_buck == 1){PID_Control(target_v, first_order[1]->num,pid_date[0]);}
	else if(flag_boost_buck == 2){PID_Control(target_v, first_order[1]->num,pid_date[1]);}
}

void adc_init(void)
{
	ADC_init(ADC_CH4);
	ADC_init(ADC_CH5);
	ADC_init(ADC_CH6);
	for(int i=0;i<3;i++)
	First_Order_parameter_init(first_order[i],0.1);
}

void pwm_init(void)
{
	TIM_PWM_Init(TIM2, PWM_CH1, 15000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH2, 15000, ENABLE);
	TIM_SetCompare(TIM2, PWM_CH1,0);
	TIM_SetCompare(TIM2, PWM_CH2,0);
}

void pid_init(void)
{
	for(int i=0; i<2; i++)
	PID_DATE_Init(pid_date[i]);
}

void display(void)
{
  for(int i = 0; i<=4; i++)
	{
			OLED_P16x16Ch(22+i*16,0,i);
	}
  OLED_P6x8Str_Num(22,3,"@ADC1:",'V',first_order[0]->num);
  OLED_P6x8Str_Num(22,4,"@ADC2:",'V',first_order[1]->num);
  OLED_P6x8Str_Num(22,5,"@ADC3:",'V',first_order[2]->num);
}




