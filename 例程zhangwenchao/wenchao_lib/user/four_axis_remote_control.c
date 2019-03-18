/**
  ******************************************************************************
  * @file    			four_axis_aircraft.c
  * @author  			张文超 
	* @qq      			269426626
  * @version 			V1.0
  * @date    			2016.10.5
  * @note    			此程序四轴遥控程序
  * @config_adc   adc1
  *               @arg ADC_CH0                 PA0         //左摇杆X轴信号采集
  *               @arg ADC_CH1                 PA1         //左摇杆Y轴信号采集
  *               @arg ADC_CH2                 PA2         //右摇杆X轴信号采集
  *               @arg ADC_CH3                 PA3         //右摇杆X轴信号采集
  *               @arg ADC_CH4                 PA4				 //电量采集	
  * @config_iic   模拟iic(OLED12864)
  *               @arg IIC_SCL                 PB10
  *               @arg IIC_SDA                 PB11
  * @config_in    gpio输入
  *               @arg KEY1                    PA5         //左摇杆Z轴按键
  *               @arg KEY2                    PA6         //右摇杆Z轴按键
  * @config_spi   spi2(nrf2401)
  *               @arg NRF24L01_CE             PB4
  *               @arg NRF24L01_IRQ            PB6
  *               @arg NRF24L01_CSN            PB12
  *               @arg NRF24L01_SCK            PB13
  *               @arg NRF24L01_MISO           PB14
  *               @arg NRF24L01_MOSI           PB15  
  * @config_led   指示灯
  *               @arg 运行指示灯              PC13
  *               @arg 电量低指示灯            PA12
  * @config_uart  uart1
  *               @arg UART1_TX                PA9     
  *               @arg UART1_RX                PA10     
  * @config_boot  boot
  *               @arg BOOT0                   44号引脚     
  *               @arg BOOT1                   PB2     
  ******************************************************************************
  */
