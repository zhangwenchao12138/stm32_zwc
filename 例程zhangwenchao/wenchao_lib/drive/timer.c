/**
  ******************************************************************************
  * @file    timer.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.22
  * @note    �˳���ΪARM�ĵײ��̣������ڹٷ��ײ��Ķ��η�װ��
  ******************************************************************************
  */
	
	
#include "sysinc.h"			

//�����жϻص�����
static TIMER_CallBackType TIMER_CallBackTable[6] = {NULL};



 /**
 * @brief  		  ΢�뼶��ʱ����ʼ��
 * @code
 *     					// ��ʼ����ʱ��2��100us����ʱ�������ж�TIM_IT_Update����Ӧ���ȼ�2��������ʱ��
 *      				TIM_us_Init(TIM2, 100, TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   Tim_us
 * @param[in]   TIM_IT		 �����ж�����     
 *              @arg TIM_IT_Update   ��ʱ�������ж�
 *              @arg TIM_IT_CC1			 ���벶׽�ж�-ͨ��һ
 *              @arg TIM_IT_CC2			 ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC3      ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC4			 ���벶׽�ж�-ͨ����
 * @param[in]   Subpri		 ��ռ���ȼ�   ֵ��0,1,2,3
 * @param[in]   NewState   ʹ��/ʧ�ܶ�ʱ������ѡֵENABLE��DISABLE
 */
u8 TIM_us_Init(TIM_TypeDef* TIMx, u32 Tim_us, u16 TIM_IT, u16 Subpri,FunctionalState NewState)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u16 arr;
	u16 psc;
	IRQn_Type tim_irq;
	u32 num;
	num = Tim_us*72;
	psc = num/20000;
	if(psc <= 1) 
	{
		psc = 0;
		arr = num;
	}
	else if((psc > 1) &&(psc <= 10000))
	{
		psc--;
		arr = num/psc;
	}		
	else if(psc > 10000)
	{
		return 0;
	}
	
	if(TIMx == TIM2)
	{
		tim_irq = TIM2_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	}
	else if(TIMx == TIM3)
	{
		tim_irq = TIM3_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	}
	else if(TIMx == TIM4)
	{
		tim_irq = TIM4_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	}

  TIM_TimeBaseStructure.TIM_Period = arr;                                            //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc;                                         //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                            //����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                        //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);                                    //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIMx,TIM_IT,NewState);                                                //ʹ��ָ����TIM3�ж�,��������ж�
 
	NVIC_InitStructure.NVIC_IRQChannel = tim_irq;                           
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                          //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;                                   
	NVIC_Init(&NVIC_InitStructure);                                                    //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIMx, NewState);                                                           //ʹ��TIMx	
	return  1;
}

 /**
 * @brief  		  ���뼶��ʱ����ʼ��
 * @code
 *     					// ��ʼ����ʱ��2��100ms����ʱ�������ж�TIM_IT_Update����Ӧ���ȼ�2��������ʱ��
 *      				TIM_ms_Init(TIM2, 100, TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   Tim_us
 * @param[in]   TIM_IT		 �����ж�����     
 *              @arg TIM_IT_Update   ��ʱ�������ж�
 *              @arg TIM_IT_CC1			 ���벶׽�ж�-ͨ��һ
 *              @arg TIM_IT_CC2			 ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC3      ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC4			 ���벶׽�ж�-ͨ����
 * @param[in]   Subpri		 ��ռ���ȼ�   ֵ��0,1,2,3
 * @param[in]   NewState   ʹ��/ʧ�ܶ�ʱ������ѡֵENABLE��DISABLE
 */
