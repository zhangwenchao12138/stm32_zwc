/**
  ******************************************************************************
  * @file    iwdg.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.21
  * @note    此程序为ARM中IWDG的底层编程（依附于官方底层库封装）
  ******************************************************************************
  */
#include "iwdg.h"

/**
 * @brief  		 初始化独立看门狗
 * @code
 *      			 //看门狗100ms
 *      			 WDOG_QuickInit(100); 
 * @endcode
 * @param[in]  timeInMs 范围10——10000Ms
 * @param[in]  时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
 * @retval     None
 */
void IWDG_QuickInit(int timeInMs)
{	
	int rlr;
	rlr = (int)(timeInMs *10/32);
  IWDG->KR = IWDG_WriteAccess_Enable;  																	//使能对寄存器IWDG_PR和IWDG_RLR的写操作
	IWDG->PR = IWDG_Prescaler_32;       																  //设置IWDG预分频值:设置IWDG预分频值为64
	IWDG->RLR = rlr;
	IWDG->KR = ((uint16_t)0xAAAA);																				//按照IWDG重装载寄存器的值重装载IWDG计数器
}

/**
 * @brief   			喂独立看门狗
 * @retval 				None
 */
void IWDG_Feed(void)
{   
	IWDG->KR = ((uint16_t)0xAAAA);																				//按照IWDG重装载寄存器的值重装载IWDG计数器
}


/**
 * @brief   	使能独立看门狗
 * @retval 		None
 */
void IWDG_ENABLE(void)
{
	IWDG->KR = ((uint16_t)0xCCCC);  																			//使能IWDG
}

