#include "InterfacesConf.hh"
#include <SPIprotocol.hh>

using Drivers::BusStatus;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef*){
	Drivers::uart.handleFinish(nullptr);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c == &hi2c1){
		Drivers::i2c.handleError(nullptr);
	}
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c == &hi2c1){
		Drivers::i2c.handleAbort(nullptr);
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c == &hi2c1){
		Drivers::i2c.handleFinish(nullptr);
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c == &hi2c1){
		Drivers::i2c.handleFinish(nullptr);
	}
}

SPIprotocol spiProtocol;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == SLAVE_SELECT_Pin){
		if(GPIO_PIN_SET == HAL_GPIO_ReadPin(SLAVE_SELECT_GPIO_Port, SLAVE_SELECT_Pin)){
			HAL_SPI_Abort_IT(&hspi1);
			spiProtocol.unmarkUsedData();
		}else{
			HAL_SPI_TransmitReceive_DMA(&hspi1, spiProtocol.getTxData(), spiProtocol.getRxData(), spiProtocol.size());
			spiProtocol.markUsedData();
		}
	}else if(GPIO_Pin == MPU_IRQ_Pin){
		Drivers::gpio.setChangedPin(InterruptInputList::MPU_Irq);
		Drivers::gpio.handleFinish(nullptr);
	}else if(GPIO_Pin == ECHO_1_Pin){
		Drivers::gpio.setChangedPin(InterruptInputList::EchoIrq1);
		Drivers::gpio.handleFinish(nullptr);
	}else if(GPIO_Pin == ECHO_2_Pin){
		Drivers::gpio.setChangedPin(InterruptInputList::EchoIrq2);
		Drivers::gpio.handleFinish(nullptr);
	}else if(GPIO_Pin == ECHO_3_Pin){
		Drivers::gpio.setChangedPin(InterruptInputList::EchoIrq3);
		Drivers::gpio.handleFinish(nullptr);
	}else if(GPIO_Pin == SLAVE_SELECT_Pin){
		Drivers::gpio.setChangedPin(InterruptInputList::SS);
		Drivers::gpio.handleFinish(nullptr);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim11){
		Drivers::loopFlag40Hz = true;
		Drivers::loopCounter40Hz++;
		Drivers::timer.handleTimeEvent(nullptr);
	}else if(htim == &htim10){
		Drivers::loopFlag100Hz = true;
		Drivers::loopCounter100Hz++;
		Drivers::gpio.handleTimeEvent(nullptr);
		Drivers::i2c.handleTimeEvent(nullptr);
	}
}
