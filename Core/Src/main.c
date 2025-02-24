#include "main.h"

#include "config.h"
#include "ov7670.h"

#include <stdio.h>
#include <string.h>

// OneDrive\Skule_Year4\ECE342\labs\lab5\lab5-serial-monitor-exec>
// .\serial_monitor.exe -p COM5

#define VIDEO_FRAME_COUNT 30  // Number of frames to capture for video
#define FRAME_DELAY_MS 10    // Delay between frames (adjust for FPS)

uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];

uint8_t dma_flag = 0; // Global flag for DMA completion
uint8_t recording = 0; // Flag to toggle video recording mode

void print_buf(void);
void send_uart_data(uint8_t *data, uint16_t size);
void send_grayscale_image(uint8_t color);
void send_single_frame(uint16_t *buff);
void record_video(void); 

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

  while (1)
  {	
    // Check if user button is pressed to toggle video recording mode
    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
      
      if (!recording) {
        recording = 1;
        record_video();
      } 
			else {recording = 0;}
    }
  }
}

// Function to capture and send video frames
void record_video(void) {
  char msg[50];

  for (int frame = 0; frame < VIDEO_FRAME_COUNT && recording; frame++) {

    dma_flag = 0;  // Reset flag before capture

    // Start image capture
    ov7670_capture(snapshot_buff);
    HAL_NVIC_SetPendingIRQ(DMA2_Stream1_IRQn);  // Need to call manually
		
		
    // Wait until DMA transfer is complete
    while (!dma_flag) {
      HAL_Delay(10);
    }
		
    // Send the frame
    send_single_frame(snapshot_buff);

    // Delay between frames (simulating FPS)
    HAL_Delay(FRAME_DELAY_MS);
  }

}

// Send image data via UART
void send_uart_data(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart3, data, size, HAL_MAX_DELAY);
}

// Send full grayscale image
void send_grayscale_image(uint8_t color)
{
    const char *preamble_msg = "\r\nPREAMBLE!\r\n";
    send_uart_data((uint8_t *)preamble_msg, strlen(preamble_msg));

    for (int i = 0; i < IMG_ROWS * IMG_COLS; i++) {
        send_uart_data(&color, 1);
    }
    const char *preamble_msg1 = "\r\nSTOP!\r\n";
    send_uart_data((uint8_t *)preamble_msg1, strlen(preamble_msg1));
}

// Send a single frame (grayscale Y-channel only)
void send_single_frame(uint16_t *buff){
    const char *preamble_msg = "\r\nPREAMBLE!\r\n";
    send_uart_data((uint8_t *)preamble_msg, strlen(preamble_msg));

    for (int i = 1; i < (IMG_ROWS * IMG_COLS); i ++) {  // Only send odd-indexed bytes
        uint16_t y_value = buff[i] & 0xFF00;
				uint8_t y = y_value >> 8;			// Extract lower 8 bits (Y channel)
        send_uart_data(&y, 1);
    }

    const char *preamble_msg1 = "\r\nSTOP!\r\n";
    send_uart_data((uint8_t *)preamble_msg1, strlen(preamble_msg1));
}
