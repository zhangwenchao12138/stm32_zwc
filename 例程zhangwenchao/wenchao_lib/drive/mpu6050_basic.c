/**
  ******************************************************************************
  * @file    mpu6050_basic.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.11
  * @note    �˳���ΪARM�ĵײ���
  ******************************************************************************
  */

#include "sysinc.h"


 /**
 * @brief   		MPU6050�Ĵ���д����
 * @param[in]   REG_Address   �Ĵ�����ַ   ֵ���ο�ͷ�ļ��еļĴ�����ַ
 * @param[in]   REG_data      ��Ҫд�������      
 */
void MPU6050_ByteWrite(uint8_t REG_Address,uint8_t REG_data)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_Address);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_data);	
	IIC_Wait_Ack();
	
	IIC_Stop();	
}


 /**
 * @brief   		MPU6050�Ĵ���������
 * @param[in]   REG_Address   �Ĵ�����ֵַ���ο�ͷ�ļ��еļĴ�����ַ
 * @return			��ȡ������
 */
uint8_t MPU6050_ByteRead(uint8_t REG_Address)
{
	u8 Date;
	IIC_Start();
	
	IIC_Send_Byte(SlaveAddress);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_Address);	
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(SlaveAddress+1);	
	IIC_Wait_Ack();
	Date = IIC_Read_Byte(0);
	IIC_Stop();	
	return Date;
}

 /**
 * @brief   		MPU6050��ʼ��
 */
void MPU6050_Init(void)
{
	MPU6050_ByteWrite(PWR_MGMT_1, 0x00);
	MPU6050_ByteWrite(SMPLRT_DIV, 0x07);
	MPU6050_ByteWrite(CONFIG_,    0x06);
	MPU6050_ByteWrite(GYRO_CONFIG, 0x18);
	MPU6050_ByteWrite(ACCEL_CONFIG, 0x01);
}

/**
 * @brief    		MPU6050�豸���
 * @return			����ֵ��1�����ɹ�     0�����ʧ��
 */

u8 MPU6050_Check(void)
{
	if((MPU6050_ByteRead(WHO_AM_I)) == 0x68)
		return 1;
	else
		return 0;
}

 /**
 * @brief   		MPU6050�����Լ캯��
 * @param[in]   ag    MPU6050���ݽṹ��ָ��
 */
void MPU6050_Offset(ACGY *ag)
{	
	int sum,i;
	sum = 0;
	ag->gyr_offset[0]=0;
	ag->gyr_offset[1]=0;
	ag->gyr_offset[2]=0;
	
	ag->acc_offset[0]=0;
	ag->acc_offset[1]=0;
	ag->acc_offset[2]=0;

	ag->tmp = 0;

	for(i=0; i<2000; i++)
	{ag->gyr_offset[0]=0;
	 ag->gyr_offset[0]	= MPU6050_ByteRead(GYRO_XOUT_H);
	 ag->gyr_offset[0]  =ag->gyr_offset[0]<<8;
	 ag->gyr_offset[0] |= MPU6050_ByteRead(GYRO_XOUT_L);
	 sum += ag->gyr_offset[0];
	}ag->gyr_offset[0]=sum/i;sum=0;
	for(i=0; i<2000; i++)
	{ag->gyr_offset[1]  =0;
	 ag->gyr_offset[1]  = MPU6050_ByteRead(GYRO_YOUT_H);
	 ag->gyr_offset[1]  =ag->gyr_offset[1]<<8;
	 ag->gyr_offset[1] |= MPU6050_ByteRead(GYRO_YOUT_L);
	 sum += ag->gyr_offset[1];
	}ag->gyr_offset[1]=sum/i;sum=0;
	for(i=0; i<2000; i++)
	{ag->gyr_offset[2]  =0;
	 ag->gyr_offset[2]  = MPU6050_ByteRead(GYRO_ZOUT_H);
	 ag->gyr_offset[2]  =ag->gyr_offset[2]<<8;
	 ag->gyr_offset[2] |= MPU6050_ByteRead(GYRO_ZOUT_L);
	 sum += ag->gyr_offset[2];
	}ag->gyr_offset[2]=sum/i;sum=0;
}


 /**
 * @brief   		MPU6050���ݶ�ȡ
 * @param[in]   ag    MPU6050���ݽṹ��ָ��
 */
