/**
  ******************************************************************************
  * @file    quaternion_fix.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.27
  * @note    此程序为ARM的应用编程（参考网上例程，本人添加注释）
  ******************************************************************************
  */

#include "sysinc.h"



float q0 = 1, q1 = 0, q2 = 0, q3 = 0;       
float exInt = 0, eyInt = 0, ezInt = 0;        

float Yaw,Pitch,Roll;  


 /**
 * @brief   		四元数更新-三个角度
 * @code
 *      				IMUupdate(acgy->gyr[0],acgy->gyr[1], acgy->gyr[2],acgy->acc[0], acgy->acc[1], acgy->acc[2]);
 * @endcode
 * @param[in]   gx		x轴角速度，单位deg/s
 * @param[in]   gy		y轴角速度，单位deg/s
 * @param[in]   gz		z轴角速度，单位deg/s
 * @param[in]   ax		x轴加速度数据，可以无单位
 * @param[in]   ay		y轴加速度数据，可以无单位
 * @param[in]   az		z轴加速度数据，可以无单位
 */
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
        float norm;
        float vx, vy, vz;
        float ex, ey, ez;  

        norm = sqrt(ax*ax + ay*ay + az*az);                       //标准化加速度
        ax = ax / norm;                    
        ay = ay / norm;
        az = az / norm;      

        vx = 2*(q1*q3 - q0*q2);																		//利用角速度得到的世界坐标
        vy = 2*(q0*q1 + q2*q3);
        vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

        ex = (ay*vz - az*vy);                                     //角速度得到的坐标与加速度坐标做误差
        ey = (az*vx - ax*vz);																			//提示：三维坐标运算，z*x=y
        ez = (ax*vy - ay*vx);

        exInt = exInt + ex*Ki;																		//误差积分
        eyInt = eyInt + ey*Ki;
        ezInt = ezInt + ez*Ki;

        gx = gx + Kp*ex + exInt;																	//修正后的角速度
        gy = gy + Kp*ey + eyInt;
        gz = gz + Kp*ez + ezInt;

        q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;									//更新四元数
        q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
        q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
        q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  

        norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);								//标准化四元数
        q0 = q0 / norm;
        q1 = q1 / norm;
        q2 = q2 / norm;
        q3 = q3 / norm;

        Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; 					//得到俯仰角和翻滚角
        Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; 
        //Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;           
}




