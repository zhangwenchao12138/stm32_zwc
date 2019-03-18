#include "sysinc.h"

ACGY acgy[1]={0};                                          //��ʼ��һ���ṹ�壬�����������Ŀǰ�����õ���������
KALMAN_DIFFERENT_PARAMETER kalman[1] = {0};                //��ʼ��kalman�����ṹ��
 
int time=0;                                                //�����жϴ�����ʱ��������whileѭ�����ṩ׼ȷ��ʱ��

void timer3()
{	
		MPU6050_Get_Basic_Data(acgy);                          //�õ�MPU6050�Ļ������ݣ����ٶ�ֵ����ٶ�ֵ��
		MPU6050_Acc_Angle_Approximately(acgy);                 //�ɼ��ٶȽ�������Ƕ�
		MPU6050_Gyr_W(acgy);                                   //�ɽ��ٶ�ֵת��Ϊ���ٶȣ���λת��Ϊ��ÿ�룩
		Kalman_Different_Q_Filter(kalman, acgy->angle_acc_x, acgy->w_gyr_x);  //kalman�˲�
		time++;
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	                              //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);  //��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);           //��ʼ������1�����ò�����Ϊ115200     

	IIC_Init();                                               //��ʼ��IIC
	MPU6050_Init();                                           //��ʼ��MPU6050
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //�����mpu6050ͨ���Ƿ�����
	else  {printf(" NO MPU6050_Check\n");return 0;}           //�����������ֱ���˳�����
	
	kalman_different_parameter_init(kalman,5);                //kalman������ʼ��
	
	TIM_us_Init(TIM3, 5000, TIM_IT_Update,2, ENABLE);	        //��ʱ��3�ṩһ��5ms���ж�
 	TIMER_CallbackInstall(HW_TIMER3, timer3);                 //ע���жϺ�����ÿ����ʱ��3�ж�ʱ������timer3������������������Ǹ�����

  while(1)
	{		
		printf(" %f  %f\n",kalman->angle, kalman->angle_dot);   //���kalman�˲���ĽǶ�ֵ�����Ž��ٶ�
		if(time >= 100){                                        //ÿ�ж�100�Σ�Ҳ����500ms����һ��
			PCout(13)= !PCout(13);                                //LED��˸
			time = 0;                                             //����
		}
	}
}

