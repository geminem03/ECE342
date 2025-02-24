/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "stdbool.h"

/* External variables --------------------------------------------------------*/
extern int8_t current_col;
volatile uint32_t last_interrupt = 0;
volatile uint32_t last_key_press = 0;
extern bool key_press;
extern bool key_release;
const uint32_t debounce_delay = 150;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  // Your code here
	
	if (__HAL_GPIO_EXTI_GET_FLAG(COL0_Pin)){
		// get current system time
		uint32_t current_time = HAL_GetTick();
		
		if(!key_press && (current_time - last_key_press > debounce_delay))
		{
			current_col = 0;
			key_press = true;
			key_release = true;
			last_key_press = current_time;
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(COL0_Pin);
		}
	}
  HAL_GPIO_EXTI_IRQHandler(COL0_Pin);
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
	
  // Your code here
	if (__HAL_GPIO_EXTI_GET_FLAG(COL1_Pin)){
		// get current system time
		uint32_t current_time = HAL_GetTick();
		
		if(!key_press && (current_time - last_key_press > debounce_delay))
		{
			current_col = 1;
			key_press = true;
			key_release = true;
			last_key_press = current_time;
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(COL1_Pin);
		}
	}
	else if (__HAL_GPIO_EXTI_GET_FLAG(COL2_Pin)){
		// get current system time
		uint32_t current_time = HAL_GetTick();
		
		if(!key_press && (current_time - last_key_press > debounce_delay))
		{
			current_col = 2;
			key_press = true;
			key_release = true;
			last_key_press = current_time;
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(COL2_Pin);
		}
	}
	else if (__HAL_GPIO_EXTI_GET_FLAG(COL3_Pin)){
		// get current system time
		uint32_t current_time = HAL_GetTick();
		
		if(!key_press && (current_time - last_key_press > debounce_delay))
		{
			current_col = 3;
			key_press = true;
			key_release = true;
			last_key_press = current_time;
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(COL3_Pin);
		}
	}
	
	
	/*
	
	// Array of column pins
	const uint16_t column_pins[4] = {COL0_Pin, COL1_Pin, COL2_Pin, COL3_Pin};

	// Iterate over columns to check for a key press
	for (int i = 1; i < 4; i++)
	{
			if (__HAL_GPIO_EXTI_GET_FLAG(column_pins[i]))
			{
					// Get current system time
					uint32_t current_time = HAL_GetTick();

					// Debounce logic
					if (!key_press && (current_time - last_key_press > debounce_delay))
					{
							current_col = i;           // Set the current column
							key_press = true;          // Mark the key as pressed
							key_release = true;        // Allow key release handling
							last_key_press = current_time; // Update the last press time
					}

					// Clear the interrupt flag for the column
					__HAL_GPIO_EXTI_CLEAR_FLAG(column_pins[i]);

					break; // Exit the loop since a column was detected
			}
	}
	*/

  HAL_GPIO_EXTI_IRQHandler(COL1_Pin);
  HAL_GPIO_EXTI_IRQHandler(COL2_Pin);
  HAL_GPIO_EXTI_IRQHandler(COL3_Pin);
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */


void EXTI15_10_IRQHandler(void)
{
  // get current system time
  uint32_t current_time = HAL_GetTick();

  // check if button was pressed
  if (__HAL_GPIO_EXTI_GET_FLAG(USER_Btn_Pin)) {
    if ((current_time - last_interrupt) > 20) { // 20ms debounce time
      HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin); // toggle green led

      // Update the last interrupt time
      last_interrupt = current_time;
    }
  }

  // Clear the interrupt flag
  HAL_GPIO_EXTI_IRQHandler(USER_Btn_Pin);
}

