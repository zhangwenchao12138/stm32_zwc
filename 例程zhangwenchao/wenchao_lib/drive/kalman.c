/**
  ******************************************************************************
  * @file    kalman.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.25
  * @note    此程序为底层应用程序KALMAN
  ******************************************************************************
  */

#include "sysinc.h"


 /**
 * @brief   		kalman参数初始化，每定义一个结构体都要做初始化
 * @code
 *      				//初始化结构体 kalman_x，周期5ms
 *      				kalman_same_parameter_init(kalman_x,i5);
 * @endcode
 * @param[in]   kalman_same_parameter *p,kalman参数结构体
 * @param[in]   Time，周期值，单位为ms
 */
void kalman_same_parameter_init(KALMAN_SAME_PARAMETER *p,float Time)
{
	p->A=1.0;
	p->H=1.0; 						
  p->Q=0.003;
	p->R=0.01;					
  p->P00=5.0;
	p->P10=0.0;				
  p->T = 0.001*(float)Time;								
  p->angle_00=0.0;	
}

 /**
 * @brief   		kalman Q不变滤波器
 * @code
 *      				//结构体 kalman_x，加速度算的角度angle_m=45度，陀螺仪角速度值gyro_m=12deg/s
 *      				Kalman_Same_Q_Filter(&kalman_x,45, 12)；
 * @endcode
 * @param[in]   kalman_same_parameter *p,kalman参数结构体
 * @param[in]   angle_m 加速度算的角度
 * @param[in]   gyro_m	陀螺仪角速度值
 */
void Kalman_Same_Q_Filter(KALMAN_SAME_PARAMETER *p,float angle_m, float gyro_m)
{
		p->angle_10=p->A*p->angle_00+p->T*gyro_m; 										//预测值
		p->P10=p->A*p->P00*(p->A)+p->Q;																//预测P
		p->Kg=p->P10*(p->H)/(p->H*p->P10*(p->H)+p->R);								//更新Kg
		p->angle_00=p->angle_10+p->Kg*(angle_m-p->H*p->angle_10);			//得出最优值
		p->P00=(1-p->Kg*p->H)*p->P10;																	//更新P
}






 /**
 * @brief   		kalman参数初始化，每定义一个结构体都要做初始化
 * @code
 *      				//初始化结构体 kalman_x，周期5ms
 *      				kalman_different_parameter_init(kalman_x,i5);
 * @endcode
 * @param[in]   kalman_different_parameter *p,kalman参数结构体
 * @param[in]   Time，周期值，单位为ms
 */
void kalman_different_parameter_init(KALMAN_DIFFERENT_PARAMETER *p,float Time)
{
  p->T = 0.001*(float)Time;								
	p->P[0][0] = 1;
	p->P[0][1] = 0;
	p->P[1][0] = 0;
	p->P[1][1] = 1;
  p->Pdot[0] = 0;
  p->Pdot[1] = 0;
  p->Pdot[2] = 0;
  p->Pdot[3] = 0;
	p->Q_angle = 0.001;
	p->Q_gyro = 0.005; 
	p->R_angle = 0.5;
	p->C_0 = 1; 
}


/**
 * @brief   		kalman Q变滤波器
 * @code
 *      				//结构体 kalman_x，加速度算的角度angle_m=45度，陀螺仪角速度值gyro_m=12deg/s
 *      				Kalman_Different_Q_Filter(&kalman_x,45, 12)；
 * @endcode
 * @param[in]   kalman_same_parameter *p,kalman参数结构体
 * @param[in]   angle_m 加速度算的角度
 * @param[in]   gyro_m	陀螺仪角速度值
 */
void Kalman_Different_Q_Filter(KALMAN_DIFFERENT_PARAMETER *p,float angle_m, float gyro_m)
{
		p->angle += (gyro_m-p->q_bias) * p->T; 
		p->angle_err = angle_m - p->angle;  
		p->Pdot[0] = p->Q_angle - p->P[0][1] - p->P[1][0];   
		p->Pdot[1] = -p->P[1][1];
		p->Pdot[2] = -p->P[1][1];
		p->Pdot[3] = p->Q_gyro;
		p->P[0][0] += p->Pdot[0] * p->T;  
		p->P[0][1] += p->Pdot[1] * p->T; 
		p->P[1][0] += p->Pdot[2] * p->T;
		p->P[1][1] += p->Pdot[3] * p->T;
		p->PCt_0 = p->C_0 * p->P[0][0];   
		p->PCt_1 = p->C_0 * p->P[1][0];
		p->E = p->R_angle + p->C_0 *p->PCt_0;  
		p->K_0 = p->PCt_0 / p->E;         
		p->K_1 = p->PCt_1 / p->E;
		p->t_0 = p->PCt_0;
		p->t_1 = p->C_0 * p->P[0][1];
		p->P[0][0] -= p->K_0 * p->t_0;  
		p->P[0][1] -= p->K_0 * p->t_1;
		p->P[1][0] -= p->K_1 * p->t_0;
		p->P[1][1] -= p->K_1 * p->t_1;  
		p->angle += p->K_0 * p->angle_err; 
		p->q_bias += p->K_1 * p->angle_err;
		p->angle_dot = gyro_m-p->q_bias;
}


