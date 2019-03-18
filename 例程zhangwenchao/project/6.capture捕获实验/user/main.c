#include "sysinc.h"

#define TIMcc                    TIM4
#define CAP_CHx                  CAP_CH4
#define  TIM_GetCapture				   TIM_GetCapture4   //改变TIM_GetCapture2的这个数字，即改变通道
#define TIM_IT_CCx               TIM_IT_CC4
#define TIM_OCPolarityConfig     TIM_OC4PolarityConfig



u32 temp=0; 
u16  TIMCC_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIMCC_CAPTURE_VAL;	//输入捕获值
 
//定时器5中断服务程序	 
void timercc(void)
{ 
	// printf("start: timer4\n");

 	if((TIMCC_CAPTURE_STA&0X8000)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIMcc, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIMCC_CAPTURE_STA&0X4000)//已经捕获到高电平了
			{
				if((TIMCC_CAPTURE_STA&0X3FFF)==0X3FFF)//高电平太长了
				{
					TIMCC_CAPTURE_STA|=0X8000;//标记成功捕获了一次
					TIMCC_CAPTURE_VAL=0XFFFF;
				}else TIMCC_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIMcc, TIM_IT_CCx) != RESET)//捕获1发生捕获事件
		{	
			if(TIMCC_CAPTURE_STA&0X4000)		//捕获到一个下降沿 		
			{	  			
				TIMCC_CAPTURE_STA|=0X8000;		//标记成功捕获到一次上升沿
				TIMCC_CAPTURE_VAL=TIM_GetCapture(TIMcc);
				temp=TIMCC_CAPTURE_STA&0X3FFF;
			  temp*=65536;//溢出时间总和
			  temp+=TIMCC_CAPTURE_VAL;//得到总的高电平时间
			  printf(" HIGH:%d   one seventy second us\r\n",temp);//打印总的高点平时间
			  TIMCC_CAPTURE_STA=0;//开启下一次捕获
		 		TIM_OCPolarityConfig(TIMcc,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIMCC_CAPTURE_STA=0;			//清空
				TIMCC_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIMcc,0);
				TIMCC_CAPTURE_STA|=0X4000;		//标记捕获到了上升沿
		 		TIM_OCPolarityConfig(TIMcc,TIM_ICPolarity_Falling);		
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIMcc, TIM_IT_CCx|TIM_IT_Update); //清除中断标志位
 
}

 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	TIM_Cap_Init(TIMcc, CAP_CHx, 0xffff,0, 2, 0);
	TIMER_CallbackInstall(HW_TIMER4, timercc);
	printf("start:");
	while(1)
	{			
		PCout(13)=0;
		SYSTICK_DelayMs(900);	 
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
 }

