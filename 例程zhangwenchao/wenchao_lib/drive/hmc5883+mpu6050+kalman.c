/**
  ******************************************************************************
  * @file    hmc5883+mpu6050+kalman.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.2
  * @note    此程序为ARM中应用编程
  ******************************************************************************
  */

#include "sysinc.h"



 /**
 * @brief   		得到九轴数据
 * @param[in]   ag： 储存mpu六轴数据
 * @param[in]   p：  储存hmc三轴数据
 */
void Nine_Axis_Get(MPU_DATE *ag,HMC_DATE *p)
{
	  MPU6050_GetData(ag);
    MPU6050_Acc_Angle(ag);
    MPU6050_Gyr_W(ag);

	  HMC5883_GetLevel_Angle(p);
}

 /**
 * @brief   		得到六轴数据（加速度陀螺仪）
 * @param[in]   ag： 储存mpu六轴数据
 * @param[in]   p：  储存hmc三轴数据
 */
void Six_Axis_Get(MPU_DATE *ag)
{
	  MPU6050_GetData(ag);
    MPU6050_Acc_Angle(ag);
    MPU6050_Gyr_W(ag);
}

 /**
 * @brief   		得到三轴数据（地磁场）
 * @param[in]   ag： 储存mpu六轴数据
 * @param[in]   p：  储存hmc三轴数据
 */
void Sthree_Axis_Get(HMC_DATE *p)
{
	  HMC5883_GetLevel_Angle(p);
}


 /**
 * @brief   		HMC5883得到非水平放置下的偏角（实验结果不理想）
 * @param[in]   fly：  数据结构体FLYING_ANGLE指针
 * @param[in]   p：  数据结构体HMC_DATE指针
 */
void HMC5883_NLevel_Angle(FLYING_ANGLE *fly,HMC_DATE *p)
{			
			HMC5883_GetDate(p);
			float fly_pitch,fly_roll;
			fly_pitch = fly->pitch * DEG_TO_RAD;
			fly_roll  = fly->roll * DEG_TO_RAD;
			p->hmc_nlevel_x = p->hmc_date_x*cos(fly_pitch) + p->hmc_date_y*sin(fly_roll)*sin(fly_pitch)
	                                           + p->hmc_date_z*cos(fly_roll)*sin(fly_pitch); 
	
			p->hmc_nlevel_y = (-1)*p->hmc_date_y*cos(fly_roll) + p->hmc_date_z*sin(fly_roll);
			//p->hmc_nlevel_y = p->hmc_date_y*cos(fly_roll) + p->hmc_date_x*sin(fly_roll)*sin(fly_pitch)
	      //                                     - p->hmc_date_z*cos(fly_pitch)*sin(fly_roll); 
	
			//p->hmc_nlevel_x = p->hmc_date_x*cos(fly_pitch) + p->hmc_date_z*sin(fly_pitch);
	
			p->hmc_nlevel_angle = T_ANGLE*atan(p->hmc_nlevel_y/p->hmc_nlevel_x);
			p->hmc_nlevel_angle = fabsf(p->hmc_nlevel_angle);
			if(((p->hmc_date_y>0)&&(p->hmc_date_x<0)) || ((p->hmc_date_y<0)&&(p->hmc_date_x>0)))
			{
				p->hmc_nlevel_angle = p->hmc_nlevel_angle*(-1);
			}
			if(p->hmc_date_x<0){p->hmc_nlevel_angle = p->hmc_nlevel_angle+90;}
			else if(p->hmc_date_x>0){p->hmc_nlevel_angle = p->hmc_nlevel_angle-90;}
}



 /**
 * @brief   		kalman俯仰角
 * @param[in]   kalman： 储存kalman结构体数据
 * @param[in]   ag：     储存mpu六轴数据
 * @param[in]   fly：    数据结构体FLYING_ANGLE指针
 */
