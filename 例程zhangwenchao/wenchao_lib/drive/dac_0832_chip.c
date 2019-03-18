/**
  ******************************************************************************
  * @file    DAC_0832_CHIP.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.24
  * @note    此程序为ARM中的应用编程
  ******************************************************************************
  */

#include "sysinc.h"

void Chip_0832_Port_Init(void)
{
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_1, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_8, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP);
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_12, GPIO_Mode_Out_PP);
}

void Write_0832_Date(u8 date)
{
	PAout(1) = 1;
	PAout(2) = 1;
	PAout(3) = 1;
	PAout(6) = 1;
	PAout(7) = 1;
	PAout(8) = 0;
	PAout(11) = 0;
	PAout(12) = 0;
}

void Write_0832_V(float V)
{
	int tep;
	tep = (int)(V* DAC0832_v_TO_date+0.5);
	Write_0832_Date(tep);
}
