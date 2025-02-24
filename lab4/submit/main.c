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
 #include "dfr0151.h"
 #include "config.h"
 #include <stdio.h>
 #include <string.h>
 
 /* Global Variables */
 uint8_t hour, min, sec;
 uint8_t week_day, day, month, year;
 
 uint8_t alarm_hour, alarm_min, alarm_sec;
 uint8_t alarm_weekday, alarm_day, alarm_month, alarm_year;
 
 uint8_t* rtc_values[7] = {&hour,&min, &sec, &week_day, &day, &month, &year};
 uint16_t rtc_addresses[7] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
 
 uint8_t* alarm_values[7] = {&alarm_hour, &alarm_min, &alarm_sec, &alarm_weekday, &alarm_day, &alarm_month, &alarm_year};
 uint16_t alarm_addresses[7] = {0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD};
 
 uint8_t eeprom_flag = 0;
 
 /* Function Prototypes */
 void test_binary_conversions(uint8_t value);
 void print_date_and_time(uint8_t hour, uint8_t min, uint8_t sec,
                          uint8_t week_day, uint8_t day, uint8_t month, uint8_t year);
 void print_alarm(uint8_t alarm_hour, uint8_t alarm_min, 
                                     uint8_t alarm_sec, uint8_t alarm_week_day, 
                                     uint8_t alarm_day, uint8_t alarm_month,  uint8_t alarm_year);
 
 int main(void)
 {
   /* Reset of all peripherals. */
   HAL_Init();
 
   /* Configure the system clock */
   SystemClock_Config();
 
   /* Initialize all configured peripherals */
   MX_GPIO_Init();
   MX_I2C1_Init();
   MX_USART3_UART_Init();
   MX_USB_OTG_FS_PCD_Init();  
   rtc_init();
 
   char msg[100];
     
     sprintf(msg, "######## RESET PROGRAM ###############\n");
     print_msg(msg);
     
     for (int i = 0; i < 7; i++) 
     {
       if (eeprom_read(rtc_addresses[i]) == 0xFF) 
             {
                     eeprom_write(rtc_addresses[i], 0, 1); // write 0 to all rtc addresses if they are empty
       } 
             else 
             { 
                 *rtc_values[i] = eeprom_read(rtc_addresses[i]);
                  HAL_Delay(11);
       }
                  *alarm_values[i] = eeprom_read(alarm_addresses[i]);
                  HAL_Delay(11);
   }
     
     // hard code current date (Feb 6, 2024 @1310h)
     week_day = 4, day = 6, month = 2, year = 25;
   hour = 12, min = 10, sec = 0;
     
     alarm_weekday = 4, alarm_day = 6, alarm_month = 2, alarm_year = 25;
   alarm_hour = 12, alarm_min = 10, alarm_sec = 5;
     
     rtc_set_time(hour, min, sec);
     rtc_set_date(week_day, day, month, year);
     
     sprintf(msg, "RTC Restored: %02d/%02d/%02d %02d:%02d:%02d\n", day, month, year, hour, min, sec);
   print_msg(msg);
 
   sprintf(msg, "Alarm Restored: %02d/%02d/%02d %02d:%02d:%02d\n\n", alarm_day, alarm_month, alarm_year, alarm_hour, alarm_min, alarm_sec);
   print_msg(msg);
 
   while (1)
   {
         /* 1.3 Setting Date & Time */
         rtc_get_time(&hour, &min, &sec);
         rtc_get_date(&week_day, &day, &month, &year);
         
         
         if (eeprom_flag == 1)  // ? EEPROM write only when needed
     {
         sprintf(msg, "****************************\ninside eeproom loop in main\n");
                 print_msg(msg);
 
         // ? Store RTC time in EEPROM
         for (int i = 0; i < 7; i++) {
                         sprintf(msg, "storing rtc\n");
                         print_msg(msg);
             eeprom_write(rtc_addresses[i], (uint8_t*)rtc_values[i], 1);
         }
 
         sprintf(msg, "Date & time saved to EEPROM\n");
         print_msg(msg);
 
         // ? Store Alarm time in EEPROM
         for (int i = 0; i < 7; i++) {
                         sprintf(msg, "storing alarm\n");
                         print_msg(msg);
             eeprom_write(alarm_addresses[i], (uint8_t*)alarm_values[i], 1);
         }
 
         sprintf(msg, "Alarm stored to EEPROM\n****************************\n\n");
         print_msg(msg);
 
         eeprom_flag = 0;  // Reset flag after writing
     }
         
         
         // Check if the RTC time matches the alarm time
     if (hour == alarm_hour && min == alarm_min && sec == alarm_sec 
              && day == alarm_day && month == alarm_month && year == alarm_year )
         {
                 print_date_and_time(hour, min, sec, week_day, day, month, year);
                 print_alarm(alarm_hour, alarm_min, alarm_sec, alarm_weekday, alarm_day, alarm_month, alarm_year);
             
                 sprintf(msg, "ALARM!\n");
                 print_msg(msg);
             
             for (int i = 0; i < 10; i++)  // Loop for 10 iterations (10 seconds)
             {
                     HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
                     HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
                     HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
                     
                     HAL_Delay(1000);  // 1-second delay before toggling again
                     rtc_get_time(&hour, &min, &sec);
             }	
             
             sprintf(msg, "ALARM OFF\n\n");
             print_msg(msg);
     }
     
         
         // save current date and time in eeprom 
         eeprom_write(0x0, rtc_values[0], 7);
         
     print_date_and_time(hour, min, sec, week_day, day, month, year);
         print_alarm(alarm_hour, alarm_min, alarm_sec, alarm_weekday, alarm_day, alarm_month, alarm_year);
 
     HAL_Delay(1000); // wait 1 second before reading again
 
   }
 }
 
 void test_binary_conversions(uint8_t value) {
     char msg[100];
 
     uint8_t bcd_val = bin2bcd(value);
     uint8_t bid_val = bcd2bin(bcd_val);
 
     sprintf(msg, "Original Value = %d\n", value);
     print_msg(msg);
     
      sprintf(msg, "BID Value: ");
    print_msg(msg);
    for (int i = 7; i >= 0; i--) {
         sprintf(msg, "%d", (bid_val >> i) & 1);
         print_msg(msg);
     }
     print_msg("\n");
 
     sprintf(msg, "BCD Value: ");
     print_msg(msg);
     for (int i = 7; i >= 0; i--) {
         sprintf(msg, "%d", (bcd_val >> i) & 1);
         print_msg(msg);
     }
     print_msg("\n");
 }
 
 void print_date_and_time(uint8_t hour, uint8_t min, uint8_t sec,
                          uint8_t week_day, uint8_t day, uint8_t month, uint8_t year) {
     char msg[100];
 
     // Format and print the time
     sprintf(msg, "Date:  %02d/%02d/%02d %02d:%02d:%02d\n", day, month, year, hour, min, sec);
     print_msg(msg);
 }
                                                  
 void print_alarm(uint8_t alarm_hour, uint8_t alarm_min, 
                                     uint8_t alarm_sec, uint8_t alarm_weekday, 
                                     uint8_t alarm_day, uint8_t alarm_month,  uint8_t alarm_year) 
 {
     char msg[100];
 
     // Format and print the date
     sprintf(msg, "Alarm: %02d/%02d/%02d %02d:%02d:%02d\n\n", alarm_day, alarm_month, alarm_year, alarm_hour, alarm_min, alarm_sec);
     print_msg(msg);
 }
 