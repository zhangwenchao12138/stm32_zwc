#include "sysinc.h"

#include "balance_car.h"

ACGY acgy[1]={0};                                          //��ʼ��һ���ṹ�壬�����������Ŀǰ�����õ���������
FIRST_COMPLEMENTARY_PARAMETER fcp[1] = {0};                //��ʼ�� һ���˲� �����ṹ��
KALMAN_DIFFERENT_PARAMETER kalman[1] = {0};                //��ʼ�� �������˲������ṹ��

int timerInterruptCounter = 0;                             //��ʱ���жϼ�����ÿ�����˴�����һ���ٶȻ����ƺ���
int Velocity_pwm = 0;                                      //�ٶȻ�������õ�PWMֵ
int angle_pwm  = 0;                                        //ֱ����������õ�PWMֵ

#define MAXPWM 10000                                       //PWM�޷���������pwm�������ʱ�����ֵ

#define EXTITRIGGER_TYPE  EXTI_Trigger_Rising              //�ⲿ�ж�ģʽ���������ش�����ʽ

int motor1_encoder=0, motor2_encoder=0;                    //��������������������Ϊ�ٶ�ֵ
int time=0;																								 //������5ms��ʱ���ж�ʱ��timer��1����while�еĳ����ṩ׼ȷʱ�ӣ���Ϊ���жϣ�����while�е���ʾ�����ͻ᲻׼��

void exti_moter1(void);                                    //�ⲿ�ж� -- ��������������
void exti_moter2(void);																		 //�ⲿ�ж� -- ��������������
void motor_ctrl(int motor1_pwm, int motor2_pwm);           //������ƺ���

void timer3()                                              //��ʱ���жϣ�5ms����һ�Σ���ƽ�⳵���ƺ���
{	
					MPU6050_Get_Basic_Data(acgy);                    //�õ�MPU6050�Ļ������ݣ����ٶ�ֵ����ٶ�ֵ��
					MPU6050_Acc_Angle_Approximately(acgy);           //�ɼ��ٶȽ�������Ƕ�
					MPU6050_Gyr_W(acgy);                             //�ɽ��ٶ�ֵת��Ϊ���ٶȣ���λת��Ϊ��ÿ�룩
				//	First_order_complementary_filter(fcp , acgy->angle_acc_x, acgy->w_gyr_x);
					Kalman_Different_Q_Filter(kalman, acgy->angle_acc_x, acgy->w_gyr_x);  //һ�ڻ����˲����õ��ɿ��Ƕ�
					time++;                                          //��whileѭ�������ṩһ��5ms��ʱ���׼
					angle_pwm  = (int)(AngleController(kalman->angle, kalman->angle_dot));  //ֱ�������Ʒ���pwm

					if(++timerInterruptCounter > 8){                 //ÿ�˴ν���һ�Σ�����ٶȻ�pwm
						Velocity_pwm = VelocityController(motor1_encoder, motor2_encoder);   //�ٶȻ����pwm
						timerInterruptCounter = 0;
						motor1_encoder = 0;                            //���㣬���¼�������Ϊ�ٶ�ֵ
						motor2_encoder = 0;
					}
					int pwm = angle_pwm  + Velocity_pwm;             //�����pwm
					if(pwm > MAXPWM) pwm = MAXPWM;                   //�޷�
					else if(pwm < -MAXPWM) pwm = -MAXPWM;
					motor_ctrl(pwm, pwm );                           //�������
}

 int main(void)
 {
	NVIC_init();
	SYSTICK_DelayInit();	    	 														 //��ʱ������ʼ��	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP); //��ʼ����LED���ӵ�Ӳ���ӿ�
	UART_QuickInit(HW_UART1, 115200, 2, 2, ENABLE);          //��ʼ������1�����ò�����Ϊ115200

	IIC_Init();                                              //��ʼ��IIC
	MPU6050_Init();                                          //��ʼ��MPU6050	 
	 
	if(MPU6050_Check()) printf(" MPU6050_Check OK\n");        //�����mpu6050ͨ���Ƿ�����
	else  {printf(" NO MPU6050_Check\n");return 0;}           //�����������ֱ���˳�����
	
	First_complementary_parameter_init(fcp, 5);
	kalman_different_parameter_init(kalman,5);               //kalman������ʼ��
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_6, 0, 3);              //�������ⲿ�жϳ�ʼ��
	EXTI_CallbackInstall(EXTI_Pin_6, exti_moter1);	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_IPU);

	
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_7, 0, 3);              //�������ⲿ�жϳ�ʼ��
	EXTI_CallbackInstall(EXTI_Pin_7, exti_moter2);
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPU);
	
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

void exti_moter1(void)
{ 			
	if(PAin(7) == 1){
		motor1_encoder++;
	}else{
		motor1_encoder--;
	}
}

void exti_moter2(void)
{ 			
	if(PBin(6) == 1){
		motor2_encoder++;	
	}else{
		motor2_encoder--;;	
	}
}
