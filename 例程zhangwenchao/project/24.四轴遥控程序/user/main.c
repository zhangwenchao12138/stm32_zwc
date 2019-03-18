/**
  ******************************************************************************
  * @note    串级PID测试，测试PID的响应曲线。串口输入可以改变比例积分微分系数。
  ******************************************************************************
  */

#include "sysinc.h"

PID_CASCADE_DATE pid_date[1]={0};
float tep,i,flag;
void timer3()
{	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{
			if(flag == 0) {i = i + 1;}
			else if(flag == 1){i = i - 1;}
			
			tep = 0.3*i;
			if((tep <= 16) && (flag <= 1))
			{
			PID_Cascade(5,tep,pid_date);
			printf("  %d        %d       %d    ",(int)tep,(int)pid_date->pid_out_err,(int)pid_date->pid_out_control);			
			printf("  %d        %d     \r\n",(int)pid_date->pid_in_err,(int)pid_date->pid_in_control);			
			}
			else if((tep > 15) && (flag == 0)){ flag = 1;}
			
			if((flag == 1) &&(tep <= 1)) {flag = 3;} 
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
		}
}


void uart1(uint16_t Res)
{		
		 char ch;
		 ch = Res;
  	 UART_WriteByte(HW_UART1, ch);
  	 UART_WriteByte(HW_UART1, '\n');
		 switch(ch)
		 {
			 case '1': pid_date->pid_out_p++; flag = 0; i = 0; tep = 0; break;
			 case '2': pid_date->pid_out_i++; flag = 0; i = 0; tep = 0; break;
			 case '3': pid_date->pid_out_d++; flag = 0; i = 0; tep = 0; break;
			 case '4': pid_date->pid_out_p++; flag = 0; i = 0; tep = 0; break;
			 case '5': pid_date->pid_out_i++; flag = 0; i = 0; tep = 0; break;
			 case '6': pid_date->pid_out_d++; flag = 0; i = 0; tep = 0; break;
			 default : break;
		 }
		 printf(" outp     outi    outd    inp      ini    ind   \r\n");
		 
		 printf("  %d        %d      %d      %d       %d     %d  \r\n",(int)pid_date->pid_out_p,(int)pid_date->pid_out_i,
									(int)pid_date->pid_out_d,(int)pid_date->pid_in_p,(int)pid_date->pid_in_i,(int)pid_date->pid_in_d);
		 
		 printf(" now_date    out_err   out_ctr   in_err   in_ctr   /r/n");
}


int main(void)
{		 
  NVIC_init();
	SYSTICK_DelayInit();
	PID_CASCADE_DATE_Init(pid_date);
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	UART_CallbackInstall(HW_UART1, uart1);
	TIM_ms_Init(TIM3, 10, TIM_IT_Update,0, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER3, timer3);
	i = 0;
	flag = 0;
	printf(" now_date    out_err   out_ctr   in_err   in_ctr   /r/n");
	while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);
		PCout(13)=1;	
		SYSTICK_DelayMs(900);
	}
}


