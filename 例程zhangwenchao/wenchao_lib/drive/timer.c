/**
  ******************************************************************************
  * @file    timer.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.22
  * @note    此程序为ARM的底层编程（依附于官方底层库的二次封装）
  ******************************************************************************
  */
	
	
#include "sysinc.h"			

//定义中断回调函数
static TIMER_CallBackType TIMER_CallBackTable[6] = {NULL};



 /**
 * @brief  		  微秒级定时器初始化
 * @code
 *     					// 初始化定时器2，100us，定时器更新中断TIM_IT_Update，响应优先级2，开启定时器
 *      				TIM_us_Init(TIM2, 100, TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   Tim_us
 * @param[in]   TIM_IT		 常用中断类型     
 *              @arg TIM_IT_Update   定时器更新中断
 *              @arg TIM_IT_CC1			 输入捕捉中断-通道一
 *              @arg TIM_IT_CC2			 输入捕捉中断-通道二
 *              @arg TIM_IT_CC3      输入捕捉中断-通道三
 *              @arg TIM_IT_CC4			 输入捕捉中断-通道四
 * @param[in]   Subpri		 抢占优先级   值：0,1,2,3
 * @param[in]   NewState   使能/失能定时器，可选值ENABLE，DISABLE
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

  TIM_TimeBaseStructure.TIM_Period = arr;                                            //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc;                                         //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                            //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                        //TIM向上计数模式
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);                                    //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIMx,TIM_IT,NewState);                                                //使能指定的TIM3中断,允许更新中断
 
	NVIC_InitStructure.NVIC_IRQChannel = tim_irq;                           
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                          //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;                                   
	NVIC_Init(&NVIC_InitStructure);                                                    //初始化NVIC寄存器
	TIM_Cmd(TIMx, NewState);                                                           //使能TIMx	
	return  1;
}

 /**
 * @brief  		  毫秒级定时器初始化
 * @code
 *     					// 初始化定时器2，100ms，定时器更新中断TIM_IT_Update，响应优先级2，开启定时器
 *      				TIM_ms_Init(TIM2, 100, TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   Tim_us
 * @param[in]   TIM_IT		 常用中断类型     
 *              @arg TIM_IT_Update   定时器更新中断
 *              @arg TIM_IT_CC1			 输入捕捉中断-通道一
 *              @arg TIM_IT_CC2			 输入捕捉中断-通道二
 *              @arg TIM_IT_CC3      输入捕捉中断-通道三
 *              @arg TIM_IT_CC4			 输入捕捉中断-通道四
 * @param[in]   Subpri		 抢占优先级   值：0,1,2,3
 * @param[in]   NewState   使能/失能定时器，可选值ENABLE，DISABLE
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
 * @brief  		  定时器中断设置函数
 * @code
 *     					// 初始化定时器2, 定时器更新中断TIM_IT_Update，响应优先级2，开启定时器中断
 *      				TIM_ITconfig(TIM2,TIM_IT_Update,2, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   TIM_IT		 常用中断类型     
 *              @arg TIM_IT_Update   定时器更新中断
 *              @arg TIM_IT_CC1			 输入捕捉中断-通道一
 *              @arg TIM_IT_CC2			 输入捕捉中断-通道二
 *              @arg TIM_IT_CC3      输入捕捉中断-通道三
 *              @arg TIM_IT_CC4			 输入捕捉中断-通道四
 * @param[in]   Subpri		 抢占优先级   值：0,1,2,3
 * @param[in]   NewState   使能/失能定时器，可选值ENABLE，DISABLE
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
 * @brief  		  PWM初始化
 * @code
 *     					// 初始化定时器2，通道2，频率1000，开启定时器
 *      				TIM_PWM_Init(TIM2, PWM_CH2, 1000, ENABLE);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   PWM_CHx    PWM通道选择      值：PWM_CH1,PWM_CH2,PWM_CH3,PWM_CH4 
 * @param[in]   Frequency	 PWM的频率   
 * @param[in]   NewState   使能/失能定时器，可选值ENABLE，DISABLE
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
 * @brief  		  PWM设置占空比函数
 * @code
 *     					// 初始化定时器2，通道2，占空比20%
 *      					TIM_SetCompare(TIM2, PWM_CH2, 2000);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   PWM_CHx    PWM通道选择      值：PWM_CH1,PWM_CH2,PWM_CH3,PWM_CH4 
 * @param[in]   PWM_Duty   占空比           值：从0到10000，代表着从0%~100%
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
 * @brief  		  输入捕捉初始化
 * @code
 *     					// 初始化定时器2，通道2，装载计数值0xffff，预分频0，响应优先级2，数字滤波0
 *      					TIM_Cap_Init(TIM2, CAP_CH2, 0xffff,0, 2, 0);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   CAP_CHx    PWM通道选择      值：CAP_CH1,CAP_CH2,CAP_CH3,CAP_CH4 
 * @param[in]   arr				 预装值           值：0~0xffff
 * @param[in]   psc				 预分频值         值：0~0xffff
 * @param[in]   subpri     响应优先级，     值：0,1,2,3
 * @param[in]   N       	 滤波值           值：0~0x0f
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
	TIM_ITConfig(TIMx,TIM_IT_Update|((TIM_Channel_x>>16) & 0xff),ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ICInitStructure.TIM_Channel = (TIM_Channel_x & 0xff); //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = N;										//IC1F配置输入滤波器 
  TIM_ICInit(TIMx, &TIM_ICInitStructure);
	TIM_Cmd(TIMx,ENABLE ); 
}

 /**
 * @brief  		  中断回调函数
 * @code
 *     					// 初始化定时器2，用户函数app(){};
 *      				TIMER_CallbackInstall(HW_TIMER2,app);
 * @endcode
 * @param[in]   HW_TIMERx	 选择定时器模块   值：HW_TIMER2，HW_TIMER3，HW_TIMER4 
 * @param[in]   AppCBFun   用户函数地址
 */

