/**
  ******************************************************************************
  * @file    complemen_filter.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.9.26
  * @note    �˳���ΪARM�е�Ӧ�ñ��
  ******************************************************************************
  */
#include "sysinc.h"


 /**
 * @brief   		һ�׻����˲�������ʼ����ÿ����һ���ṹ�嶼Ҫ����ʼ��
 * @code
 *      				//��ʼ���ṹ�� First_complementary_x������5ms
 *      				First_complementary_parameter_init(First_complementary_x, 5);
 * @endcode
 * @param[in]   First_complementary_parameter *p  First_complementary�����ṹ��
 * @param[in]   Time������ֵ����λΪms
 */
void First_complementary_parameter_init(FIRST_COMPLEMENTARY_PARAMETER *p, float Time)
{
	p->K1 = 0.1; 
  p->T = 0.001*3.0*(float)Time;								
}


/**
 * @brief   		һ�׻����˲���
 * @code
 *      				//�ṹ�� First_complementary_x�����ٶ���ĽǶ�angle_m=45�ȣ������ǽ��ٶ�ֵgyro_m=12deg/s
 *      				First_order_complementary_filter(First_complementary_x, 45, 12);
 * @endcode
 * @param[in]   First_complementary_parameter *p,First_complementary�����ṹ��
 * @param[in]   angle_m ���ٶ���ĽǶ�
 * @param[in]   gyro_m	�����ǽ��ٶ�ֵ
 */
void First_order_complementary_filter(FIRST_COMPLEMENTARY_PARAMETER *p, float angle_m, float gyro_m)
{
     p->angle1 = p->K1 * angle_m+ (1-p->K1) * (p->angle1 + gyro_m * p->T);
}



 /**
 * @brief   	 ���׻����˲�������ʼ����ÿ����һ���ṹ�嶼Ҫ����ʼ��
 * @code
 *      				//��ʼ���ṹ�� Two_complementary_x������5ms
 *     					Two_complementary_parameter_init(Two_complementary_x, 5);
 * @endcode
 * @param[in]   Two_complementary_parameter *p  Two_complementary�����ṹ��
 * @param[in]   Time������ֵ����λΪms
 */
void Two_complementary_parameter_init(TWO_COMPLEMENTARY_PARAMETER *p, float Time)
{
  p->T = 0.001*3.0*Time;
	p->K2 =0.2; 
}


/**
 * @brief   		���׻����˲���
 * @code
 *      				//�ṹ��  Two_complementary_x�����ٶ���ĽǶ�angle_m=45�ȣ������ǽ��ٶ�ֵgyro_m=12deg/s
 *      				Two_order_complementary_filter(Two_complementary_x, 45, 12);
 * @endcode
 * @param[in]   Two_complementary_parameter *p, Two_complementary�����ṹ��
 * @param[in]   angle_m ���ٶ���ĽǶ�
 * @param[in]   gyro_m	�����ǽ��ٶ�ֵ
 */
void Two_order_complementary_filter(TWO_COMPLEMENTARY_PARAMETER *p, float angle_m, float gyro_m)	 
{
    p->x1 = (angle_m-p->angle2)*(1-p->K2)*(1-p->K2);
    p->y1 = p->y1+p->x1*p->T;
    p->x2 = p->y1+2*(1-p->K2)*(angle_m-p->angle2)+gyro_m;
    p->angle2 = p->angle2+ p->x2*p->T;
}
