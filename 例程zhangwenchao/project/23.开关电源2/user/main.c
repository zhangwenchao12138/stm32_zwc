/**
  ******************************************************************************
  * @file    			开关电源STM32F103C8T6资源分配表
  * @author  			张文超 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.24
  * @note    			一阶滞后滤波平滑ADC值，单环PID控制PWM
  * @config_pwm   timer2
  *               @arg PWM_CH1_fan             PA0
  * @config_adc   adc1
  *               @arg ADC_CH4_input_v         PA4
  *               @arg ADC_CH5_output_A        PA5
  * @config_iic   模拟iic(ssd1306)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_exti  外部中断
  *               @arg exti4 k1 选择_内阻模式   PB4
  *               @arg exti5 k2 调高            PB5
  *               @arg exti6 k3 调低            PB6
  *               @arg exti7 k4 0.27            PB7
  *               @arg exti8 k5 2.6             PB8
  *               @arg exti9 k6 5               PB9
  * @config_led   指示灯
  *               @arg 运行指示灯              PC13
  * @config_dac   dac0832
  *               @arg dat0                    PA1
  *               @arg dat1                    PA2
  *               @arg dat2                    PA3
  *               @arg dat3                    PA6
  *               @arg dat4                    PA7
  *               @arg dat5                    PA8
  *               @arg dat6                    PA11
  *               @arg dat7                    PA12
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44号引脚     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */


#include "sysinc.h"

#define ADC_tans_num  0.0008057
#define ADC2_res      0.25
#define TRANS_V       11

void init(void);
void adc_init(void);
void timer3(void);
void update_adc(void);
void update_dac(void);
void updat_c(void);
void display(void);
void open_v(void);
float test_in_res(void);
void exti4(void);
void exti5(void);
void exti6(void);
void exti7(void);
void exti8(void);
void exti9(void);
void update_fan_pwm(void);
void A_protect(void);
void V_protect(void);

float num[2]={0};
u8 flag_led = 0;
float target_A = 0;
float val_dac = 0;
float c = 0;
float open_vol;
float val_inres;
float pwm_fan = 0;
int pwm = 3000;
int pwm2 = 70;

int main()
{
	NVIC_init();
 	SYSTICK_DelayInit();
	init();
	open_v();
	while(1)
	{
		 target_A = 1;
		SYSTICK_DelayMs(10);	
				TIM_SetCompare(TIM2, PWM_CH1,4000);

	}
}

void init(void)
{
	adc_init();
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
//	Chip_0832_Port_Init();	
	IIC_Init();
	OLED_Init();//ssd1306
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_4, 1, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_5, 3, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_6, 3, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_7, 1, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_8, 1, 1);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_9, 1, 1);
	EXTI_CallbackInstall(EXTI_Pin_4, exti4);
	EXTI_CallbackInstall(EXTI_Pin_5, exti5);
	EXTI_CallbackInstall(EXTI_Pin_6, exti6);
	EXTI_CallbackInstall(EXTI_Pin_7, exti7);
	EXTI_CallbackInstall(EXTI_Pin_8, exti8);
	EXTI_CallbackInstall(EXTI_Pin_9, exti9);
	TIM_PWM_Init(TIM2, PWM_CH1, 15000, ENABLE);
	TIM_SetCompare(TIM2, PWM_CH1,0);
	TIM_PWM_Init(TIM2, PWM_CH2, 15000, ENABLE);
	TIM_SetCompare(TIM2, PWM_CH2,0);

	TIM_ms_Init(TIM3, 30, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
}

void adc_init(void)
{
	ADC_init(ADC_CH4);
	ADC_init(ADC_CH5);
}

void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)   
	{	
		update_adc();
		update_dac();
		updat_c();
    //update_fan_pwm();

		if(flag_led >= 20){display(); PCout(13) = ~PCout(13); flag_led = 0;}
		flag_led++;
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);   
	}
}

void update_adc(void)
{
	num[0] = Get_Adc_Average(ADC_CH4, 10);
	num[1] = Get_Adc_Average(ADC_CH5, 10);
	for(int i=0; i<2; i++)
	{
		num[i]*=ADC_tans_num;
		//OLED_P6x8Str_Num(2,3,"@input_V:",'V',(num[0]));
	}
}