u8 TIM_ms_Init(TIM_TypeDef* TIMx, u16 Tim_ms, u16 TIM_IT,u16 Subpri, FunctionalState NewState)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u16 arr;
	u16 psc;
	u32 num;
	num = Tim_ms*72000;
	IRQn_Type tim_irq;
	psc = num/30000;
	if(psc <= 1) 
	{
		psc = 0;
		arr = num;
	}
	else if((psc > 1) &&(psc <= 40000))
	{
		psc--;
		arr = num/psc;
	}		
	else if(psc > 20000)
	{
		printf("Tim_ms is too much");
		return 0;
	}
	
	if(TIMx == TIM2)
	{
		tim_irq = TIM2_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	}
	else if(TIMx == TIM3)
	{
		tim_irq = TIM3_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	}
	else if(TIMx == TIM4)
	{
		tim_irq = TIM4_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	}

	
	TIM_TimeBaseStructure.TIM_Period = arr;                
	TIM_TimeBaseStructure.TIM_Prescaler = psc;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);  
 
	TIM_ITConfig(TIMx,TIM_IT,NewState);  

 	NVIC_InitStructure.NVIC_IRQChannel = tim_irq;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;  
	NVIC_Init(&NVIC_InitStructure);   
			
	TIM_ITConfig(TIMx,TIM_IT,NewState);  
	TIM_Cmd(TIMx, NewState);   
	return  1;
}

 /**
 * @brief  		  ��ʱ���ж����ú���
 * @code
 *     					// ��ʼ����ʱ��2, ��ʱ�������ж�TIM_IT_Update����Ӧ���ȼ�2��������ʱ���ж�
 *      				TIM_ITconfig(TIM2,TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   TIM_IT		 �����ж�����     
 *              @arg TIM_IT_Update   ��ʱ�������ж�
 *              @arg TIM_IT_CC1			 ���벶׽�ж�-ͨ��һ
 *              @arg TIM_IT_CC2			 ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC3      ���벶׽�ж�-ͨ����
 *              @arg TIM_IT_CC4			 ���벶׽�ж�-ͨ����
 * @param[in]   Subpri		 ��ռ���ȼ�   ֵ��0,1,2,3
 * @param[in]   NewState   ʹ��/ʧ�ܶ�ʱ������ѡֵENABLE��DISABLE
 */
void TIM_ITconfig(TIM_TypeDef* TIMx, u16 TIM_IT,u16 Subpri, FunctionalState NewState)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	IRQn_Type tim_irq;

	TIM_ITConfig(TIMx,TIM_IT,NewState);  
 	if(TIMx == TIM2)
	{
		tim_irq = TIM2_IRQn;
 	}
	else if(TIMx == TIM3)
	{
		tim_irq = TIM3_IRQn;
 	}
	else if(TIMx == TIM4)
	{
		tim_irq = TIM4_IRQn;
 	}
	NVIC_InitStructure.NVIC_IRQChannel = tim_irq;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;  
	NVIC_Init(&NVIC_InitStructure);   
}

 /**
 * @brief  		  PWM��ʼ��
 * @code
 *     					// ��ʼ����ʱ��2��ͨ��2��Ƶ��1000��������ʱ��
 *      				TIM_PWM_Init(TIM2, PWM_CH2, 1000, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   PWM_CHx    PWMͨ��ѡ��      ֵ��PWM_CH1,PWM_CH2,PWM_CH3,PWM_CH4 
 * @param[in]   Frequency	 PWM��Ƶ��   
 * @param[in]   NewState   ʹ��/ʧ�ܶ�ʱ������ѡֵENABLE��DISABLE
 */
void TIM_PWM_Init(TIM_TypeDef* TIMx, u8 PWM_CHx, u32 Frequency, FunctionalState NewState)
{
	u32 tim;
	u32 tep;
	tim = 72000000/Frequency;
	tep = 72000000%Frequency;
	if(tep >= (Frequency/2))
	{
		tim++;
	}
	
	GPIO_PWM_Init(TIMx, PWM_CHx);
	TIM_tim_Init(TIMx, tim, NewState);
	TIM_OCInit(TIMx, PWM_CHx);
	
	TIM_Cmd(TIMx, NewState);   
}	

 /**
 * @brief  		  PWM����ռ�ձȺ���
 * @code
 *     					// ��ʼ����ʱ��2��ͨ��2��ռ�ձ�20%
 *      					TIM_SetCompare(TIM2, PWM_CH2, 2000);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   PWM_CHx    PWMͨ��ѡ��      ֵ��PWM_CH1,PWM_CH2,PWM_CH3,PWM_CH4 
 * @param[in]   PWM_Duty   ռ�ձ�           ֵ����0��10000�������Ŵ�0%~100%
 */
