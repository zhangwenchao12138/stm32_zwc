/**
  ******************************************************************************
  * @file    			four_axis_aircraft.c
  * @author  			���ĳ� 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.5
  * @note    			�˳�������Ӧ�ò��̣�ר��Ϊ�������������
  * @config_pwm   timer2
  *               @arg PWM_CH1                 PA0
  *               @arg PWM_CH2                 PA1
  *               @arg PWM_CH3                 PA2
  *               @arg PWM_CH4                 PA3
  * @config_adc   adc1
  *               @arg ADC_CH4                 PA4
  * @config_iic   ģ��iic(mpu6050+hmc5883)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_spi   spi2(nrf2401)
  *               @arg NRF24L01_CE             PB4
  *               @arg NRF24L01_IRQ            PB6
  *               @arg NRF24L01_CSN            PB12
  *               @arg NRF24L01_SCK            PB13
  *               @arg NRF24L01_MISO           PB14
  *               @arg NRF24L01_MOSI           PB15  
  * @config_led   ָʾ��
  *               @arg ����ָʾ��              PC13
  *               @arg ������ָʾ��            PA12
  *               @arg ҹ��                    PA11
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44������     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */
/**
  ******************************************************************************
  * @note    			NRF2401���ͽ���˵��
  * @����       ����      txbuf[0](���ʹ���)           txbuf[1](ֵ)       txbuf[2](ֵ)        txbuf[3](ֵ)
  *         ��Դ��ѹʮ��ֵ          1                   adc_date*10       
  *            ��̬��               2                 fly_angle->pitch   fly_angle->roll      fly_angle->yaw	
  *          ������PIDֵ            3                      �⻷P         			 �⻷I  						 �⻷D          �ڻ�.....
  *          �����PIDֵ            4                      �⻷P         			 �⻷I  						 �⻷D          �ڻ�.....
  *           ��Դ����              5                        8       
  *
  * @����       ����        rxbuf[0](���ʹ���)                rxbuf[1](ֵ)       rxbuf[2](ֵ)          rxbuf[3](ֵ)
  *        ���ſ���                 10                         ����ֵ/8	
  *        ����ʵʱ������̬��       11                       12(����ֵ�ر�)	
  *        ʵʱ��ȡ��ѹʮ��ֵ       12                       13(����ֵ�ر�)	       
  *        ʵʱ��ȡ������PIDֵ      13                       14(����ֵ�ر�)	       �⻷P         			 �⻷P  						 �⻷P          �ڻ�.....
  *        ʵʱ��ȡ�����PIDֵ      14                       15(����ֵ�ر�)	       �⻷P         			 �⻷P  						 �⻷P          �ڻ�.....
  *        �����⻷P                15                       1(pitch)��2(roll)      11(P++) 						 12(P--)
  *        �����⻷I                16                       1(pitch)��2(roll)      11(I++) 						 12(I--)
  *        �����⻷D                17                       1(pitch)��2(roll)      11(D++) 						 12(D--)
  *        �����ڻ�P                18                       1(pitch)��2(roll)      13(P++) 						 15(P--)
  *        �����ڻ�I                19                       1(pitch)��2(roll)      13(I++) 						 15(I--)
  *        �����ڻ�D                20                       1(pitch)��2(roll)      13(D++) 						 15(D--)
  *        �򶫷���                 21                      1
  *        ��������                 22                      2
  *        ���Ϸ���                 23                      3
  *        �򱱷���                 24                      4
  *         ��ͣ                    25                      1
  *        ҹ�ƿ���                 26                      3������������ֵ�رգ�
  ******************************************************************************
  */
	
	/**
  ******************************************************************************
  * @note    			����˵��
  *               west and east           ��pitch���ƣ�pitchΪ���Ƕ�ʱ���򶫷��У���֮����������
  *               north and south         ��roll���ƣ�rollΪ���Ƕ�ʱ���򱱷��У���֮�����Ϸ���
  *               yaw˵����ָ��ǶȲ��䣬�Դ���������ǣ�ʹ���������Ա��ֺ�����ȷ��
  *               
  ******************************************************************************
  */
	
	/**
  ******************************************************************************
  * @note    			����˵��
  *               accelerator             ���ŵĳ�ʼֵ����PWM_BASE_F������ռ�ձȵ����ֵ
  *               							          ����δ�ƶ�ʱ��������ͣ��
  *               
  ******************************************************************************
  */

	
