/**
  ******************************************************************************
  * @file    nrf2401.c
  * @author  张文超 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.22
  * @note    此程序为ARM中nrf2401的底层驱动编程
  ******************************************************************************
  */

#include "sysinc.h"


 /*收发地址*/
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 


 /**
 * @brief    		NRF2401初始化函数
 * @code
 *      				// 初始化Spi1
 *       				NRF24L01_Init(Spi1);
 * @endcode
 * @param[in]   Spix : Spi模块选择，可选值有Spi1，Spi2，Spi3
 */
void NRF24L01_Init(int Spix)
{
  SPI_QuickInit(Spix, CPOL_LOW_CPHA_1, SPI_BaudRatePrescaler_16, SPI_NSS_Soft);
	NRF24L01_CE=0; 																																						//使能24L01
	NRF24L01_CSN=1;																																						//SPI片选取消  
	SPI_SetSpeed(SPIcs,SPI_BaudRatePrescaler_8);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_6, 3, 3);

	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPD);																			//初始化与LED连接的硬件接口
	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_4);	
	if(NRF24L01_Check(SPIcs)==1)
	 {
			printf(" no_check\n");
	 }
	 else
		 printf("  check_0k\n");
     NRF24L01_RX_Mode(SPIcs);
}



 /**
 * @brief  			NRF2401中断函数
 * @code
 *      			  初始化extiA，pin10，主优先级preepri=2,响应优先级subpri=2,使能中断ITsta=ENABLE
 *      				NRF24L01_ITinit(HW_EXTIA, EXTI_Pin_10, 2, 2);
 * @endcode
 * @param[in]   instance 模块号
 *              @arg HW_EXTIA A端口
 *              @arg HW_EXTIB B端口
 *              @arg HW_EXTIC C端口
 *              @arg HW_EXTID D端口
 *              @arg HW_EXTIE E端口
 * @param[in]   EXTI_Pin_x ：x为GPIO引脚号，可选值 0,1,2....15
 * @param[in]   preepri 抢占优先级，可选值0,1,2,3
 * @param[in]   subpri  响应优先级，可选值0,1,2,3
 */
void NRF24L01_ITinit(int instance, int EXTI_Pin_x,int preepri, int subpri)
{
	EXTI_QuickInit(instance, EXTI_Pin_x, preepri, subpri);
}



 /**
 * @brief    		NRF2401配置为接收模式
 * @code
 *      				配置SPI1为NRF2401的接收模块
 *       				NRF24L01_RX_Mode(SPI1)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 */
void NRF24L01_RX_Mode(SPI_TypeDef *SPIx)
{
  	NRF24L01_Write_Reg(SPIcs, WRITE_REG_NRF+CONFIG, 0x08);																	//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	  EXTI_IRQcmd(HW_EXTIB, EXTI_Pin_6,ENABLE);

	  NRF24L01_CE=0;	  
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);				//写RX节点地址
	  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_AA,0x01);    																	//使能通道0的自动应答    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_RXADDR,0x01);																	//使能通道0的接收地址  	 
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_CH,40);	     																	//设置RF通信频率		  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);												//选择通道0的有效数据宽度 	    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_SETUP,0x0f);																	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG, 0x0f);																		//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  	NRF24L01_CE = 1; //CE为高,进入接收模式 
}


 /**
 * @brief    		NRF2401配置为发射模式
 * @code
 *      				配置SPI1为NRF2401的接收模块
 *       				NRF24L01_TX_Mode(SPI1)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 */
void NRF24L01_TX_Mode(SPI_TypeDef *SPIx)
{
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG, 0x08);																//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
		EXTI_IRQcmd(HW_EXTIB, EXTI_Pin_6,DISABLE);

		NRF24L01_CE=0;	    
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);				//写TX节点地址 
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); 		//设置TX节点地址,主要为了使能ACK	  

  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_AA,0x01);     														//使能通道0的自动应答    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_RXADDR,0x01); 														//使能通道0的接收地址  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+SETUP_RETR,0x1a);														//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_CH,40);      														  //设置RF通道为40
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_SETUP,0x0f); 														  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG,0x0e);    														//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	  NRF24L01_CE=1;//CE为高,10us后启动发送
}


 /**
 * @brief    		NRF2401写数据
 * @code
 *      				配置SPI1，写入寄存器地址为0x01,数组a[12],长度为12
 *       				NRF24L01_Write_Buf(SPI1 ,0x01, a, 12)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   reg :  寄存器地址
 * @param[in]   pBuf : 数据地址
 * @param[in]   len :  数据长度
 * @return			返回值:读到的状态值
 */
u8 NRF24L01_Write_Buf(SPI_TypeDef *SPIx ,u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;	    
 	  NRF24L01_CSN = 0;          																												//使能SPI传输
  	status = SPI_ReadWriteByte(SPIx ,reg);																						//发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_ReadWriteByte(SPIx ,*pBuf++); 							//写入数据	 
  	NRF24L01_CSN = 1;       																													//关闭SPI传输
  	return status;          																													//返回读到的状态值
}


 /**
 * @brief    		NRF2401读数据
 * @code
 *      				配置SPI1，读寄存器地址为0x01,数组a[12],长度为12
 *       				NRF24L01_Read_Buf(SPI1 ,0x01, a, 12)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   reg :  寄存器地址
 * @param[in]   pBuf : 数据地址
 * @param[in]   len :  数据长度
 * @return			返回值:读到的状态值
 */
