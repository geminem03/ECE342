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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "fixedpt.h"

#define DAC_SIZE 4095     // 12-bit 
#define TWO_PI 6.28
#define LUT_SIZE 628     // LUT size (same as number of points)
#define CLOCK_FREQ 100000000 //100 MHz
#define HARMONIC_COUNT 10  // Number of harmonics to approximate square wave
#define PI 3.14159265358979

#define SINE_STEPS 629
#define SQUARE_TERMS 50  // Number of terms for Fourier series approximation

uint32_t sine_wave_lut[SINE_STEPS];
uint32_t square_wave_lut[SINE_STEPS];
int square_terms = 10;

void print_msg(char * msg);

void generate_sin_LUT() {
  for (int i = 0; i < SINE_STEPS; i++) {
    sine_wave_lut[i] =
        (uint32_t)((sin((2 * PI * i) / SINE_STEPS) + 1) * 2047);
  }
}

void output_sine_using_math() {
  for (int i = 0; i < SINE_STEPS; i++) {
    uint32_t dac_value =
        (uint32_t)((sin((2 * PI * i) / SINE_STEPS) + 1) * 2047);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    // HAL_Delay(10); // Adjust delay for desired frequency
  }
}

void output_sine_using_LUT() {
  for (int i = 0; i < SINE_STEPS; i++) {
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sine_wave_lut[i]);
    // HAL_Delay(10);
  }
}

void generate_square_LUT() {
  for (int i = 0; i < SINE_STEPS; i++) {
    float sum = 0;
    for (int n = 0; n < square_terms; n++) {
      int k = 2 * n + 1;
      sum += sin(k * (2 * PI * i) / SINE_STEPS) / k;
    }
    square_wave_lut[i] =
        (uint32_t)((sum * 4 / PI + 1) * 2047);  // Scale to 12-bit DAC
  }
}

void output_square_using_math() {
  for (int i = 0; i < SINE_STEPS; i++) {
    float sum = 0;
    for (int n = 0; n < square_terms; n++) {
      int k = 2 * n + 1;
      sum += sin(k * (2 * PI * i) / SINE_STEPS) / k;
    }
    uint32_t dac_value = (uint32_t)((sum * 4 / PI + 1) * 2047);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    // HAL_Delay(10);
  }
}

void output_square_using_LUT() {
  for (int i = 0; i < SINE_STEPS; i++) {
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, square_wave_lut[i]);
    HAL_Delay(10);
  }
}

// part 4
fixedpt raw_sine_lut_fixed[SINE_STEPS];   // Stores raw sin(angle)
fixedpt sine_wave_lut_fixed[SINE_STEPS];  // Scaled for DAC
fixedpt square_wave_lut_fixed[SINE_STEPS];

void generate_raw_sine_LUT_fixed() {
  for (int i = 0; i < SINE_STEPS; i++) {
    float angle = (2.0f * PI * i) / SINE_STEPS;
    float sin_val = sin(angle);
    raw_sine_lut_fixed[i] = FXD_FROM_FLOAT(sin_val);
  }
}

void generate_sin_LUT_fixed() {
  for (int i = 0; i < SINE_STEPS; i++) {
    fixedpt scaled = FXD_ADD(raw_sine_lut_fixed[i], FIXEDPT_ONE);
    scaled = FXD_MUL(scaled, FXD_FROM_INT(2047));
    sine_wave_lut_fixed[i] = scaled;
  }
}

void generate_square_LUT_fixed() {
  fixedpt four_over_pi = FXD_FROM_FLOAT(4.0 / PI);
  for (int i = 0; i < SINE_STEPS; i++) {
    fixedpt sum = 0;
    for (int n = 0; n < SQUARE_TERMS; n++) {
      int k = 2 * n + 1;
      int index = (i * k) % SINE_STEPS;
      fixedpt term = FXD_DIV(raw_sine_lut_fixed[index], FXD_FROM_INT(k));
      sum = FXD_ADD(sum, term);
    }
    sum = FXD_MUL(sum, four_over_pi);
    sum = FXD_ADD(sum, FIXEDPT_ONE);
    sum = FXD_MUL(sum, FXD_FROM_INT(2047));
    square_wave_lut_fixed[i] = sum;
  }
}

void output_sine_using_LUT_fixed() {
  for (int i = 0; i < SINE_STEPS; i++) {
    uint32_t dac_value = FXD_TO_INT(sine_wave_lut_fixed[i]);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
  }
}

void output_square_using_LUT_fixed() {
  for (int i = 0; i < SINE_STEPS; i++) {
    uint32_t dac_value = FXD_TO_INT(square_wave_lut_fixed[i]);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
  }
}

int main(void) {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  uint32_t counterStart = __HAL_TIM_GET_COUNTER(&htim6);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  MX_USART3_Init();
  HAL_DAC_Init(&hdac);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

  HAL_TIM_Base_Start(&htim6);

  generate_sin_LUT();
  generate_square_LUT();

  // part 4
  //generate_raw_sine_LUT_fixed();
  //generate_sin_LUT_fixed();
  //generate_square_LUT_fixed();
	
	 uint32_t start = __HAL_TIM_GET_COUNTER(&htim6);
	 
	 char message[100];

  while (1) {
    //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

   
    // output_sine_using_math(); // Comment this to use LUT
		
    // output_sine_using_LUT(); // Uncomment to use LUT
		
    // output_square_using_math(); // Comment this to use LUT
		
    // output_square_using_LUT(); // Uncomment to use LUT

    // part 4: Measure execution cycles 
		
    uint32_t start_time = __HAL_TIM_GET_COUNTER(&htim6);
		// insert fuction to measure here: 
		output_square_using_math(); 
		
		uint32_t end_time = __HAL_TIM_GET_COUNTER(&htim6);
		uint32_t cycles_elapsed;
		if (end_time >= start) {
				// No rollover, simple subtraction
				cycles_elapsed = end_time - start;
		} else {
				// Timer rolled over, correct the calculation
				cycles_elapsed = (0xFFFF - start_time) + end_time + 1;
		}
		
		sprintf(message, "num cycles = %d\n", cycles_elapsed);
		print_msg(message);

		
  }
}