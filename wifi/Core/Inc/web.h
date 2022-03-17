/*
 * web.h
 *
 *  Created on: 23 gru 2021
 *      Author: Kwaśny
 */

#ifndef INC_WEB_H_
#define INC_WEB_H_

#pragma once

#include "stm32f4xx.h"



#define NOT_LOGGED 0
#define LOGGED 1
#define LIGHTS 2
#define HEIZUNG 3
#define PRESENCE 4


char webpage[500], error[500], frontpage[500], lights[1024],heizung[800],presence[8000]; // 483
volatile char check_login[30];
volatile char check_password[30];
char button_check[2];


//uint8_t *web_state = &webstate;
void web_def (void);
void select_web (UART_HandleTypeDef *huart, int *web_state);
void login(UART_HandleTypeDef * uart, int *web_state);
void handle_request_strona(UART_HandleTypeDef * uart, int *web_state);
void lights_page(UART_HandleTypeDef * uart, int *web_state);
void heizung_seite(UART_HandleTypeDef * uart , int *web_state); // uint8_t
void presence_page(UART_HandleTypeDef * uart , int *web_state);







#endif /* INC_WEB_H_ */
