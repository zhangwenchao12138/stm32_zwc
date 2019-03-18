/**
  ******************************************************************************
  * @file         ��Դ�����
  * @author  			Harry
	* @qq      			3502596529
  * @version 			V2.0
  * @date    			2018.10.5
  * @note    			���Գ��� -- ��������
  * @config_pwm   timer2
  *               @arg PWM_CH3                 PA2      TB6612��PWMA���������ţ����ڿ��Ƶ��Aת��
  *               @arg PWM_CH4                 PA3			TB6612��PWMA���������ţ����ڿ��Ƶ��Bת��  
	* @config_gpio  TB6612���������ţ����Ƶ����ת����
  *               @arg AIN1                    PB14     ���Ƶ��A��ת����
  *               @arg AIN2                    PB15			���Ƶ��A��ת����
	*               @arg BIN1                    PB13     ���Ƶ��B��ת����
  *               @arg BIN2                    PB12			���Ƶ��B��ת����
  * @config_iic   ģ��iic(mpu6050)
  *               @arg IIC_SCL                 PB8
  *               @arg IIC_SDA                 PB9 
  * @config_led   ָʾ��
  *               @arg ����ָʾ��              PC13
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  ******************************************************************************
  */

#include "sysinc.h"

ACGY acgy[1]={0};                                          //��ʼ��һ���ṹ�壬�����������Ŀǰ�����õ���������
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};                //��ʼ�� һ���˲� �����ṹ��
 
int time=0;                                                //�����жϴ�����ʱ��������whileѭ�����ṩ׼ȷ��ʱ��

#define MAXPWM 9800                                        //����ռ�ձ����ƣ�ռ�ձ����ֻ�ܵ�9600�����������10000��Ϊ�˷�ֹ����������������ƣ�
#define KP 500                                             //�Ƕȵı���������pwm = KP * fcp->angle;

void motor_ctrl(int motor1_pwm, int motor2_pwm);           //������������Ŀ��ƺ���


//��ʱ���жϺ�����ÿ5ms����һ��
void timer3()
{	
		MPU6050_Get_Basic_Data(acgy);                          //�õ�MPU6050�Ļ������ݣ����ٶ�ֵ����ٶ�ֵ��
		MPU6050_Acc_Angle_Approximately(acgy);                 //�ɼ��ٶȽ�������Ƕ�
		MPU6050_Gyr_W(acgy);                                   //�ɽ��ٶ�ֵת��Ϊ���ٶȣ���λת��Ϊ��ÿ�룩
			
		First_order_complementary_filter(fcp, acgy->angle_acc_x, acgy->w_gyr_x);  //һ�ڻ����˲����õ��ɿ��Ƕ�

		time++;                                                //��whileѭ�������ṩһ��5ms��ʱ���׼
	
		int pwm = KP * fcp->angle;                             //���Ƕȳ��ϱ���ϵ����Ϊ������Ƶ�����
	
		if(pwm > MAXPWM) pwm = MAXPWM;                         //������PWM�����ֵ
		else if(pwm < -MAXPWM) pwm = -MAXPWM;                  //���Ƹ�PWM����Сֵ
	
		motor_ctrl(pwm, pwm );                                 //������ƺ�����������溯������
}

 int main(void)
 {
	NVIC_init();                                            //
	SYSTICK_DelayInit();	    	                               //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);   //��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);            //��ʼ������1�����ò�����Ϊ115200  

	IIC_Init();                                                //��ʼ��IIC
	MPU6050_Init();                                            //��ʼ��MPU6050	 
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //�����mpu6050ͨ���Ƿ�����
	else  {printf(" NO MPU6050_Check\n");return 0;}           //�����������ֱ���˳�����
	
	First_complementary_parameter_init(fcp, 5);

	TIM_us_Init(TIM3, 5000, TIM_IT_Update,2, ENABLE);	        //��ʱ��3�ṩһ��5ms���ж�
 	TIMER_CallbackInstall(HW_TIMER3, timer3);                 //ע���жϺ�����ÿ����ʱ��3�ж�ʱ������timer3������������������Ǹ�����

  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);                //���� PWM��ʼ����Ƶ��1000HZ
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP);	//TB6612���������ţ����Ƶ����ת����
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP);  //TB6612���������ţ����Ƶ����ת����
	PBout(14)= 1;
	PBout(15)= 0;

	TIM_PWM_Init(TIM2, PWM_CH4, 1000, ENABLE);                //���� PWM��ʼ����Ƶ��1000HZ
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_12, GPIO_Mode_Out_PP);  //TB6612���������ţ����Ƶ����ת����
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_13, GPIO_Mode_Out_PP);  //TB6612���������ţ����Ƶ����ת����
	PBout(12)= 1;
	PBout(13)= 0;
	
	TIM_SetCompare(TIM2, PWM_CH3, 0);                         //��ʼ�����ֵ�ռ�ձ�Ϊ0----ת��Ϊ0
	TIM_SetCompare(TIM2, PWM_CH4, 0);													//��ʼ�����ֵ�ռ�ձ�Ϊ0----ת��Ϊ0


  while(1)
	{		
		if(time >= 100){                                        //ÿ�ж�100�Σ�Ҳ����500ms����һ��
			PCout(13)= !PCout(13);                                //LED��˸
			time = 0;                                             //����
		}
	}
}


 /**
 * @brief   		������ƺ���
 * @code
 *      				//��ʼ�����A��B��ת��ת��Ϊռ�ձ�50%��ת��
 *      				motor_ctrl(5000, 5000);
 * @endcode
 * @param[in]   motor1_pwm  A���ռ�ձȣ�����-10000~+10000,���������Ϊ����ʱ�򣬵����ת�����������Ϊ����ʱ�򣬵����ת
 * @param[in]   motor1_pwm  A���ռ�ձȣ�����-10000~+10000,���������Ϊ����ʱ�򣬵����ת�����������Ϊ����ʱ�򣬵����ת
 */
void motor_ctrl(int motor1_pwm, int motor2_pwm){
	if(motor1_pwm >= 0){   //��pwm > 0ʱ��ͨ������TB6612��AIN1��AIN2�ĸߵ͵�ƽ�����Ƶ��������ת��
		PBout(14)= 1;        //AIN1
		PBout(15)= 0;        //AIN2
		TIM_SetCompare(TIM2, PWM_CH3, motor1_pwm);  //���Ƶ��ת��
	}else{
		PBout(14)= 0;
		PBout(15)= 1;
		TIM_SetCompare(TIM2, PWM_CH3, -1*motor1_pwm);
	}
	
	if(motor2_pwm >= 0){  //��pwm > 0ʱ��ͨ������TB6612��BIN1��BIN2�ĸߵ͵�ƽ�����Ƶ��������ת��
		PBout(12)= 0;
		PBout(13)= 1;
		TIM_SetCompare(TIM2, PWM_CH4, motor2_pwm);
	}else{
		PBout(12)= 1;
		PBout(13)= 0;
		TIM_SetCompare(TIM2, PWM_CH4, -1*motor2_pwm);
	}
}


