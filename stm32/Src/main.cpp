#include "main.h"
#include "stm32f4xx_hal.h"
#include <GPIO.hh>
#include <BothChannelEngineDriver.hh>
#include <US-015.hh>
#include <Log.hh>
#include <MPU6050.hh>
#include <ConfigurationProvider.hh>
#include <EncoderParser.hh>
#include <EncoderMenager.hh>
#include <AHRS.hh>
#include <PID.hh>
#include <US-015.hh>
#include <SPIparser.hh>
#include <StrategyManager.hh>
#include <BatteryObserver.hh>

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

#include"InterruptFunctions.hh"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART1_UART_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

using Drivers::gpio;

double batteyVoltage[2];
double batteyTuneVoltage[2];
uint32_t upperResistors_Ohm[3];
uint32_t lowerResistors_Ohm[3];

void functionCalledWhenExceptionIsThrowed(const char* text, unsigned int size){
  Drivers::gpio.pin(OutputList::LED_RED, false);
  HAL_Delay(500);
  Drivers::gpio.pin(OutputList::LED_RED, true);
  HAL_Delay(500);
  HAL_UART_Transmit(&huart1, (uint8_t*)"\n\r", 2, 10);
  HAL_UART_Transmit(&huart1, (uint8_t*)text, size, 100);
}

int main(void){
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_Base_Start_IT(&htim11);
  
  uint16_t adcData[4] = {0xfff, 0xfff};

  TIM4->CCR3 = 0;
  TIM4->CCR4 = 0;

  Configuration::Configuration configuration;
  bool akcelerometerAndGyroskopeDataReady = false;
  bool magnetometerDataReady = false;
  Drivers::gpio.pin(OutputList::StmReady, false);
  Drivers::timer.updateSignalParams(RCC_MAX_FREQUENCY, htim11.Init.Period, htim11.Init.Prescaler);
  Drivers::timer.setCounterReference(htim11.Instance->CNT);
  PlatformDrivers::Log log(&Drivers::uart, PlatformDrivers::LogingLevel::infoLogs, Drivers::loopCounter40Hz, UART_DATA_BUFFOR);
  ConfigurationProvider configurationProvider(log, 0xA0, &Drivers::i2c);
  if(!configurationProvider.run(configuration))
    THROW_invalid_argument("Configuration cannot be loaded.");
  Drivers::MPU6050 mpu(&Drivers::i2c, 0xD0, akcelerometerAndGyroskopeDataReady, magnetometerDataReady, HAL_Delay, Drivers::IMUsensorIfc::RottatedAxis::X);
  AHRS ahrs(mpu.akcelerometr, mpu.gyroskope, nullptr);
  Drivers::BothChannelEngineDriver<volatile uint32_t> engineRight(&Drivers::gpio, OutputList::EngineRightForward, OutputList::EngineRightBackward, &(TIM4->CCR3), htim4.Init.Period + 1, 0.5 * (htim4.Init.Period + 1));
  Drivers::BothChannelEngineDriver<volatile uint32_t> engineLeft(&Drivers::gpio, OutputList::EngineLeftForward, OutputList::EngineLeftBackward, &(TIM4->CCR4), htim4.Init.Period + 1, 0.5 * (htim4.Init.Period + 1));
  PlatformDrivers::EncoderParser<volatile uint32_t> encoderRight(TIM3->CNT, htim3.Init.Period, 32.0 * 120.0);
  PlatformDrivers::EncoderParser<volatile uint32_t> encoderLeft(TIM2->CNT, htim2.Init.Period, 32.0 * 120.0);
  PlatformDrivers::EncoderMenager encoderMenager(&encoderRight, &encoderLeft, {80.0, 125.0});
  Drivers::US_015 distanceFront(Drivers::timer, &Drivers::gpio, OutputList::Trig3, InputList::Echo3, InterruptInputList::EchoIrq3, HAL_Delay);
  Drivers::US_015 distanceRight(Drivers::timer, &Drivers::gpio, OutputList::Trig2, InputList::Echo2, InterruptInputList::EchoIrq2, HAL_Delay);
  Drivers::US_015 distanceLeft(Drivers::timer, &Drivers::gpio, OutputList::Trig1, InputList::Echo1, InterruptInputList::EchoIrq1, HAL_Delay);
  InterfaceSpiData spiInterfaceData = {log, configuration.spiKeyTx, configuration.spiKeyRx, configuration.mode, {}, {}, false, 0, nullptr, 0, nullptr, nullptr, 0};
  SPIparser spiParser(&spiProtocol, spiInterfaceData);
  PID pidEngineMove;
  PID pidEngineRotate;
  BatteryConfiguration batteryConfiguration{
    configuration.numberOfBatteryCells,
    configuration.upperResistors_Ohm,
    configuration.lowerResistors_Ohm,
    configuration.batteryAdcMaxWord,
    configuration.maxMeasuredVoltage
  };
  BatteryObserver batteryObserver(log, adcData, batteryConfiguration);
  PlatformDrivers::DriversPlatform driversPlatform{
    &engineRight,
    &engineLeft,
    &encoderMenager,
    &distanceFront,
    &distanceRight,
    &distanceLeft,
    &Drivers::gpio,
    &ahrs,
    &pidEngineMove,
    &pidEngineRotate,
    &batteryObserver
  };
  PlatformDrivers::ModeManager manager(driversPlatform, spiInterfaceData);

  pidEngineMove.P = -configuration.pidForward.p;
  pidEngineMove.I = -configuration.pidForward.i;
  pidEngineMove.D = -configuration.pidForward.d;
  pidEngineMove.AntiWindup = -configuration.pidForward.antiWindUp;
  pidEngineMove.setLimit(-50.0, 50.0);
  pidEngineRotate.P = configuration.pidRotation.p;
  pidEngineRotate.I = configuration.pidRotation.i;
  pidEngineRotate.D = configuration.pidRotation.d;
  pidEngineRotate.AntiWindup = configuration.pidRotation.antiWindUp;
  pidEngineRotate.AntiWindup = 0.0;
  pidEngineRotate.setLimit(-50.0, 50.0);
  HAL_Delay(10);
	if(mpu.init())
    log.info("MPU initialised succesfuly.");
  else{
    log.error("MPU cannot be initialised.");
  }
  distanceFront.init();
  distanceRight.init();
  distanceLeft.init();
  gpio.pin(OutputList::LED_YELLOW, true);
  gpio.pin(OutputList::LED_BLUE, true);

  engineRight.set(0);
  engineLeft.set(0);

  upperResistors_Ohm[0] = configuration.upperResistors_Ohm[0];
  lowerResistors_Ohm[0] = configuration.lowerResistors_Ohm[0];
  upperResistors_Ohm[1] = configuration.upperResistors_Ohm[1];
  lowerResistors_Ohm[1] = configuration.lowerResistors_Ohm[1];
  upperResistors_Ohm[2] = configuration.upperResistors_Ohm[2];
  lowerResistors_Ohm[2] = configuration.lowerResistors_Ohm[2];
  Drivers::gpio.pin(OutputList::StmReady, true);
  while(true){
    if(akcelerometerAndGyroskopeDataReady){
      akcelerometerAndGyroskopeDataReady = false;
      ahrs.run();
      // roll = ahrs[0];
      // pitch = ahrs[1];
      // yawl = ahrs[2];
    }
    if(Drivers::loopFlag100Hz){
      Drivers::loopFlag100Hz = false;
      gpio.pin(OutputList::LED_GREEN, Drivers::loopCounter100Hz % 10 != 0);
      gpio.pin(OutputList::LED_BLUE, !spiProtocol.isReadyToUse());
      manager.runPlatformLoop();
      spiParser.run();
    }
    if(Drivers::loopFlag40Hz){
      Drivers::loopFlag40Hz = false;
      gpio.pin(OutputList::LED_RED, Drivers::loopCounter40Hz % 10 != 0);
      manager.runSpiInterfaceLoop();
      batteyVoltage[0] = batteryObserver.batteryVoltageVal(0);
      batteyVoltage[1] = batteryObserver.batteryVoltageVal(1);
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, 4);
    }
  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(true){}
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    while(true){}
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    while(true){}
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    while(true){}
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    while(true){}
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    while(true){}
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    while(true){}
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    while(true){}
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    while(true){}
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_Encoder_InitTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 16000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 15;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    while(true){}
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    while(true){}
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_Encoder_InitTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 16000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 15;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    while(true){}
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    while(true){}
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 19;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 49999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    while(true){}
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    while(true){}
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    while(true){}
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    while(true){}
  }

  HAL_TIM_MspPostInit(&htim4);

}