void TIMER_CallbackInstall(u8 HW_TIMERx, TIMER_CallBackType AppCBFun)
{
	if(AppCBFun != NULL)
	{
		TIMER_CallBackTable[HW_TIMERx] = AppCBFun;
	}
}

 /**
 * @brief  		 定时器使能函数
 * @code
 *     					// 初始化定时器2,开启定时器中断
 *      				TIM_cmd(TIM2,ENABLE);	
 * @endcode
 * @param[in]   TIMx			 选择定时器模块   值：TIM2，TIM3，TIM4 
 * @param[in]   NewState   使能/失能定时器，可选值ENABLE，DISABLE
 */
void TIM_cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
	TIM_Cmd(TIMx, NewState);   
}

 /**
 * @brief  		 定时器供PWM使用内部函数，用户无需调用
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
 * @brief  		 GPIO供PWM使用内部函数，用户无需调用
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
 * @brief  		 GPIO供CAP使用内部函数，用户无需调用
 */
u32 GPIO_Cap_Init(TIM_TypeDef* TIMx, u8 CAP_CHx)
{
	u16 TIM_Channel_x;
	u32 IT;

	if(TIMx == TIM2)
		{ RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM5时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟  
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
		{ RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM5时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOA时钟 
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟 
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
		{	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM5时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOA时钟
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
 * @brief  		 定时器供PWM使用内部函数，用户无需调用
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
 * @brief  		 内部函数，用户无需调用
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
		TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
		TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
		NVIC_InitStructure.NVIC_IRQChannel = tim_irq;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = Subpri;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	
   	TIM_Cmd(TIMx, NewState); 
		return 0;
}

 /**
 * @brief  		 内部函数，用户无需调用
 */
void TIM2_IRQHandler(void)   //TIM3中断
{
	if(TIMER_CallBackTable[HW_TIMER2] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER2]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
		}
}

 /**
 * @brief  		 内部函数，用户无需调用
 */
void TIM3_IRQHandler(void)   //TIM3中断
{
		if(TIMER_CallBackTable[HW_TIMER3] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER3]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
		}
}

 /**
 * @brief  		 内部函数，用户无需调用
 */
void TIM4_IRQHandler(void)   //TIM3中断
{
	if(TIMER_CallBackTable[HW_TIMER4] != NULL)
		{
			TIMER_CallBackTable[HW_TIMER4]();
		}
		else
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
		}
}
