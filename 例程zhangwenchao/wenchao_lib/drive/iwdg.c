/**
  ******************************************************************************
  * @file    iwdg.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.21
  * @note    �˳���ΪARM��IWDG�ĵײ��̣������ڹٷ��ײ���װ��
  ******************************************************************************
  */
#include "iwdg.h"

/**
 * @brief  		 ��ʼ���������Ź�
 * @code
 *      			 //���Ź�100ms
 *      			 WDOG_QuickInit(100); 
 * @endcode
 * @param[in]  timeInMs ��Χ10����10000Ms
 * @param[in]  ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
 * @retval     None
 */
void IWDG_QuickInit(int timeInMs)
{	
	int rlr;
	rlr = (int)(timeInMs *10/32);
  IWDG->KR = IWDG_WriteAccess_Enable;  																	//ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	IWDG->PR = IWDG_Prescaler_32;       																  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
	IWDG->RLR = rlr;
	IWDG->KR = ((uint16_t)0xAAAA);																				//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
}

/**
 * @brief   			ι�������Ź�
 * @retval 				None
 */
void IWDG_Feed(void)
{   
	IWDG->KR = ((uint16_t)0xAAAA);																				//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
}


/**
 * @brief   	ʹ�ܶ������Ź�
 * @retval 		None
 */
void IWDG_ENABLE(void)
{
	IWDG->KR = ((uint16_t)0xCCCC);  																			//ʹ��IWDG
}

