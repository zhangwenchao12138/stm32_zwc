/**
  ******************************************************************************
  * @file    			four_axis_aircraft.c
  * @author  			张文超 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.5
  * @note    			此程序四轴应用层编程，专门为四轴主程序打辅助
  * @config_pwm   timer2
  *               @arg PWM_CH1                 PA0
  *               @arg PWM_CH2                 PA1
  *               @arg PWM_CH3                 PA2
  *               @arg PWM_CH4                 PA3
  * @config_adc   adc1
  *               @arg ADC_CH4                 PA4
  * @config_iic   模拟iic(mpu6050+hmc5883)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_spi   spi2(nrf2401)
  *               @arg NRF24L01_CE             PB4
  *               @arg NRF24L01_IRQ            PB6
  *               @arg NRF24L01_CSN            PB12
  *               @arg NRF24L01_SCK            PB13
  *               @arg NRF24L01_MISO           PB14
  *               @arg NRF24L01_MOSI           PB15  
  * @config_led   指示灯
  *               @arg 运行指示灯              PC13
  *               @arg 电量低指示灯            PA12
  *               @arg 夜灯                    PA11
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44号引脚     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */
/**
  ******************************************************************************
  * @note    			NRF2401发送接收说明
  * @发送       类型      txbuf[0](类型代号)           txbuf[1](值)       txbuf[2](值)        txbuf[3](值)
  *         电源电压十倍值          1                   adc_date*10       
  *            姿态角               2                 fly_angle->pitch   fly_angle->roll      fly_angle->yaw	
  *          俯仰角PID值            3                      外环P         			 外环I  						 外环D          内环.....
  *          横滚角PID值            4                      外环P         			 外环I  						 外环D          内环.....
  *           电源警报              5                        8       
  *
  * @接收       类型        rxbuf[0](类型代号)                rxbuf[1](值)       rxbuf[2](值)          rxbuf[3](值)
  *        油门控制                 10                         油门值/8	
  *        开启实时更新姿态角       11                       12(其他值关闭)	
  *        实时获取电压十倍值       12                       13(其他值关闭)	       
  *        实时获取俯仰角PID值      13                       14(其他值关闭)	       外环P         			 外环P  						 外环P          内环.....
  *        实时获取横滚角PID值      14                       15(其他值关闭)	       外环P         			 外环P  						 外环P          内环.....
  *        更新外环P                15                       1(pitch)：2(roll)      11(P++) 						 12(P--)
  *        更新外环I                16                       1(pitch)：2(roll)      11(I++) 						 12(I--)
  *        更新外环D                17                       1(pitch)：2(roll)      11(D++) 						 12(D--)
  *        更新内环P                18                       1(pitch)：2(roll)      13(P++) 						 15(P--)
  *        更新内环I                19                       1(pitch)：2(roll)      13(I++) 						 15(I--)
  *        更新内环D                20                       1(pitch)：2(roll)      13(D++) 						 15(D--)
  *        向东飞行                 21                      1
  *        向西飞行                 22                      2
  *        向南飞行                 23                      3
  *        向北飞行                 24                      4
  *         悬停                    25                      1
  *        夜灯控制                 26                      3（开启，其他值关闭）
  ******************************************************************************
  */
	
	/**
  ******************************************************************************
  * @note    			航向说明
  *               west and east           由pitch控制，pitch为负角度时，向东飞行，反之，向西飞行
  *               north and south         由roll控制，roll为负角度时，向北飞行，反之，向南飞行
  *               yaw说明：指向角度不变，以此锁死方向角，使飞行器得以保持航向正确。
  *               
  ******************************************************************************
  */
	
	/**
  ******************************************************************************
  * @note    			油门说明
  *               accelerator             油门的初始值就是PWM_BASE_F，就是占空比的最高值
  *               							          油门未推动时，飞行器停机
  *               
  ******************************************************************************
  */

	
#include "sysinc.h"

extern PID_CASCADE_DATE pid_date_pitch[1];
extern PID_CASCADE_DATE pid_date_roll[1];
extern u8 rxbuf[33];

 /**
 * @brief   		悬停
 * @param[in]   dire_fly：    数据结构体DIRECTION_FLY指针,储存pid需要到达的角度
 */
void hover_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = 0;
}

 /**
 * @brief   		向东飞行
 * @param[in]   dire_fly：    数据结构体DIRECTION_FLY指针,储存pid需要到达的角度
 */
void east_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = -6;
	dire_fly->south_north_fly = 0;
}


 /**
 * @brief   		向西飞行
 * @param[in]   dire_fly：    数据结构体DIRECTION_FLY指针,储存pid需要到达的角度
 */
void west_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 6;
	dire_fly->south_north_fly = 0;
}


 /**
 * @brief   		向南飞行
 * @param[in]   dire_fly：    数据结构体DIRECTION_FLY指针,储存pid需要到达的角度
 */
void south_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = -6;
}


 /**
 * @brief   	  向北飞行
 * @param[in]   dire_fly：    数据结构体DIRECTION_FLY指针,储存pid需要到达的角度
 */
void north_flying(DIRECTION_FLY *dire_fly)
{
	dire_fly->east_west_fly = 0;
	dire_fly->south_north_fly = 6;
}

 /**
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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
 * @brief   	  配合nrf2401中断判别，进行pid系数更新
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



