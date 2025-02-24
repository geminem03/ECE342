/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "config.h"
#include <stdio.h>
#include "stdbool.h"

int8_t current_row = -1, current_col = -1;
bool key_press = false;
bool key_release = true;

// Keypad mapping: rows 0-3 (top to bottom), columns 3-0 (left to right)
const char keypad[4][4] = {
    {'3', '2', '1', '0'}, // Row 0
    {'7', '6', '5', '4'}, // Row 1
    {'B', 'A', '9', '8'}, // Row 2
    {'F', 'E', 'D', 'C'}  // Row 3
};


int main(void)
{
  /* Reset of all peripherals. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();

  char message[100];

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  
  /* Initialize ROW outputs to LOW */
  HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_RESET); // changet SET to RESET 
  HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
	

  /* Infinite loop */
	
  while (1){	
		for(int i = 0; i < 4; i++){
			// set all rows to LOW each time
      HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
			
			switch (i)
        {
        case 0:
            HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
            break;
        default:
            break;
        }
				
				current_row = i; // update active row
        HAL_Delay(10);   // stabilize voltage
				
				// print message only if pressed & released 
        if (key_press && key_release){
					// Get the corresponding key from the keypad mapping
					char key = keypad[current_row][current_col];

					// Format and print the message with the key
					sprintf(message, "Key pressed: %c\n", key);
					print_msg(message);
					
					key_release = false;

       }
				
			 // Check if the key is released (all columns are LOW)
        if (HAL_GPIO_ReadPin(COL0_GPIO_Port, COL0_Pin) == GPIO_PIN_RESET &&
            HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin) == GPIO_PIN_RESET &&
            HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin) == GPIO_PIN_RESET &&
            HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin) == GPIO_PIN_RESET)
        {
            key_press = false;   // reset flag
            key_release = true;  // allow new presses
        }
		}
  }
	
}