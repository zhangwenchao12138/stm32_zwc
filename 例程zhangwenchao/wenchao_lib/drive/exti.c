/**
  ******************************************************************************
  * @file    exti.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.22
  * @note    �˳���ΪARM��EXTI�ĵײ��̣������ڹٷ��ײ��Ķ��η�װ��
  ******************************************************************************
  */

	
#include "sysinc.h"

static GPIO_CallBackType GPIO_CallBackTable[16] = {NULL};


 /**
 * @brief   		�ⲿ�жϿ��ٳ�ʼ������Ĭ���ж�ʹ���Լ��½��ش���
 * @code
 *      				// ��ʼ��extiA��pin10�������ȼ�preepri=2,��Ӧ���ȼ�subpri=2,ʹ���ж�ITsta=ENABLE
 *      				EXTI_QuickInit(HW_EXTIA, EXTI_Pin_10, 2, 2);
 * @endcode
 * @param[in]   instance ģ���
 *              @arg HW_EXTIA A�˿�
 *              @arg HW_EXTIB B�˿�
 *              @arg HW_EXTIC C�˿�
 *              @arg HW_EXTID D�˿�
 *              @arg HW_EXTIE E�˿�
 * @param[in]   EXTI_Pin_x ��xΪGPIO���źţ���ѡֵ 0,1,2....15
 * @param[in]   preepri ��ռ���ȼ�����ѡֵ0,1,2,3
 * @param[in]   subpri  ��Ӧ���ȼ�����ѡֵ0,1,2,3
 */
void EXTI_QuickInit(int instance, int EXTI_Pin_x,int preepri, int subpri)
{	
		int EXTIx_IRQn;
	
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_QuickInit(instance, 0x01 << EXTI_Pin_x, GPIO_Mode_IPU);
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);							//ʹ�ܸ��ù���ʱ��

  	GPIO_EXTILineConfig(instance, EXTI_Pin_x); 

  	EXTI_InitStructure.EXTI_Line    = 0x01 << EXTI_Pin_x;	
  	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	                      					//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	  switch(EXTI_Pin_x)
		{
			case EXTI_Pin_0: EXTIx_IRQn = EXTI0_IRQn;  break;
			case EXTI_Pin_1: EXTIx_IRQn = EXTI1_IRQn;  break;
			case EXTI_Pin_2: EXTIx_IRQn = EXTI2_IRQn;  break;
			case EXTI_Pin_3: EXTIx_IRQn = EXTI3_IRQn;  break;
			case EXTI_Pin_4: EXTIx_IRQn = EXTI4_IRQn;  break;
			case EXTI_Pin_5: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_6: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_7: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_8: EXTIx_IRQn = EXTI9_5_IRQn;  break;			
			case EXTI_Pin_9: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_10: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_11: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_12: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_13: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_14: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_15: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			default :		break ;
		}

		NVIC_InitStructure.NVIC_IRQChannel = EXTIx_IRQn;		  										//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preepri;						//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subpri;										//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 	
}




 /**
 * @brief  		  �ж�ʹ�ܻ�ʧ�ܳ���
 * @code
 *     					// ��ʼ��extiA��pin10�������ȼ�preepri=2,��Ӧ���ȼ�subpri=2,ʹ���ж�ITsta=ENABLE
 *      				EXTI_QuickInit(HW_EXTIA, EXTI_Pin_10, 2, 2);
 * @endcode
 * @param[in]   instance ģ���
 *              @arg HW_EXTIA A�˿�
 *              @arg HW_EXTIB B�˿�
 *              @arg HW_EXTIC C�˿�
 *              @arg HW_EXTID D�˿�
 *              @arg HW_EXTIE E�˿�
 * @param[in]   EXTI_Pin_x ��xΪGPIO���źţ���ѡֵ 0,1,2....15
 * @param[in]   ITsta   ʹ��/ʧ���жϣ���ѡֵENABLE��DISABLE
 */
void EXTI_IRQcmd(int instance, int EXTI_Pin_x,FunctionalState ITsta)
{		
	  int EXTIx_IRQn;
		NVIC_InitTypeDef NVIC_InitStructure;
		switch(EXTI_Pin_x)
		{
			case EXTI_Pin_0: EXTIx_IRQn = EXTI0_IRQn;  break;
			case EXTI_Pin_1: EXTIx_IRQn = EXTI1_IRQn;  break;
			case EXTI_Pin_2: EXTIx_IRQn = EXTI2_IRQn;  break;
			case EXTI_Pin_3: EXTIx_IRQn = EXTI3_IRQn;  break;
			case EXTI_Pin_4: EXTIx_IRQn = EXTI4_IRQn;  break;
			case EXTI_Pin_5: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_6: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_7: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_8: EXTIx_IRQn = EXTI9_5_IRQn;  break;			
			case EXTI_Pin_9: EXTIx_IRQn = EXTI9_5_IRQn;  break;
			case EXTI_Pin_10: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_11: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_12: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_13: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_14: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			case EXTI_Pin_15: EXTIx_IRQn = EXTI15_10_IRQn ;  break;
			default :		break ;
		}
		NVIC_InitStructure.NVIC_IRQChannel = EXTIx_IRQn;		  					//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ITsta;									//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 	
}




 /**
 * @brief  		  �жϻص�����
 * @param[in]   EXTI_Pin_x ��xΪGPIO���źţ���ѡֵ 0,1,2....15
 * @param[in]   AppCBFun  ������ַ
 */
void EXTI_CallbackInstall(int EXTI_Pin_x, GPIO_CallBackType AppCBFun)
{
    /* init moudle */
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[EXTI_Pin_x] = AppCBFun;
    }
}


/**
 * @brief  ϵͳ�жϺ������û�����ʹ�ã�ֻ������ļ���ͷ��ע�ắ�����ɡ�
 */
void EXTI0_IRQHandler(void)
{
if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
	  if(GPIO_CallBackTable[0] !=NULL)GPIO_CallBackTable[0]();
		EXTI_ClearITPendingBit(EXTI_Line0);  
	}
}
 

void EXTI1_IRQHandler(void)
{	

if(EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		if(GPIO_CallBackTable[1] !=NULL) {GPIO_CallBackTable[1]();}
		EXTI_ClearITPendingBit(EXTI_Line1);  
	}
}

void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
	  if(GPIO_CallBackTable[2] !=NULL) GPIO_CallBackTable[2]();
		EXTI_ClearITPendingBit(EXTI_Line2);  
	}
}

void EXTI3_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
	 if(GPIO_CallBackTable[3] !=NULL) GPIO_CallBackTable[3]();
		EXTI_ClearITPendingBit(EXTI_Line3);  
	}
}

void EXTI4_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
	  if(GPIO_CallBackTable[4] !=NULL)GPIO_CallBackTable[4]();
		EXTI_ClearITPendingBit(EXTI_Line4); 
	}
}
 

void EXTI9_5_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line5) == SET)
	{
	  if(GPIO_CallBackTable[5] !=NULL)GPIO_CallBackTable[5]();
		EXTI_ClearITPendingBit(EXTI_Line5);  
	}
	else if(EXTI_GetITStatus(EXTI_Line6) == SET)
	{
	  if(GPIO_CallBackTable[6] !=NULL)GPIO_CallBackTable[6]();
		EXTI_ClearITPendingBit(EXTI_Line6); 
	}
	else if(EXTI_GetITStatus(EXTI_Line7) == SET)
	{
	  if(GPIO_CallBackTable[7] !=NULL)GPIO_CallBackTable[7]();
		EXTI_ClearITPendingBit(EXTI_Line7);  
	}
	else if(EXTI_GetITStatus(EXTI_Line8) == SET)
	{
	  if(GPIO_CallBackTable[8] !=NULL)GPIO_CallBackTable[8]();
		EXTI_ClearITPendingBit(EXTI_Line8);   
	}
	else if(EXTI_GetITStatus(EXTI_Line9) == SET)
	{
	  if(GPIO_CallBackTable[9] !=NULL)GPIO_CallBackTable[9]();
		EXTI_ClearITPendingBit(EXTI_Line9);  
	}
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) == SET)
	{
	  if(GPIO_CallBackTable[10] !=NULL)GPIO_CallBackTable[10]();
		EXTI_ClearITPendingBit(EXTI_Line10);   
	}
	else if(EXTI_GetITStatus(EXTI_Line11) == SET)
	{
	  if(GPIO_CallBackTable[11] !=NULL)GPIO_CallBackTable[11]();
		EXTI_ClearITPendingBit(EXTI_Line11);  
	}
	else if(EXTI_GetITStatus(EXTI_Line12) == SET)
	{
	  if(GPIO_CallBackTable[12] !=NULL)GPIO_CallBackTable[12]();
		EXTI_ClearITPendingBit(EXTI_Line12);   
	}
	else if(EXTI_GetITStatus(EXTI_Line13) == SET)
	{
	  if(GPIO_CallBackTable[13] !=NULL)GPIO_CallBackTable[13]();
		EXTI_ClearITPendingBit(EXTI_Line13);   
	}
	else if(EXTI_GetITStatus(EXTI_Line14) == SET)
	{
	  if(GPIO_CallBackTable[14] !=NULL)GPIO_CallBackTable[14]();
		EXTI_ClearITPendingBit(EXTI_Line14);  
	}
	else if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
	  if(GPIO_CallBackTable[15] !=NULL)GPIO_CallBackTable[15]();
		EXTI_ClearITPendingBit(EXTI_Line15);  
	}
}
