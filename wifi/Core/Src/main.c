/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include <string.h>
#include <stdio.h>
#include "math.h"
#include "esp.h"
#include "web.h"
#include "wire.h"
#include "ds18b20.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


// tutaj

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef char log_line[41];

float temperature1;
float temperature2;
float result;
char temp_char_air[5];
char temp_char_water[5];
char esp_IP[1000]; //30
char IP_Request[20] = {"AT+CIPSTA?"};//AT+CIPSTA?
int ph = 'k'+1;
int interrupt_flag=0;
uint8_t flag=0;
uint8_t second_prev=0;
//int log_num=0;
//log_line log_tab[10]; // tablica logów
//char logg[40]="<p><b> --:--:-- , --:--:--</b> </p>\r\n";


uint8_t ds1[]={0x28,0x5e,0xe3,0xfb,0x0c,0x00,0x00,0xe9}; // adres termometru bez obudowy
uint8_t ds2[]={0x28,0xd5,0x7f,0xcc,0x2b,0x20,0x01,0xd2}; // adres termometru w obudowie
uint8_t moj_scratchpad[DS18B20_SCRATCHPAD_SIZE]; // testowy

RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Log_Time(void); // pobranie aktualnego czasu i wpisanie aktualnego loga

// tu powinna być tylko deklaracja funkcji, a poniżej maina jej ciało, no ale dobra
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // funkcja z redefinicji
{

	  Log_Time();
	//interrupt_flag=1;
	//interrupt++;

  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// Funkcja obliczająca korekcję gamma i ustawiająca jasność diody
void set_led_brightness(TIM_HandleTypeDef * timer,
		uint32_t channel, uint8_t brightness)
{
	int32_t value = powf((double) brightness / 255.0, 2.2) * 49999;
	__HAL_TIM_SET_COMPARE(timer, channel, value);
}
// Funkcja ustawiająca kolor świecenia diody RGB
void set_color(uint8_t, uint8_t, uint8_t);


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	int web_state = 0;

	//uint32_t sec;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */


  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);


  if(ds18b20_init() != HAL_OK) Error_Handler();
  //if(ds18b20_read_address(ds1)!= HAL_OK)Error_Handler(); // do odczytywania adresów czujników

  web_def(); // przepisanie stron internetowych

//  if (esp_setup(&huart1, "BUS OBSERWACYJNY 1504", "HUNDERTWASSERHAUS"))
//	  set_color(100, 100, 0);
//  else
//	  set_color(30, 30, 0);           //////// Docelowy router

  if (esp_setup(&huart1, "BUS OBSERWACYJNY 1504", "HUNDERTWASSERHAUS"))
	  set_color(100, 100, 0);
  else
	  set_color(30, 30, 0);

  ///HAL_Delay(500);
  //esp_send_data_and_close(&huart1, esp_recv_mux,IP_Request); //IP_Request
  //esp_getIP(&huart1,esp_IP);
  	///  esp_send_cmd(&huart1, &IP_Request);
	//esp_send_data_and_close(&huart1, esp_recv_mux, webpage); // dodałem, żeby wysyłało stronę zanim wejdzie do while
	//esp_restart_int_recv(&huart1);


  //  //set time
    if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1) != 0x32F2) // próba zrobienia, żeby się nie resetowało
    {
    sTime.Hours = 13;
    sTime.Minutes = 23;
    sTime.Seconds = 30;
    HAL_RTC_SetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
    }

    sDate.Date = 18;
    sDate.Month = RTC_MONTH_MARCH;
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Year = 22;
    HAL_RTC_SetDate(&hrtc,&sDate, RTC_FORMAT_BIN);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  // Obsługujemy dane przychodzące przez połączenie TCP,
	  // jeśli w przerwaniu ustawiono flagę esp_recv_flag