/* TIM10 init function */
static void MX_TIM10_Init(void)
{

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 19;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 49999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    while(true){}
  }

}

/* TIM11 init function */
static void MX_TIM11_Init(void)
{

  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 49;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 49999;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    while(true){}
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    while(true){}
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, ENGINE_L_IN1_Pin|ENGINE_L_IN2_Pin|LED_3_Pin|TRIG_2_Pin 
                          |TRIG_3_Pin|TRIG_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ENGINE_P_IN2_Pin|ENGINE_P_IN1_Pin|LED_2_Pin|LED_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, STM_READY_Pin|LED_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ENGINE_L_IN1_Pin ENGINE_L_IN2_Pin LED_3_Pin TRIG_2_Pin 
                           TRIG_3_Pin TRIG_1_Pin */
  GPIO_InitStruct.Pin = ENGINE_L_IN1_Pin|ENGINE_L_IN2_Pin|LED_3_Pin|TRIG_2_Pin 
                          |TRIG_3_Pin|TRIG_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ENGINE_P_IN2_Pin ENGINE_P_IN1_Pin LED_2_Pin LED_1_Pin */
  GPIO_InitStruct.Pin = ENGINE_P_IN2_Pin|ENGINE_P_IN1_Pin|LED_2_Pin|LED_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SLAVE_SELECT_Pin */
  GPIO_InitStruct.Pin = SLAVE_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SLAVE_SELECT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ECHO_2_Pin MPU_IRQ_Pin */
  GPIO_InitStruct.Pin = ECHO_2_Pin|MPU_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STM_READY_Pin LED_4_Pin */
  GPIO_InitStruct.Pin = STM_READY_Pin|LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMORY_SELECT_Pin */
  GPIO_InitStruct.Pin = MEMORY_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMORY_SELECT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ECHO_3_Pin ECHO_1_Pin */
  GPIO_InitStruct.Pin = ECHO_3_Pin|ECHO_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char*, int)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