u8 	TIM_SetCompare(TIM_TypeDef* TIMx, u8 PWM_CHx, u32 PWM_Duty)
{
	if(PWM_Duty > 10000)
	{
		return 0;
	}
	u32 num;
	num = TIMx->ARR ;
	PWM_Duty = PWM_Duty*(num+1)/10000;
	
	switch(PWM_CHx)
	{
		case PWM_CH1: 	TIM_SetCompare1(TIMx,PWM_Duty);   break;
		case PWM_CH2: 	TIM_SetCompare2(TIMx,PWM_Duty);   break;
		case PWM_CH3: 	TIM_SetCompare3(TIMx,PWM_Duty);   break;
		case PWM_CH4: 	TIM_SetCompare4(TIMx,PWM_Duty);   break;
		default : break;
	}
	return  1;
}

 /**
 * @brief  		  ���벶׽��ʼ��
 * @code
 *     					// ��ʼ����ʱ��2��ͨ��2��װ�ؼ���ֵ0xffff��Ԥ��Ƶ0����Ӧ���ȼ�2�������˲�0
 *      					TIM_Cap_Init(TIM2, CAP_CH2, 0xffff,0, 2, 0);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   CAP_CHx    PWMͨ��ѡ��      ֵ��CAP_CH1,CAP_CH2,CAP_CH3,CAP_CH4 
 * @param[in]   arr				 Ԥװֵ           ֵ��0~0xffff
 * @param[in]   psc				 Ԥ��Ƶֵ         ֵ��0~0xffff
 * @param[in]   subpri     ��Ӧ���ȼ���     ֵ��0,1,2,3
 * @param[in]   N       	 �˲�ֵ           ֵ��0~0x0f
 */
void TIM_Cap_Init(TIM_TypeDef* TIMx, u8 CAP_CHx, u16 arr,u16 psc, u16 Subpri, u8 N)
{
	u16 TIM_Channel_x;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

  TIM_Channel_x = GPIO_Cap_Init(TIMx, CAP_CHx);
	TIM_Ctim_Init(TIMx, arr, psc, Subpri, ENABLE);
	switch(CAP_CHx)
			 {case CAP_CH1: TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC1,ENABLE);break;
				case CAP_CH2: TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC2,ENABLE);break;
				case CAP_CH3: TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC3,ENABLE);break;
				case CAP_CH4: TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC4,ENABLE);break;
				default : break; }
	TIM_ITConfig(TIMx,TIM_IT_Update|((TIM_Channel_x>>16) & 0xff),ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_ICInitStructure.TIM_Channel = (TIM_Channel_x & 0xff); //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM_ICInitStructure.TIM_ICFilter = N;										//IC1F���������˲��� 
  TIM_ICInit(TIMx, &TIM_ICInitStructure);
	TIM_Cmd(TIMx,ENABLE ); 
}

 /**
 * @brief  		  �жϻص�����
 * @code
 *     					// ��ʼ����ʱ��2���û�����app(){};
 *      				TIMER_CallbackInstall(HW_TIMER2,app);
 * @endcode
 * @param[in]   HW_TIMERx	 ѡ��ʱ��ģ��   ֵ��HW_TIMER2��HW_TIMER3��HW_TIMER4 
 * @param[in]   AppCBFun   �û�������ַ
 */

void TIMER_CallbackInstall(u8 HW_TIMERx, TIMER_CallBackType AppCBFun)
{
	if(AppCBFun != NULL)
	{
		TIMER_CallBackTable[HW_TIMERx] = AppCBFun;
	}
}

 /**
 * @brief  		 ��ʱ��ʹ�ܺ���
 * @code
 *     					// ��ʼ����ʱ��2,������ʱ���ж�
 *      				TIM_cmd(TIM2,ENABLE);	
 * @endcode
 * @param[in]   TIMx			 ѡ��ʱ��ģ��   ֵ��TIM2��TIM3��TIM4 
 * @param[in]   NewState   ʹ��/ʧ�ܶ�ʱ������ѡֵENABLE��DISABLE
 */
void TIM_cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
	TIM_Cmd(TIMx, NewState);   
}

 /**
 * @brief  		 ��ʱ����PWMʹ���ڲ��������û��������
 */
u8 TIM_tim_Init(TIM_TypeDef* TIMx, u32 Tim_us, FunctionalState NewState)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	u16 arr;
	u16 psc;
	u32 num;
	num = Tim_us;
	psc = num/20000;
	if(psc <= 1) 
	{
		psc = 0;
		arr = --num;
	}
	else if((psc > 1) &&(psc <= 10000))
	{
		arr = num/psc - 1;	
		psc--;
	}		
	else if(psc > 10000)
	{
		printf("Tim_us is too much");
		return 0;
	}
	
	if(TIMx == TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	}
	else if(TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	}
	else if(TIMx == TIM4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	}

	TIM_TimeBaseStructure.TIM_Period = arr;                
	TIM_TimeBaseStructure.TIM_Prescaler = psc;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);  
 

	TIM_Cmd(TIMx, NewState);  
	return  1;
}
 /**
 * @brief  		 GPIO��PWMʹ���ڲ��������û��������
 */
	void GPIO_PWM_Init(TIM_TypeDef* TIMx, u8 PWM_CHx)
	{
		if(TIMx == TIM2)
		{  switch(PWM_CHx)
			 {case PWM_CH1: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_0, GPIO_Mode_AF_PP); break;
				case PWM_CH2: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_1, GPIO_Mode_AF_PP); break;
				case PWM_CH3: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_2, GPIO_Mode_AF_PP); break;
				case PWM_CH4: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_3, GPIO_Mode_AF_PP); break;
				default : break; }}
		else if(TIMx == TIM3)
		{  switch(PWM_CHx)
			 {case PWM_CH1: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_6, GPIO_Mode_AF_PP); break;
				case PWM_CH2: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_AF_PP); break;
				case PWM_CH3: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_0, GPIO_Mode_AF_PP); break;
				case PWM_CH4: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_1, GPIO_Mode_AF_PP); break;
				default : break; }}
		else if(TIMx == TIM4)
		{  switch(PWM_CHx)
			 {case PWM_CH1: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_AF_PP); break;
				case PWM_CH2: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_7, GPIO_Mode_AF_PP); break;
				case PWM_CH3: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_8, GPIO_Mode_AF_PP); break;
				case PWM_CH4: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9, GPIO_Mode_AF_PP); break;
				default : break; }}
	}

 /**
 * @brief  		 GPIO��CAPʹ���ڲ��������û��������
 */
u32 GPIO_Cap_Init(TIM_TypeDef* TIMx, u8 CAP_CHx)
{
	u16 TIM_Channel_x;
	u32 IT;

	if(TIMx == TIM2)
		{ RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM5ʱ��
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��  
			switch(CAP_CHx)
			 {case CAP_CH1: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_0, GPIO_Mode_IPD);GPIO_ResetBits(GPIOA,GPIO_Pin_0);
						 TIM_Channel_x = TIM_Channel_1;IT= TIM_IT_CC1;break;
				case CAP_CH2: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_1, GPIO_Mode_IPD);GPIO_ResetBits(GPIOA,GPIO_Pin_1);
    				 TIM_Channel_x = TIM_Channel_2; IT= TIM_IT_CC2;break;
				case CAP_CH3: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_2, GPIO_Mode_IPD);GPIO_ResetBits(GPIOA,GPIO_Pin_2);
    				 TIM_Channel_x = TIM_Channel_3; IT= TIM_IT_CC3;break;
				case CAP_CH4: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_3, GPIO_Mode_IPD);GPIO_ResetBits(GPIOA,GPIO_Pin_3);
    				 TIM_Channel_x = TIM_Channel_4; IT= TIM_IT_CC4;break;
				default : break; }}
		else if(TIMx == TIM3)
		{ RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM5ʱ��
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOAʱ�� 
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ�� 
			switch(CAP_CHx)
			 {case CAP_CH1: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_6,GPIO_Mode_IPD); GPIO_ResetBits(GPIOA,GPIO_Pin_6);	
							TIM_Channel_x = TIM_Channel_1; IT= TIM_IT_CC1;break;
				case CAP_CH2: GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7,GPIO_Mode_IPD); GPIO_ResetBits(GPIOA,GPIO_Pin_7);
							TIM_Channel_x = TIM_Channel_2; IT= TIM_IT_CC2;break;
				case CAP_CH3: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_0,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_0);
						 TIM_Channel_x = TIM_Channel_3; IT= TIM_IT_CC3;break;
				case CAP_CH4: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_1,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    				 TIM_Channel_x = TIM_Channel_4; IT= TIM_IT_CC4;break;
				default : break; }}
		else if(TIMx == TIM4)
		{	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM5ʱ��
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOAʱ��
			switch(CAP_CHx)
			 {case CAP_CH1: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_6);		
						 TIM_Channel_x = TIM_Channel_1; IT= TIM_IT_CC1;break;
				case CAP_CH2: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_7,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_7);
						 TIM_Channel_x = TIM_Channel_2; IT= TIM_IT_CC2;break;
				case CAP_CH3: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_8,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_8);
    				 TIM_Channel_x = TIM_Channel_3;IT= TIM_IT_CC3; break;
				case CAP_CH4: GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9,GPIO_Mode_IPD); GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
				     TIM_Channel_x = TIM_Channel_4;IT= TIM_IT_CC4; break;
				default : break; }}
		return 		((IT<<16) + TIM_Channel_x);
}

 /**
 * @brief  		 ��ʱ����PWMʹ���ڲ��������û��������
 */
void TIM_OCInit(TIM_TypeDef* TIMx, u8 PWM_CHx)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
	
	switch(PWM_CHx)
	{
		case PWM_CH1: TIM_OC1Init(TIMx, &TIM_OCInitStructure);   
									TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);  
									break;
		case PWM_CH2: TIM_OC2Init(TIMx, &TIM_OCInitStructure);   
									TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);  
									break;
		case PWM_CH3: TIM_OC3Init(TIMx, &TIM_OCInitStructure);   
									TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);  
									break;
		case PWM_CH4: TIM_OC4Init(TIMx, &TIM_OCInitStructure);   
									TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);  
									break;
		default : break; 
	}
}
	
 /**
 * @brief  		 �ڲ��������û��������
 */
u8 TIM_Ctim_Init(TIM_TypeDef* TIMx, u16 arr,u16 psc, u16 Subpri, FunctionalState NewState)
{
		IRQn_Type tim_irq;

		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	if(TIMx == TIM2)
	{
		tim_irq = TIM2_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	}
	else if(TIMx == TIM3)
	{
		tim_irq = TIM3_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	}
	else if(TIMx == TIM4)
	{
		tim_irq = TIM4_IRQn;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	}
		TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
		TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
		NVIC_InitStructure.NVIC_IRQChannel = tim_irq;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	
   	TIM_Cmd(TIMx, NewState); 
		return 0;
}

 /**
 * @brief  		 �ڲ��������û��������
 */
void TIM2_IRQHandler(void)   //TIM3�ж�
{
	if(TIMER_CallBackTable[HW_TIMER2] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER2]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
		}
}

 /**
 * @brief  		 �ڲ��������û��������
 */
void TIM3_IRQHandler(void)   //TIM3�ж�
{
		if(TIMER_CallBackTable[HW_TIMER3] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER3]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
		}
}

 /**
 * @brief  		 �ڲ��������û��������
 */
void TIM4_IRQHandler(void)   //TIM3�ж�
{
	if(TIMER_CallBackTable[HW_TIMER4] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER4]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
		}
}