void update_dac(void)
{
	//pwm2 += 0.5*(target_A - num[1]/ADC2_res)*1000;
	//TIM_SetCompare(TIM2, PWM_CH2,pwm2);
	TIM_SetCompare(TIM2, PWM_CH2,pwm2);
}

void updat_c(void)
{
	c = c + num[0]/ADC2_res*0.03;
}

void open_v(void)
{
	target_A = 0;
  val_dac = 0;
	SYSTICK_DelayMs(100);
	open_vol = ADC_tans_num * (float)( Get_Adc_Average(ADC_CH4, 10));
}

void display(void)
{
  OLED_P6x8Str_Num(2,1,"@GOAL_A:",'A', target_A);
  OLED_P6x8Str_Num(2,2,"@open_ V:",'V', open_vol*11);
  OLED_P6x8Str_Num(2,3,"@input_V:",'V',(num[0]*11));
  OLED_P6x8Str_Num(2,4,"@input_A:",'A',(num[1]/ADC2_res));
  OLED_P6x8Str_Num(2,5,"@input_C:",'C',(c/ADC2_res));
  OLED_P6x8Str_Num(2,6,"@in_res:on_test k1",' ', 0);
  OLED_P6x8Str_Num(0,7," ",' ', 0);
}

void update_fan_pwm(void)
{
	pwm_fan = ((num[1]/ADC2_res) * num[0] * TRANS_V) ;
	if(pwm_fan > 3)
	{
		pwm = 1000 + (int)(pwm_fan*500);
		TIM_SetCompare(TIM2, PWM_CH1,pwm);
	}
}

float test_in_res(void)
{
	float tep1,tep2,tep1_1,tep2_2;
	val_dac = 0;
	tep1_1 = ADC_tans_num * (float)Get_Adc_Average(ADC_CH5, 10);
	tep1 = ADC_tans_num * (float)Get_Adc_Average(ADC_CH4, 10);
	val_dac = 0.5;
	tep2 = ADC_tans_num * (float)Get_Adc_Average(ADC_CH4, 10);
	tep2_2 = ADC_tans_num * (float)Get_Adc_Average(ADC_CH5, 10);
	
	val_inres = (tep1 -tep2)/((tep1_1 - tep2_2)/ADC2_res);
	return 0;
}

void exti4(void)
{	
	float tep[3];
	SYSTICK_DelayMs(20);	 
	if(PBin(4) == 0)
	{
		for(int i=0; i<3; i--)
		{
			test_in_res();
			tep[i] = val_inres;
		}
		val_inres = (tep[0] + tep[1] + tep[2])/3.0;
	}
	OLED_P6x8Str_Num(32,6,"@in_res:",'R', val_inres);
}

void exti5(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(5) == 0)
	{
		if(pwm2 <= 1400)
		pwm2 += 100;
		else
			pwm2 += 200;
	}
	OLED_P6x8Str_Num(32,1,"@TARGET_A:",'A', target_A);
}

void exti6(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(6) == 0)
	{
		if(pwm2 <= 1400)
		pwm2 -= 100;
		else
		pwm2 -= 200;
	}
	OLED_P6x8Str_Num(32,1,"@TARGET_A:",'A', target_A);
}

void exti7(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(6) == 0)
	{
	TIM_SetCompare(TIM2, PWM_CH2,70);
	}
	OLED_P6x8Str_Num(32,1,"@TARGET_A:",'A', target_A);

}

void exti8(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(6) == 0)
	{
	TIM_SetCompare(TIM2, PWM_CH2,4500);
	}
	//OLED_P6x8Str_Num(32,1,"@TARGET_A:",'A', target_A);
}

void exti9(void)
{
	SYSTICK_DelayMs(20);	 
	if(PBin(6) == 0)
	{
		target_A = 5;
	}
	OLED_P6x8Str_Num(32,1,"@TARGET_A:",'A', target_A);
}

void A_protect(void)
{
	if(num[1]/ADC2_res >= 5)
	{
		
	}
}
void V_protect(void);