//	  HAL_Delay(15000);
//	  esp_send_data_and_close(&huart1, esp_recv_mux, frontpage);
//	  HAL_Delay(15000);



	  if (esp_recv_flag == 1)
	  {
		select_web(&huart1, &web_state);
	  }


	  ds18b20_start_measure(ds1);
	  temperature1 = ds18b20_get_temp(ds1);
	  sprintf(temp_char_air,"%0.1f",temperature1);

	  ds18b20_start_measure(ds2);
	  temperature2 = ds18b20_get_temp(ds2);
	  sprintf(temp_char_water,"%0.1f",temperature2);

	  //strcat(&heizung[216], temp_char);
	  heizung[232] = temp_char_air[0];
	  heizung[233] = temp_char_air[1];
	  heizung[234] = temp_char_air[2];
	  heizung[235] = temp_char_air[3];

	  heizung[285] = temp_char_water[0];
	  heizung[286] = temp_char_water[1];
	  heizung[287] = temp_char_water[2];
	  heizung[288] = temp_char_water[3];


	  // sprawdź z ciekawości co jest w heizung[220...]
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Log_Time(void){

	int i=644;
	char seconds[3]={0,0,0};
	char minutes[3]={0,0,0};
	char hours[3]={0,0,0};
	char date[3]={0,0,0};
	char month[3]={0,0,0};
	char year[3]={0,0,0};

	  HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);

	  if(second_prev != sTime.Seconds){

		  itoa(sTime.Seconds,seconds,10);
		  itoa(sTime.Minutes,minutes,10);
		  itoa(sTime.Hours,hours,10);
		  itoa(sDate.Date,date,10);
		  itoa(sDate.Month,month,10);
		  itoa(sDate.Year,year,10);

		  if (seconds[1]=='\0'){
			  seconds[1]='0';
			 }
		  if(minutes[1]=='\0'){
			  minutes[1]='0';
			 }
		  if(hours[1]=='\0'){
			  hours[1]='0';
			 }
		  if(date[1]=='\0'){
			  date[1]='0';
			 }
		  if(month[1]=='\0'){
			  month[1]='0';
			 }
		  if(year[1]=='\0'){
			  year[1]='0';
			 }

	////////////// hours:minutes:seconds //////////////

		  if(sTime.Seconds < 10){
		  presence[298]=seconds[1];
		  presence[299]=seconds[0];
		  }
		  else{
			  presence[298]=seconds[0];
			  presence[299]=seconds[1];
		  }

		  if (sTime.Minutes < 10){
		  presence[295]=minutes[1];
		  presence[296]=minutes[0];
		  }
		  else{
			  presence[295]=minutes[0];
			  presence[296]=minutes[1];
		  }

		  if (sTime.Hours < 10){
		  presence[292]=hours[1];
		  presence[293]=hours[0];
		  }
		  else{
			  presence[292]=hours[0];
			  presence[293]=hours[1];
		  }

	///////// year:month:day ////////////////////////

		  if(sDate.Date < 10){
		  presence[287]=date[1];
		  presence[288]=date[0];
		  }
		  else{
			  presence[287]=date[0];
			  presence[288]=date[1];
		  }

		  if (sDate.Month < 10){
		  presence[284]=month[1];
		  presence[285]=month[0];
		  }
		  else{
			  presence[284]=month[0];
			  presence[285]=month[1];
		  }

		  if (sDate.Year < 10){
		  presence[281]=year[1];
		  presence[282]=year[0];
		  }
		  else{
			  presence[281]=year[0];
			  presence[282]=year[1];
		  }

	////// shift of logs //////////////////////// 311
		  while(i>311){
			  presence[i]=presence[i-37];
			  i--;
		  }
		  strcat((char*)presence[281],(char*)("YY:MM:DD , HH:MM:SS"));
		  presence[281]='Y';//(char)("YY:MM:DD , HH:MM:SS"); //{"YY:MM:DD , HH:MM:SS"} // (char)"Y"; //;

		  second_prev = sTime.Seconds;


	  }
	  //logi[65] = sTime.Seconds;

	  //strcat(presence, "<p><b>Obecnosc w pomieszczeniu:  </b> </p>\r\n");
	  //presence_page(UART_HandleTypeDef * uart ,*web_state);
	  //esp_send_data_and_close(&huart1, esp_recv_mux, presence);

	  //interrupt_flag=0;

}

void set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	set_led_brightness(&htim1, TIM_CHANNEL_2, red);
	set_led_brightness(&htim1, TIM_CHANNEL_3, green);
	set_led_brightness(&htim1, TIM_CHANNEL_1, blue);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