void Get_kalman_Pitch(KALMAN_SAME_PARAMETER *kalman,MPU_DATE *ag,FLYING_ANGLE *fly)
{
	Kalman_Same_Q_Filter(kalman, ag->angle_acc_x, ag->w_gyr_y*(-1));
	fly->pitch = kalman->angle_00+5.7;
}



 /**
 * @brief   		kalman横滚角
 * @param[in]   kalman： 储存kalman结构体数据
 * @param[in]   ag：     储存mpu六轴数据
 * @param[in]   fly：    数据结构体FLYING_ANGLE指针
 */
void Get_kalman_Roll(KALMAN_SAME_PARAMETER *kalman,MPU_DATE *ag,FLYING_ANGLE *fly)
{
	Kalman_Same_Q_Filter(kalman, ag->angle_acc_y, ag->w_gyr_x*(-1));
	fly->roll = kalman->angle_00-2.2;
}



 /**
 * @brief   		kalman航向角
 * @param[in]   kalman： 储存kalman结构体数据
 * @param[in]   ag：     储存mpu六轴数据
 * @param[in]   p：      数据结构体HMC_DATE指针
 * @param[in]   fly：    数据结构体FLYING_ANGLE指针
 */
void Get_kalman_Yaw(KALMAN_SAME_PARAMETER *kalman,MPU_DATE *ag, HMC_DATE *p,FLYING_ANGLE *fly)
{
	//HMC5883_NLevel_Angle(fly,p);
	
	HMC5883_Level_Fix(p, fly);
	Kalman_Same_Q_Filter(kalman, p->hmc_level_angle, ag->w_gyr_z*(-1));
	fly->yaw = kalman->angle_00;
}



 /**
 * @brief   		一阶互补滤波航向角
 * @param[in]   first_comp： 储存一阶互补滤波结构体数据
 * @param[in]   ag：     储存mpu六轴数据
 * @param[in]   p：      数据结构体HMC_DATE指针
 * @param[in]   fly：    数据结构体FLYING_ANGLE指针
 */
void Get_firstcomp_Yaw(FIRST_COMPLEMENTARY_PARAMETER *first_comp,MPU_DATE *ag, HMC_DATE *p,FLYING_ANGLE *fly)
{
	HMC5883_Level_Fix(p, fly);
	First_order_complementary_filter(first_comp, p->hmc_level_angle, ag->w_gyr_z*(-1));
	fly->yaw = first_comp->angle1;
}



 /**
 * @brief   		对HMC5883水平情况得到的角度进行倾斜修正（实验结果优于公式修正,对于不同环境，修正系数需重测）
 * @param[in]   p：      数据结构体HMC_DATE指针
 * @param[in]   fly：    数据结构体FLYING_ANGLE指针
 */
void HMC5883_Level_Fix(HMC_DATE *p,FLYING_ANGLE *fly)
{
	if((p->hmc_level_angle > -25) && (p->hmc_level_angle <= 25))
	{
		if(fly->pitch>0){p->hmc_level_angle +=  (fly->roll*0.3-fly->pitch*0) ;}
	}
	else if((p->hmc_level_angle > 25) && (p->hmc_level_angle <= 70))
	{
		p->hmc_level_angle -= (fly->pitch*1.1 + fly->roll*(-1.4)) ;
	}
	else if((p->hmc_level_angle > 70) && (p->hmc_level_angle <= 125))
	{
		p->hmc_level_angle -= (fly->roll*0.25+ fly->pitch*0.65);
	}
	else if((p->hmc_level_angle > 125) && (p->hmc_level_angle <= 180))
	{
		if(fly->roll < 0){p->hmc_level_angle -= fly->roll*0.35;}
		else	{p->hmc_level_angle -= fly->roll*0.6;}
	}
	else if((p->hmc_level_angle < -25) && (p->hmc_level_angle > -70))
	{
		p->hmc_level_angle += fly->pitch*0.6;
	}
	else if((p->hmc_level_angle > -125) && (p->hmc_level_angle <= -70))
	{
		p->hmc_level_angle += (fly->roll*(-0.25)+ fly->pitch*0.45);
	}
	else if((p->hmc_level_angle > -180) && (p->hmc_level_angle <= -125))
	{
		p->hmc_level_angle += fly->roll*(-0.6);
	}
	
}



