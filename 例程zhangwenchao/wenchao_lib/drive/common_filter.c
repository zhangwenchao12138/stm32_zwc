/**
  ******************************************************************************
  * @file    common_filter.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.16
  * @note    �˳���ΪARM�е�Ӧ�ñ��
  ******************************************************************************
  */
#include "sysinc.h"


 /**
 * @brief   		һ���˲�������ʼ����ÿ����һ���ṹ�嶼Ҫ����ʼ��
 * @code
 *      				//��ʼ���ṹ�� First_order_x
 *      				First_Order_parameter_init(First_complementary_x, 0.15);
 * @endcode
 * @param[in]   First_Order_parameter *p  First_Order�����ṹ��
 * @param[in]   ki  ����ֵ
 */
void First_Order_parameter_init(FIRST_ORDER_PARAMETER *p,float ki)
{
	p->k = ki; 
	p->num = 1;
}


/**
 * @brief   		һ���˲���
 * @code
 *      				//�ṹ�� First_Order_x
 *      				First_Order_filter(First_Order_x, 45);
 * @endcode
 * @param[in]   First_Order_parameter *p,First_Order�����ṹ��
 * @param[in]   number ���µ�����
 */
void First_Order_filter(FIRST_ORDER_PARAMETER *p, float number)
{
     p->num = p->k * number + (1-p->k) * p->num;
}

