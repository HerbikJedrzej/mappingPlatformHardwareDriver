/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define ENGINE_L_IN1_Pin GPIO_PIN_14
#define ENGINE_L_IN1_GPIO_Port GPIOC
#define ENGINE_L_IN2_Pin GPIO_PIN_15
#define ENGINE_L_IN2_GPIO_Port GPIOC
#define ENGINE_P_IN2_Pin GPIO_PIN_2
#define ENGINE_P_IN2_GPIO_Port GPIOA
#define ENGINE_P_IN1_Pin GPIO_PIN_3
#define ENGINE_P_IN1_GPIO_Port GPIOA
#define SLAVE_SELECT_Pin GPIO_PIN_4
#define SLAVE_SELECT_GPIO_Port GPIOA
#define SLAVE_SELECT_EXTI_IRQn EXTI4_IRQn
#define LED_3_Pin GPIO_PIN_4
#define LED_3_GPIO_Port GPIOC
#define TRIG_2_Pin GPIO_PIN_5
#define TRIG_2_GPIO_Port GPIOC
#define ECHO_2_Pin GPIO_PIN_0
#define ECHO_2_GPIO_Port GPIOB
#define ECHO_2_EXTI_IRQn EXTI0_IRQn
#define STM_READY_Pin GPIO_PIN_1
#define STM_READY_GPIO_Port GPIOB
#define MEMORY_SELECT_Pin GPIO_PIN_12
#define MEMORY_SELECT_GPIO_Port GPIOB
#define MPU_IRQ_Pin GPIO_PIN_13
#define MPU_IRQ_GPIO_Port GPIOB
#define MPU_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define LED_4_Pin GPIO_PIN_14
#define LED_4_GPIO_Port GPIOB
#define TRIG_3_Pin GPIO_PIN_8
#define TRIG_3_GPIO_Port GPIOC
#define ECHO_3_Pin GPIO_PIN_9
#define ECHO_3_GPIO_Port GPIOC
#define ECHO_3_EXTI_IRQn EXTI9_5_IRQn
#define LED_2_Pin GPIO_PIN_11
#define LED_2_GPIO_Port GPIOA
#define LED_1_Pin GPIO_PIN_12
#define LED_1_GPIO_Port GPIOA
#define TRIG_1_Pin GPIO_PIN_11
#define TRIG_1_GPIO_Port GPIOC
#define ECHO_1_Pin GPIO_PIN_12
#define ECHO_1_GPIO_Port GPIOC
#define ECHO_1_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() while(true){}

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
