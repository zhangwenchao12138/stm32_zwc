#include "sysinc.h"

u8 txbuf[33]={1,7,3,4};
u8 rxbuf[33]={1,9,3,4};

void uart1(uint16_t Res)
{		
  	 UART_WriteByte(HW_UART1, Res);
}


void uart2(uint16_t Res)
{		
  	 UART_WriteByte(HW_UART2, Res);
}


 int main(void)
 {	
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	UART_QuickInit(HW_UART2, 9600, 2, 2, ENABLE);
	UART_CallbackInstall(HW_UART1, uart1);
	UART_CallbackInstall(HW_UART2, uart1);

	while(1)
	{		
		PCout(13)=0;
		SYSTICK_DelayMs(900);	 //��ʱ300ms
		PCout(13)=1;	
		SYSTICK_DelayMs(900);	//��ʱ300ms
	}
 }
