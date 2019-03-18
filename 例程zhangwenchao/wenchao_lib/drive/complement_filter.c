/**
  ******************************************************************************
  * @file    complemen_filter.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.26
  * @note    此程序为ARM中的应用编程
  ******************************************************************************
  */
#include "sysinc.h"


 /**
 * @brief   		一阶互补滤波参数初始化，每定义一个结构体都要做初始化
 * @code
 *      				//初始化结构体 First_complementary_x，周期5ms
 *      				First_complementary_parameter_init(First_complementary_x, 5);
 * @endcode
 * @param[in]   First_complementary_parameter *p  First_complementary参数结构体
 * @param[in]   Time，周期值，单位为ms
 */
void First_complementary_parameter_init(FIRST_COMPLEMENTARY_PARAMETER *p, float Time)
{
	p->K1 = 0.1; 
  p->T = 0.001*3.0*(float)Time;								
}


/**
 * @brief   		一阶互补滤波器
 * @code
 *      				//结构体 First_complementary_x，加速度算的角度angle_m=45度，陀螺仪角速度值gyro_m=12deg/s
 *      				First_order_complementary_filter(First_complementary_x, 45, 12);
 * @endcode
 * @param[in]   First_complementary_parameter *p,First_complementary参数结构体
 * @param[in]   angle_m 加速度算的角度
 * @param[in]   gyro_m	陀螺仪角速度值
 */
void First_order_complementary_filter(FIRST_COMPLEMENTARY_PARAMETER *p, float angle_m, float gyro_m)
{
     p->angle1 = p->K1 * angle_m+ (1-p->K1) * (p->angle1 + gyro_m * p->T);
}



 /**
 * @brief   	 二阶互补滤波参数初始化，每定义一个结构体都要做初始化
 * @code
 *      				//初始化结构体 Two_complementary_x，周期5ms
 *     					Two_complementary_parameter_init(Two_complementary_x, 5);
 * @endcode
 * @param[in]   Two_complementary_parameter *p  Two_complementary参数结构体
 * @param[in]   Time，周期值，单位为ms
 */
void Two_complementary_parameter_init(TWO_COMPLEMENTARY_PARAMETER *p, float Time)
{
  p->T = 0.001*3.0*Time;
	p->K2 =0.2; 
}


/**
 * @brief   		二阶互补滤波器
 * @code
 *      				//结构体  Two_complementary_x，加速度算的角度angle_m=45度，陀螺仪角速度值gyro_m=12deg/s
 *      				Two_order_complementary_filter(Two_complementary_x, 45, 12);
 * @endcode
 * @param[in]   Two_complementary_parameter *p, Two_complementary参数结构体
 * @param[in]   angle_m 加速度算的角度
 * @param[in]   gyro_m	陀螺仪角速度值
 */
void Two_order_complementary_filter(TWO_COMPLEMENTARY_PARAMETER *p, float angle_m, float gyro_m)	 
{
    p->x1 = (angle_m-p->angle2)*(1-p->K2)*(1-p->K2);
    p->y1 = p->y1+p->x1*p->T;
    p->x2 = p->y1+2*(1-p->K2)*(angle_m-p->angle2)+gyro_m;
    p->angle2 = p->angle2+ p->x2*p->T;
}
