/**
  ******************************************************************************
  * @file    			four_axis_aircraft.c
  * @author  			���ĳ� 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.5
  * @note    			�˳�������ң�س���
  * @config_adc   adc1
  *               @arg ADC_CH0                 PA0         //��ҡ��X���źŲɼ�
  *               @arg ADC_CH1                 PA1         //��ҡ��Y���źŲɼ�
  *               @arg ADC_CH2                 PA2         //��ҡ��X���źŲɼ�
  *               @arg ADC_CH3                 PA3         //��ҡ��X���źŲɼ�
  *               @arg ADC_CH4                 PA4				 //�����ɼ�	
  * @config_iic   ģ��iic(OLED12864)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_in    gpio����
  *               @arg KEY1                    PA5         //��ҡ��Z�ᰴ��
  *               @arg KEY2                    PA6         //��ҡ��Z�ᰴ��
  * @config_spi   spi2(nrf2401)
  *               @arg NRF24L01_CE             PB4
  *               @arg NRF24L01_IRQ            PB6
  *               @arg NRF24L01_CSN            PB12
  *               @arg NRF24L01_SCK            PB13
  *               @arg NRF24L01_MISO           PB14
  *               @arg NRF24L01_MOSI           PB15  
  * @config_led   ָʾ��
  *               @arg ����ָʾ��              PC13
  *               @arg ������ָʾ��            PA12
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44������     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */
