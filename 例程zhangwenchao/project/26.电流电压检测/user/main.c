#include "sysinc.h"

void voltage_detection_init(void);
float get_power_voltage(void);
float get_res_front_voltage(void);
float get_output_voltage(void);
float get_output_electric_current(void);

 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	 	IIC_Init();

	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	 
	voltage_detection_init();

	while(1)
	{		
		printf("adc val: %f\n",get_power_voltage());
		printf("adc val: %f\n",get_res_front_voltage());
		printf("adc val: %f\n",get_output_voltage());
		printf("adc val: %f\n\n",get_output_electric_current());

		SYSTICK_DelayMs(250);	 //延时300ms
		PCout(13)=	!PCout(13);	
	}
 }
 
 
void voltage_detection_init(void){
	ADC_init(ADC_CH3);    //测检测电源电压
	ADC_init(ADC_CH4);    //检测测电流电阻前端电压
	ADC_init(ADC_CH5);    //检测输出电压
}


float get_power_voltage(void){
	int adcx=Get_Adc_Average(ADC_CH3,10);
	float temp=(float)adcx*(3.3/4096);
	return temp;
}

float get_res_front_voltage(void){
	int adcx=Get_Adc_Average(ADC_CH4,10);
	float temp=(float)adcx*(3.3/4096);
	return temp;
}

float get_output_voltage(void){
	int adcx=Get_Adc_Average(ADC_CH5,10);
	float temp=(float)adcx*(3.3/4096);
	return temp;
}

float get_output_electric_current(void){
	int adcxdif=Get_Adc_Average(ADC_CH5,10) - Get_Adc_Average(ADC_CH4,10);
	return adcxdif;
}
