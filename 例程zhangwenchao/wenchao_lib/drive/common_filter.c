/**
  ******************************************************************************
  * @file    common_filter.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.10.16
  * @note    此程序为ARM中的应用编程
  ******************************************************************************
  */
#include "sysinc.h"


 /**
 * @brief   		一阶滤波参数初始化，每定义一个结构体都要做初始化
 * @code
 *      				//初始化结构体 First_order_x
 *      				First_Order_parameter_init(First_complementary_x, 0.15);
 * @endcode
 * @param[in]   First_Order_parameter *p  First_Order参数结构体
 * @param[in]   ki  比例值
 */
void First_Order_parameter_init(FIRST_ORDER_PARAMETER *p,float ki)
{
	p->k = ki; 
	p->num = 1;
}


/**
 * @brief   		一阶滤波器
 * @code
 *      				//结构体 First_Order_x
 *      				First_Order_filter(First_Order_x, 45);
 * @endcode
 * @param[in]   First_Order_parameter *p,First_Order参数结构体
 * @param[in]   number 更新的数据
 */
void First_Order_filter(FIRST_ORDER_PARAMETER *p, float number)
{
     p->num = p->k * number + (1-p->k) * p->num;
}

