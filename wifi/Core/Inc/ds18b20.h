/*
 * ds18b20.h
 *
 *  Created on: 15 gru 2021
 *      Author: Kwaśny
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#pragma once

#include "stm32f4xx.h"

#define DS18B20_ROM_CODE_SIZE		8
#define DS18B20_SCRATCHPAD_SIZE    9



////////// Definicje użytkownika


#define TIMER &htim2
uint8_t moj_scratchpad[DS18B20_SCRATCHPAD_SIZE];
int16_t temp2;

//////////////////////////////


// Inicjalizacja 1-wire
// return - HAL_OK/HAL_ERROR
HAL_StatusTypeDef ds18b20_init(void);

// Odczytaj adres czujnika i policz CRC
// rom_code - miejsce do zapisu odczytanych danych
// return - HAL_OK/HAL_ERROR
HAL_StatusTypeDef ds18b20_read_address(uint8_t* rom_code);

// Rozpocznij pomiar temperatury
// rom_code - adres czujnika lub NULL
// return - HAL_OK/HAL_ERROR
HAL_StatusTypeDef ds18b20_start_measure(const uint8_t* rom_code);

// Pobierz temperaturę
// rom_code - adres czujnika lub NULL
// return - temperatura w stopniach Celsjusza
float ds18b20_get_temp(const uint8_t* rom_code);

#endif /* INC_DS18B20_H_ */
