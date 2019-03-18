/**
  ******************************************************************************
  * @file    nrf2401.c
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.22
  * @note    �˳���ΪARM��nrf2401�ĵײ��������
  ******************************************************************************
  */

#include "sysinc.h"


 /*�շ���ַ*/
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 


 /**
 * @brief    		NRF2401��ʼ������
 * @code
 *      				// ��ʼ��Spi1
 *       				NRF24L01_Init(Spi1);
 * @endcode
 * @param[in]   Spix : Spiģ��ѡ�񣬿�ѡֵ��Spi1��Spi2��Spi3
 */
void NRF24L01_Init(int Spix)
{
  SPI_QuickInit(Spix, CPOL_LOW_CPHA_1, SPI_BaudRatePrescaler_16, SPI_NSS_Soft);
	NRF24L01_CE=0; 																																						//ʹ��24L01
	NRF24L01_CSN=1;																																						//SPIƬѡȡ��  
	SPI_SetSpeed(SPIcs,SPI_BaudRatePrescaler_8);
	EXTI_QuickInit(HW_EXTIB, EXTI_Pin_6, 3, 3);

	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_6, GPIO_Mode_IPD);																			//��ʼ����LED���ӵ�Ӳ���ӿ�
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
 * @brief  			NRF2401�жϺ���
 * @code
 *      			  ��ʼ��extiA��pin10�������ȼ�preepri=2,��Ӧ���ȼ�subpri=2,ʹ���ж�ITsta=ENABLE
 *      				NRF24L01_ITinit(HW_EXTIA, EXTI_Pin_10, 2, 2);
 * @endcode
 * @param[in]   instance ģ���
 *              @arg HW_EXTIA A�˿�
 *              @arg HW_EXTIB B�˿�
 *              @arg HW_EXTIC C�˿�
 *              @arg HW_EXTID D�˿�
 *              @arg HW_EXTIE E�˿�
 * @param[in]   EXTI_Pin_x ��xΪGPIO���źţ���ѡֵ 0,1,2....15
 * @param[in]   preepri ��ռ���ȼ�����ѡֵ0,1,2,3
 * @param[in]   subpri  ��Ӧ���ȼ�����ѡֵ0,1,2,3
 */
void NRF24L01_ITinit(int instance, int EXTI_Pin_x,int preepri, int subpri)
{
	EXTI_QuickInit(instance, EXTI_Pin_x, preepri, subpri);
}



 /**
 * @brief    		NRF2401����Ϊ����ģʽ
 * @code
 *      				����SPI1ΪNRF2401�Ľ���ģ��
 *       				NRF24L01_RX_Mode(SPI1)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 */
void NRF24L01_RX_Mode(SPI_TypeDef *SPIx)
{
  	NRF24L01_Write_Reg(SPIcs, WRITE_REG_NRF+CONFIG, 0x08);																	//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	  EXTI_IRQcmd(HW_EXTIB, EXTI_Pin_6,ENABLE);

	  NRF24L01_CE=0;	  
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);				//дRX�ڵ��ַ
	  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_AA,0x01);    																	//ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_RXADDR,0x01);																	//ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_CH,40);	     																	//����RFͨ��Ƶ��		  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);												//ѡ��ͨ��0����Ч���ݿ�� 	    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_SETUP,0x0f);																	//����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG, 0x0f);																		//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	NRF24L01_CE = 1; //CEΪ��,�������ģʽ 
}


 /**
 * @brief    		NRF2401����Ϊ����ģʽ
 * @code
 *      				����SPI1ΪNRF2401�Ľ���ģ��
 *       				NRF24L01_TX_Mode(SPI1)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 */
void NRF24L01_TX_Mode(SPI_TypeDef *SPIx)
{
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG, 0x08);																//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
		EXTI_IRQcmd(HW_EXTIB, EXTI_Pin_6,DISABLE);

		NRF24L01_CE=0;	    
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);				//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); 		//����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_AA,0x01);     														//ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+EN_RXADDR,0x01); 														//ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+SETUP_RETR,0x1a);														//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_CH,40);      														  //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+RF_SETUP,0x0f); 														  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+CONFIG,0x0e);    														//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	  NRF24L01_CE=1;//CEΪ��,10us����������
}


 /**
 * @brief    		NRF2401д����
 * @code
 *      				����SPI1��д��Ĵ�����ַΪ0x01,����a[12],����Ϊ12
 *       				NRF24L01_Write_Buf(SPI1 ,0x01, a, 12)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   reg :  �Ĵ�����ַ
 * @param[in]   pBuf : ���ݵ�ַ
 * @param[in]   len :  ���ݳ���
 * @return			����ֵ:������״ֵ̬
 */
u8 NRF24L01_Write_Buf(SPI_TypeDef *SPIx ,u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;	    
 	  NRF24L01_CSN = 0;          																												//ʹ��SPI����
  	status = SPI_ReadWriteByte(SPIx ,reg);																						//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_ReadWriteByte(SPIx ,*pBuf++); 							//д������	 
  	NRF24L01_CSN = 1;       																													//�ر�SPI����
  	return status;          																													//���ض�����״ֵ̬
}


 /**
 * @brief    		NRF2401������
 * @code
 *      				����SPI1�����Ĵ�����ַΪ0x01,����a[12],����Ϊ12
 *       				NRF24L01_Read_Buf(SPI1 ,0x01, a, 12)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   reg :  �Ĵ�����ַ
 * @param[in]   pBuf : ���ݵ�ַ
 * @param[in]   len :  ���ݳ���
 * @return			����ֵ:������״ֵ̬
 */
