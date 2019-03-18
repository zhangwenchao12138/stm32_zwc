/**
  ******************************************************************************
  * @file    hmc5883.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.1
  * @note    此程序为ARM中应用编程
  ******************************************************************************
  */
#include "sysinc.h"
	
 /**
 * @brief   		HMC5883初始化
 */
void  HMC5883_Init(void)
{
	HMC5883_ByteWrite(HMC_REGB,0xE0);
	HMC5883_ByteWrite(HMC_MODE,0x00);
}

 /**
 * @brief   		HMC5883寄存器写数据
 * @param[in]   REG_Address   寄存器地址   值：参考头文件中的寄存器地址
 * @param[in]   REG_data      需要写入的数据      
 */
void HMC5883_ByteWrite(uint8_t REG_Address,uint8_t REG_data)
{
	IIC_Start();
	IIC_Send_Byte(HMC_SlaveAddress);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_Address);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_data);	
	IIC_Wait_Ack();
	
	IIC_Stop();	
}

 /**
 * @brief   		HMC5883单个寄存器读数据
 * @param[in]   REG_Address   寄存器地址   值：参考头文件中的寄存器地址
 * @return			读取的数据
 */
u8   HMC5883_ByteRead_Single(uint8_t REG_Address)
{
	u8 Date;
	IIC_Start();
	
	IIC_Send_Byte(HMC_SlaveAddress);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_Address);	
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(HMC_SlaveAddress+1);	
	IIC_Wait_Ack();
	Date = IIC_Read_Byte(0);
	IIC_Stop();	
	return Date;
}

 /**
 * @brief   		HMC5883读取全部数据
 * @param[in]   p：  数据结构体HMC_DATE指针
 */
void HMC5883_GetDate(HMC_DATE *p)
{
	u16 Date[6];
	Date[0] = HMC5883_ByteRead_Single(HMC_XOUT_H);
	Date[1] = HMC5883_ByteRead_Single(HMC_XOUT_L);
	Date[2] = HMC5883_ByteRead_Single(HMC_ZOUT_H);
	Date[3] = HMC5883_ByteRead_Single(HMC_ZOUT_L);
	Date[4] = HMC5883_ByteRead_Single(HMC_YOUT_H);
	Date[5] = HMC5883_ByteRead_Single(HMC_YOUT_L);
	p->hmc_date_x = (Date[0]<<8) | (Date[1]&0x00ff);
	p->hmc_date_x = p->hmc_date_x - p->hmc_offset_x;
	
	p->hmc_date_z = (Date[2]<<8) | (Date[3]&0x00ff);
  p->hmc_date_z = p->hmc_date_z - p->hmc_offset_z;

	p->hmc_date_y = (Date[4]<<8) | (Date[5]&0x00ff);
	p->hmc_date_y = p->hmc_date_y - p->hmc_offset_y;
}

 /**
 * @brief   		HMC5883读取偏差
 * @param[in]   p：  数据结构体HMC_DATE指针
 */
void HMC5883_offset(HMC_DATE *p)
{
	short tep_xh,tep_xl,tep_yh,tep_yl,tep_zh,tep_zl,i;
	tep_xh = 0;
	tep_xl = 0;
	tep_yh = 0;
	tep_yl = 0;
	tep_zh = 0;
	tep_zl = 0;
	
	for( i = 0; i<500; i++)
	{
		HMC5883_GetDate(p);
		if(p->hmc_date_x > tep_xh){ tep_xh = p->hmc_date_x;}
		if(p->hmc_date_x < tep_xl){ tep_xl = p->hmc_date_x;}
		if(p->hmc_date_y > tep_yh){ tep_yh = p->hmc_date_y;}
		if(p->hmc_date_y < tep_yl){ tep_yl = p->hmc_date_y;}
		if(p->hmc_date_z > tep_zh){ tep_zh = p->hmc_date_z;}
		if(p->hmc_date_z < tep_zl){ tep_zl = p->hmc_date_z;}

		SYSTICK_DelayMs(10);
	}
	p->hmc_offset_x = (tep_xh + tep_xl)/2;
	p->hmc_offset_y = (tep_yh + tep_yl)/2;
	p->hmc_offset_z = (tep_zh + tep_zl)/2;
}


 /**
 * @brief   		HMC5883得到水平放置下的偏角
 * @param[in]   p：  数据结构体HMC_DATE指针
 */
void HMC5883_GetLevel_Angle(HMC_DATE *p)
{
	HMC5883_GetDate(p);
	if(p->hmc_date_x == 0)
	{
		if(p->hmc_date_y > 0){p->hmc_level_angle = 0; }
		else if(p->hmc_date_y < 0){p->hmc_level_angle = -180; }
	}
	else if(p->hmc_date_x < 0)
	{
		p->hmc_level_angle =T_ANGLE*atan(((float)p->hmc_date_y)/((float)p->hmc_date_x)) + 90;
	}
	else
	{
		p->hmc_level_angle =T_ANGLE*atan(((float)p->hmc_date_y)/((float)p->hmc_date_x)) - 90;
	}
}