u8 NRF24L01_Read_Buf(SPI_TypeDef *SPIx ,u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           																																//使能SPI传输
  	status=SPI_ReadWriteByte(SPIx, reg);																												//发送寄存器值(位置),并读取状态值   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++) pBuf[u8_ctr]=SPI_ReadWriteByte(SPIx, 0XFF);								//读出数据
  	NRF24L01_CSN=1;       																																			//关闭SPI传输
  	return status;       																																			  //返回读到的状态值
}


 /**
 * @brief    		NRF2401读寄存器
 * @code
 *      				配置SPI1，读寄存器地址为0x01
 *       				NRF24L01_Read_Reg(SPI1, 0x01)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   reg :  寄存器地址
 * @return			返回值:读到的寄存器值
 */
u8 NRF24L01_Read_Reg(SPI_TypeDef *SPIx, u8 reg)
{
	  u8 reg_val;	    
 	  NRF24L01_CSN = 0;          																																	//使能SPI传输		
  	SPI_ReadWriteByte(SPIx, reg);   																														//发送寄存器号
  	reg_val=SPI_ReadWriteByte(SPIx, 0XFF);																											//读取寄存器内容
  	NRF24L01_CSN = 1;          																																	//禁止SPI传输		    
  	return(reg_val);       
}

 /**
 * @brief    		NRF2401写寄存器
 * @code
 *      				配置SPI1，写寄存器地址为0x01
 *       				NRF24L01_Write_Reg(SPI1, 0x01)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   reg :  寄存器地址
 * @return			返回值:读到的状态值
 */
u8 NRF24L01_Write_Reg(SPI_TypeDef *SPIx, u8 reg, u8 value)
{
		u8 status;	
   	NRF24L01_CSN=0;                 																														 //使能SPI传输
  	status =SPI_ReadWriteByte(SPIx, reg);																												 //发送寄存器号 
  	SPI_ReadWriteByte(SPIx, value);     																												 //写入寄存器的值
  	NRF24L01_CSN=1;                																															 //禁止SPI传输	   
  	return(status);       																																		   //返回状态值
}

 /**
 * @brief    		NRF2401检查是否可用
 * @code
 *      				配置SPI1
 *       				NRF24L01_Check(SPI1)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @return			返回0：初始化正确   1：初始化错误
 */
u8 NRF24L01_Check(SPI_TypeDef *SPIx)
{
  u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8); 																									//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+TX_ADDR,buf,5);																				//写入5个字节的地址.	
	NRF24L01_Read_Buf(SPIx, TX_ADDR,buf,5); 																											//读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;																																							//检测24L01错误	
	return 0;		 																																									//检测到24L01
}


 /**
 * @brief    		NRF2401发送数据包
 * @code
 *      				配置SPI1，写数据为adress[2];
 *       				NRF24L01_TxPacket(SPI1, adress)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   txbuf: 数据地址
 * @return			返回值: MAX_TX:达到最大重发次数    TX_OK:发送完成     0xff:其他原因发送失败
 */
u8 NRF24L01_TxPacket(SPI_TypeDef *SPIx, u8 *txbuf)
{
	NRF24L01_TX_Mode(SPIx);
	u8 sta;
 	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8);																							//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(SPIx, WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);																//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送
	
	while(NRF24L01_IRQ != 0);																																	//等待发送完成
	sta=NRF24L01_Read_Reg(SPIx, STATUS);  																										//读取状态寄存器的值	   

	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+STATUS,sta); 																			//清除TX_DS或MAX_RT中断标志
	NRF24L01_CE=0;
	if(sta&MAX_TX)
	{
		NRF24L01_Write_Reg(SPIx, FLUSH_TX,0xff);																								//清除TX FIFO寄存器 
	  NRF24L01_RX_Mode(SPIcs);	
		return MAX_TX; 
	}
	if(sta&TX_OK)
	{
		NRF24L01_RX_Mode(SPIcs);	
		return TX_OK;
	}
	NRF24L01_RX_Mode(SPIcs);	
	return 0xff;
}

 /**
 * @brief    		NRF2401接收数据包
 * @code
 *      				配置SPI1，读数据为adress[2];
 *       				NRF24L01_RxPacket(SPI1, adress)；
 * @endcode
 * @param[in]   SPIx : SPI模块选择，可选值有SPI1，SPI2，SPI3
 * @param[in]   txbuf: 数据地址
 * @return			返回值: 0：接收到数据    1：没有接收到数据
 */
u8 NRF24L01_RxPacket(SPI_TypeDef *SPIx, u8 *rxbuf)
{
	u8 sta;		    							   
	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8); 																								//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(SPIx, STATUS);  																											//读取状态寄存器的值    	 
	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+STATUS,sta); 																				//清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)																																								//接收到数据
	{
		NRF24L01_Read_Buf(SPIx, RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);															  //读取数据
		NRF24L01_Write_Reg(SPIx, FLUSH_RX,0xff);																									//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;																																										//没收到任何数据
}

