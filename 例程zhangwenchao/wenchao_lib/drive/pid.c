/**
  ******************************************************************************
  * @file    pid.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.10
  * @note    此程序PID应用层编程
  ******************************************************************************
  */

#include "sysinc.h"

 /**
 * @brief   		PID参数初始化，每定义一个结构体都要做初始化
 * @param[in]   piddate：参数结构体
 */
void PID_DATE_Init(PID_DATE *piddate)
{
		piddate->pid_p = 50;                       
	  piddate->pid_i = 0.1;
	  piddate->pid_d = 6;
		piddate->pid_err = 0;				
		piddate->pid_err_max = 3;
		piddate->pid_last_err = 0;								
		piddate->pid_integral_err = 0;						
		piddate->pid_integral_err_max = 300;						
	  piddate->pid_control = 0;
}

 /**
 * @brief   		PID控制函数
 * @param[in]   target_val：要达到的目标值
 * @param[in]   now_val：   现在的位置值
 * @param[in]   piddate：参数结构体
 */
void PID_Control(short target_val,short now_val,PID_DATE *piddate)
{
		piddate->pid_last_err = piddate->pid_err;
	  piddate->pid_err = target_val - now_val;
		if(piddate->pid_err > piddate->pid_err_max)
		{
			piddate->pid_err = piddate->pid_err_max;
		}
		piddate->pid_integral_err += piddate->pid_err;
		if(piddate->pid_integral_err > piddate->pid_integral_err_max)
		{
			piddate->pid_integral_err = piddate->pid_integral_err_max;
		}
		piddate->pid_control = piddate->pid_p*piddate->pid_err + piddate->pid_i*piddate->pid_integral_err+
																piddate->pid_d*(piddate->pid_err-piddate->pid_last_err);
}

 /**
 * @brief   		串级PID参数初始化，每定义一个结构体都要做初始化
 * @param[in]   pid_date：参数结构体
 */
void PID_CASCADE_DATE_Init(PID_CASCADE_DATE *pid_date)
{
	  pid_date->pid_out_p = 30;											
	  pid_date->pid_out_i = 1;
	  pid_date->pid_out_d = 6;
		pid_date->pid_out_err = 0;
		pid_date->pid_out_err_max = 3;
		pid_date->pid_out_last_err = 0;
		pid_date->pid_out_integral_err = 0;
		pid_date->pid_out_integral_err_max = 300;
	  pid_date->pid_out_control = 0;
	
		pid_date->pid_in_p = 30;											
	  pid_date->pid_in_i = 0.8;
	  pid_date->pid_in_d = 6;
		pid_date->pid_in_err = 0;
		pid_date->pid_in_err_max = 8;
		pid_date->pid_in_last_err = 0;
		pid_date->pid_in_integral_err = 0;
		pid_date->pid_in_integral_err_max = 300;
	  pid_date->pid_in_control = 0;
}


 /**
 * @brief   		串级PID控制函数
 * @param[in]   target_val：要达到的目标值
 * @param[in]   now_val：   现在的位置值
 * @param[in]   pid_date：参数结构体
 */
void PID_Cascade(short target_val,short now_val,PID_CASCADE_DATE *pid_date)
{
		PID_Out(target_val, now_val, pid_date);
		PID_In(pid_date->pid_out_control, pid_date->pid_out_err-pid_date->pid_out_last_err, pid_date);		
}

 /**
 * @brief   		串级外环PID控制函数
 * @param[in]   target_val：要达到的目标值
 * @param[in]   now_val：   现在的位置值
 * @param[in]   pid_date：参数结构体
 */
void PID_Out(short target_val,short now_val,PID_CASCADE_DATE *pid_date)
{
		pid_date->pid_out_last_err = pid_date->pid_out_err;
	  pid_date->pid_out_err = target_val - now_val;
	  if(pid_date->pid_out_err > pid_date->pid_out_err_max)
		{
			pid_date->pid_out_err = pid_date->pid_out_err_max;
		}
		pid_date->pid_out_integral_err += pid_date->pid_out_err;
		if(pid_date->pid_out_integral_err > pid_date->pid_out_integral_err_max)
		{
			pid_date->pid_out_integral_err = pid_date->pid_out_integral_err_max;
		}
		pid_date->pid_out_control = pid_date->pid_out_p*pid_date->pid_out_err +
																pid_date->pid_out_i*pid_date->pid_out_integral_err+
																pid_date->pid_out_d*(pid_date->pid_out_err-pid_date->pid_out_last_err);
}

 /**
 * @brief   		串级内环PID控制函数
 * @param[in]   target_val：要达到的目标值
 * @param[in]   now_val：   现在的位置值
 * @param[in]   pid_date：参数结构体
 */
void PID_In(short target_val,short now_val,PID_CASCADE_DATE *pid_date)
{
		pid_date->pid_in_last_err = pid_date->pid_in_err;
	  pid_date->pid_in_err = target_val - now_val;
		pid_date->pid_in_integral_err += pid_date->pid_in_err;
		if(pid_date->pid_in_integral_err > pid_date->pid_in_integral_err_max)
		{
			pid_date->pid_in_integral_err = pid_date->pid_in_integral_err_max;
		}
		pid_date->pid_in_control = pid_date->pid_in_p*pid_date->pid_in_err +
																pid_date->pid_in_i*pid_date->pid_in_integral_err+
																pid_date->pid_in_d*(pid_date->pid_in_err-pid_date->pid_in_last_err);
}


