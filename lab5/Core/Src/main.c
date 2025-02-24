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
#include "ov7670.h"

#include <stdio.h>
#include <string.h>

//OneDrive\Skule_Year4\ECE342\labs\lab5\lab5-serial-monitor-exec>
// .\serial_monitor.exe -p COM5

uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];

uint8_t dma_flag = 0; // global flag for 3.4 snapshot


void print_buf(void);
void send_uart_data(uint8_t *data, uint16_t size);
void send_grayscale_image(uint8_t color);
void send_single_frame(uint16_t *buff);

int main(void)
{
  /* Reset of all peripherals */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();

  char msg[100];

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  ov7670_init();

  static uint16_t image_buffer[IMG_ROWS * IMG_COLS] __attribute__((aligned(32)));
	
	//send_grayscale_image(0xFF);  
		
  while (1)
  {	
		
    // Your code here
    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
			HAL_Delay(100);  // Debounce
			
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // for debugging
			HAL_Delay(500);
			
			sprintf(msg, "Capturing image...\n");
			print_msg(msg);
			dma_flag = 0;  // reset flag before capture
			

			// start image capture
			ov7670_snapshot(image_buffer);
			HAL_NVIC_SetPendingIRQ(DMA2_Stream1_IRQn); // need to call manually 
			
			// wait until DMA transfer is complete
			while (!dma_flag) {
				HAL_Delay(10);
			}
			
			//send_grayscale_image(0xFF); 
			send_single_frame(image_buffer);
			
			sprintf(msg,"\nCapture complete!\n");
			print_msg(msg);
		}
	
  }
	
}

// send data via UART without blocking
void send_uart_data(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart3, data, size, HAL_MAX_DELAY);
}

// grayscale image with 8-bit color
void send_grayscale_image(uint8_t color)
{
    // preamble message 
    const char *preamble_msg = "\r\nPREAMBLE!\r\n";
    send_uart_data((uint8_t *)preamble_msg, strlen(preamble_msg));

    // send image data
    for (int i = 0; i < 144 * 174; i++) {
        send_uart_data(&color, 1);
    }
}

void send_single_frame(uint16_t *buff) {
    // preamble message 
    const char *preamble_msg = "\r\nPREAMBLE!\r\n";
    send_uart_data((uint8_t *)preamble_msg, strlen(preamble_msg));

    // only send y component in odd index
    for (int i = 0; i < (IMG_ROWS * IMG_COLS); i++) {
        uint8_t y_value = buff[i] & 0xFF;  // Extract the lower 8 bits (Y channel)
        send_uart_data(&y_value, 1);
    }

    print_msg("Image sent successfully!\r\n");
}


void print_buf() {
  // Send image data through serial port.

  // Your code here
}