#include "sysinc.h"

extern PID_CASCADE_DATE pid_date_pitch[1];
extern PID_CASCADE_DATE pid_date_roll[1];
extern u8 rxbuf[33];

 /**
 * @brief   		��ͣ
 * @param[in]   dire_fly��    ���ݽṹ��DIRECTION_FLYָ��,����pid��Ҫ����ĽǶ�
 */
void hover_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = 0;
}

 /**
 * @brief   		�򶫷���
 * @param[in]   dire_fly��    ���ݽṹ��DIRECTION_FLYָ��,����pid��Ҫ����ĽǶ�
 */
void east_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = -6;
	dire_fly->south_north_fly = 0;
}


 /**
 * @brief   		��������
 * @param[in]   dire_fly��    ���ݽṹ��DIRECTION_FLYָ��,����pid��Ҫ����ĽǶ�
 */
void west_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 6;
	dire_fly->south_north_fly = 0;
}


 /**
 * @brief   		���Ϸ���
 * @param[in]   dire_fly��    ���ݽṹ��DIRECTION_FLYָ��,����pid��Ҫ����ĽǶ�
 */
void south_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = -6;
}


 /**
 * @brief   	  �򱱷���
 * @param[in]   dire_fly��    ���ݽṹ��DIRECTION_FLYָ��,����pid��Ҫ����ĽǶ�
 */
void north_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = 6;
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_out_p(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 11)
		pid_date_pitch->pid_out_p++;
		if(rxbuf[2] == 12)
		pid_date_pitch->pid_out_p--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 11)
		pid_date_roll->pid_out_p++;
		if(rxbuf[2] == 12)
		pid_date_roll->pid_out_p--;
	}
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_out_i(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 11)
		pid_date_pitch->pid_out_i++;
		if(rxbuf[2] == 12)
		pid_date_pitch->pid_out_i--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 11)
		pid_date_roll->pid_out_i++;
		if(rxbuf[2] == 12)
		pid_date_roll->pid_out_i--;
	}
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_out_d(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 11)
		pid_date_pitch->pid_out_d++;
		if(rxbuf[2] == 12)
		pid_date_pitch->pid_out_d--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 11)
		pid_date_roll->pid_out_d++;
		if(rxbuf[2] == 12)
		pid_date_roll->pid_out_d--;
	}
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_in_p(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 13)
		pid_date_pitch->pid_in_p++;
		if(rxbuf[2] == 15)
		pid_date_pitch->pid_in_p--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 13)
		pid_date_roll->pid_in_p++;
		if(rxbuf[2] == 15)
		pid_date_roll->pid_in_p--;
	}
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_in_i(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 13)
		pid_date_pitch->pid_in_i++;
		if(rxbuf[2] == 15)
		pid_date_pitch->pid_in_i--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 13)
		pid_date_roll->pid_in_i++;
		if(rxbuf[2] == 15)
		pid_date_roll->pid_in_i--;
	}
}

 /**
 * @brief   	  ���nrf2401�ж��б𣬽���pidϵ������
 */
void update_in_d(void)
{
	if(rxbuf[1] == 1)
	{
		if(rxbuf[2] == 13)
		pid_date_pitch->pid_in_d++;
		if(rxbuf[2] == 15)
		pid_date_pitch->pid_in_d--;
	}
	else if(rxbuf[1] == 2)
	{
		if(rxbuf[2] == 13)
		pid_date_roll->pid_in_d++;
		if(rxbuf[2] == 15)
		pid_date_roll->pid_in_d--;
	}
}



