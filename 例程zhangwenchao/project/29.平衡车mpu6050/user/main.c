#include "sysinc.h"

ACGY acgy[1]={0};                //��ʼ��һ���ṹ�壬�����������Ŀǰ�����õ���������

int main(void)
{
	NVIC_init();
	SYSTICK_DelayInit();	    	                                  //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);      //��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);               //��ʼ������1�����ò�����Ϊ115200     

	IIC_Init();           //��ʼ��IIC
	MPU6050_Init();       //��ʼ��MPU6050
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");            //�����mpu6050ͨ���Ƿ�����
	else  {printf(" NO MPU6050_Check\n");return 0;}               //�����������ֱ���˳�����

  while(1)
	{		
			MPU6050_Get_Basic_Data(acgy);                             //�õ�MPU6050�Ļ������ݣ����ٶ�ֵ����ٶ�ֵ��
			MPU6050_Acc_Angle_Approximately(acgy);                    //�ɼ��ٶȽ�������Ƕ�
			MPU6050_Gyr_W(acgy);                                      //�ɽ��ٶ�ֵת��Ϊ���ٶȣ���λת��Ϊ��ÿ�룩
			
			printf("acc_angle=%f   gyr=%f \n",acgy->angle_acc_x, acgy->w_gyr_x);  //�������X�����б�Ƕ�ֵ��X�᷽��Ľ��ٶ�ֵ��
			PCout(13)= !PCout(13);                           //LED����˸
			SYSTICK_DelayMs(300);                            //��ʱ300MS
	}
}

