#include "sysinc.h"

#define TIMcc                    TIM4
#define CAP_CHx                  CAP_CH4
#define  TIM_GetCapture				   TIM_GetCapture4   //�ı�TIM_GetCapture2��������֣����ı�ͨ��
#define TIM_IT_CCx               TIM_IT_CC4
#define TIM_OCPolarityConfig     TIM_OC4PolarityConfig



u32 temp=0; 
u16  TIMCC_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIMCC_CAPTURE_VAL;	//���벶��ֵ
 
//��ʱ��5�жϷ������	 
void timercc(void)
{ 
	// printf("start: timer4\n");

 	if((TIMCC_CAPTURE_STA&0X8000)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIMcc, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIMCC_CAPTURE_STA&0X4000)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIMCC_CAPTURE_STA&0X3FFF)==0X3FFF)//�ߵ�ƽ̫����
				{
					TIMCC_CAPTURE_STA|=0X8000;//��ǳɹ�������һ��
					TIMCC_CAPTURE_VAL=0XFFFF;
				}else TIMCC_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIMcc, TIM_IT_CCx) != RESET)//����1���������¼�
		{	
			if(TIMCC_CAPTURE_STA&0X4000)		//����һ���½��� 		
			{	  			
				TIMCC_CAPTURE_STA|=0X8000;		//��ǳɹ�����һ��������
				TIMCC_CAPTURE_VAL=TIM_GetCapture(TIMcc);
				temp=TIMCC_CAPTURE_STA&0X3FFF;
			  temp*=65536;//���ʱ���ܺ�
			  temp+=TIMCC_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
			  printf(" HIGH:%d   one seventy second us\r\n",temp);//��ӡ�ܵĸߵ�ƽʱ��
			  TIMCC_CAPTURE_STA=0;//������һ�β���
		 		TIM_OCPolarityConfig(TIMcc,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIMCC_CAPTURE_STA=0;			//���
				TIMCC_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIMcc,0);
				TIMCC_CAPTURE_STA|=0X4000;		//��ǲ�����������
		 		TIM_OCPolarityConfig(TIMcc,TIM_ICPolarity_Falling);		
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIMcc, TIM_IT_CCx|TIM_IT_Update); //����жϱ�־λ
 
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