u8 NRF24L01_Read_Buf(SPI_TypeDef *SPIx ,u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           																																//ʹ��SPI����
  	status=SPI_ReadWriteByte(SPIx, reg);																												//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++) pBuf[u8_ctr]=SPI_ReadWriteByte(SPIx, 0XFF);								//��������
  	NRF24L01_CSN=1;       																																			//�ر�SPI����
  	return status;       																																			  //���ض�����״ֵ̬
}


 /**
 * @brief    		NRF2401���Ĵ���
 * @code
 *      				����SPI1�����Ĵ�����ַΪ0x01
 *       				NRF24L01_Read_Reg(SPI1, 0x01)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   reg :  �Ĵ�����ַ
 * @return			����ֵ:�����ļĴ���ֵ
 */
u8 NRF24L01_Read_Reg(SPI_TypeDef *SPIx, u8 reg)
{
	  u8 reg_val;	    
 	  NRF24L01_CSN = 0;          																																	//ʹ��SPI����		
  	SPI_ReadWriteByte(SPIx, reg);   																														//���ͼĴ�����
  	reg_val=SPI_ReadWriteByte(SPIx, 0XFF);																											//��ȡ�Ĵ�������
  	NRF24L01_CSN = 1;          																																	//��ֹSPI����		    
  	return(reg_val);       
}

 /**
 * @brief    		NRF2401д�Ĵ���
 * @code
 *      				����SPI1��д�Ĵ�����ַΪ0x01
 *       				NRF24L01_Write_Reg(SPI1, 0x01)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   reg :  �Ĵ�����ַ
 * @return			����ֵ:������״ֵ̬
 */
u8 NRF24L01_Write_Reg(SPI_TypeDef *SPIx, u8 reg, u8 value)
{
		u8 status;	
   	NRF24L01_CSN=0;                 																														 //ʹ��SPI����
  	status =SPI_ReadWriteByte(SPIx, reg);																												 //���ͼĴ����� 
  	SPI_ReadWriteByte(SPIx, value);     																												 //д��Ĵ�����ֵ
  	NRF24L01_CSN=1;                																															 //��ֹSPI����	   
  	return(status);       																																		   //����״ֵ̬
}

 /**
 * @brief    		NRF2401����Ƿ����
 * @code
 *      				����SPI1
 *       				NRF24L01_Check(SPI1)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @return			����0����ʼ����ȷ   1����ʼ������
 */
u8 NRF24L01_Check(SPI_TypeDef *SPIx)
{
  u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8); 																									//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(SPIx, WRITE_REG_NRF+TX_ADDR,buf,5);																				//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(SPIx, TX_ADDR,buf,5); 																											//����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;																																							//���24L01����	
	return 0;		 																																									//��⵽24L01
}


 /**
 * @brief    		NRF2401�������ݰ�
 * @code
 *      				����SPI1��д����Ϊadress[2];
 *       				NRF24L01_TxPacket(SPI1, adress)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   txbuf: ���ݵ�ַ
 * @return			����ֵ: MAX_TX:�ﵽ����ط�����    TX_OK:�������     0xff:����ԭ����ʧ��
 */
u8 NRF24L01_TxPacket(SPI_TypeDef *SPIx, u8 *txbuf)
{
	NRF24L01_TX_Mode(SPIx);
	u8 sta;
 	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8);																							//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(SPIx, WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);																//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE=1;//��������
	
	while(NRF24L01_IRQ != 0);																																	//�ȴ��������
	sta=NRF24L01_Read_Reg(SPIx, STATUS);  																										//��ȡ״̬�Ĵ�����ֵ	   

	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+STATUS,sta); 																			//���TX_DS��MAX_RT�жϱ�־
	NRF24L01_CE=0;
	if(sta&MAX_TX)
	{
		NRF24L01_Write_Reg(SPIx, FLUSH_TX,0xff);																								//���TX FIFO�Ĵ��� 
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
 * @brief    		NRF2401�������ݰ�
 * @code
 *      				����SPI1��������Ϊadress[2];
 *       				NRF24L01_RxPacket(SPI1, adress)��
 * @endcode
 * @param[in]   SPIx : SPIģ��ѡ�񣬿�ѡֵ��SPI1��SPI2��SPI3
 * @param[in]   txbuf: ���ݵ�ַ
 * @return			����ֵ: 0�����յ�����    1��û�н��յ�����
 */
u8 NRF24L01_RxPacket(SPI_TypeDef *SPIx, u8 *rxbuf)
{
	u8 sta;		    							   
	SPI_SetSpeed(SPIx, SPI_BaudRatePrescaler_8); 																								//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(SPIx, STATUS);  																											//��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(SPIx, WRITE_REG_NRF+STATUS,sta); 																				//���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)																																								//���յ�����
	{
		NRF24L01_Read_Buf(SPIx, RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);															  //��ȡ����
		NRF24L01_Write_Reg(SPIx, FLUSH_RX,0xff);																									//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;																																										//û�յ��κ�����
}