void MPU6050_GetData(ACGY *ag)
{
	ag->gyr[0]  = MPU6050_ByteRead(GYRO_XOUT_H);
	ag->gyr[0]  =ag->gyr[0]<<8;
	ag->gyr[0] |= MPU6050_ByteRead(GYRO_XOUT_L);
	ag->gyr[0] -= ag->gyr_offset[0];
	
	ag->gyr[1]  = MPU6050_ByteRead(GYRO_YOUT_H);
	ag->gyr[1]  =ag->gyr[1]<<8;
	ag->gyr[1] |= MPU6050_ByteRead(GYRO_YOUT_L);
	ag->gyr[1] -= ag->gyr_offset[1];
	
	ag->gyr[2] = MPU6050_ByteRead(GYRO_ZOUT_H);
	ag->gyr[2] =ag->gyr[2]<<8;
	ag->gyr[2] |= MPU6050_ByteRead(GYRO_ZOUT_L);
	ag->gyr[2] -= ag->gyr_offset[2];

	ag->acc[0]  = MPU6050_ByteRead(ACCEL_XOUT_H);
	ag->acc[0]  =ag->acc[0]<<8;
	ag->acc[0] |= MPU6050_ByteRead(ACCEL_XOUT_L);
	ag->acc[0] -= ag->acc_offset[0];
	
	ag->acc[1]  = MPU6050_ByteRead(ACCEL_YOUT_H);
	ag->acc[1]  =ag->acc[1]<<8;
	ag->acc[1] |= MPU6050_ByteRead(ACCEL_YOUT_L);
	ag->acc[1] -= ag->acc_offset[1];
	
	ag->acc[2]  = MPU6050_ByteRead(ACCEL_ZOUT_H);
	ag->acc[2]  =ag->acc[2]<<8;
	ag->acc[2] |= MPU6050_ByteRead(ACCEL_ZOUT_L);
	ag->acc[2] -= ag->acc_offset[2];
	
	ag->tmp			= MPU6050_ByteRead(TEMP_OUT_H);
	ag->tmp 		=ag->tmp<<8;
	ag->tmp		 |= MPU6050_ByteRead(TEMP_OUT_L);
}


 /**
 * @brief   		MPU6050���ٶ�������ýǶ�
 * @param[in]   ag    MPU6050���ݽṹ��ָ��
 */
void MPU6050_Acc_Angle(ACGY *ag)
{
		float norm;
		norm = math_rsqrt((ag->acc[0]*ag->acc[0]+ag->acc[1]*ag->acc[1]+ag->acc[2]*ag->acc[2]));
		ag->angle_acc_x = ((float)ag->acc[0])/norm;
		ag->angle_acc_y = ((float)ag->acc[1])/norm;
		ag->angle_acc_z = ((float)ag->acc[2])/norm;
		ag->angle_acc_x = atan(ag->angle_acc_x/ag->angle_acc_z) * T_ANGLE;
		ag->angle_acc_y = atan(ag->angle_acc_y/ag->angle_acc_z) * T_ANGLE;
}

 /**
 * @brief   		MPU6050��������ý��ٶ�
 * @param[in]   ag    MPU6050���ݽṹ��ָ��
 */
void MPU6050_Gyr_W(ACGY *ag)
{
	  ag->w_gyr_x=ag->gyr[0]/16.4;
	  ag->w_gyr_y=ag->gyr[1]/16.4;
	  ag->w_gyr_z=ag->gyr[2]/16.4;
}
