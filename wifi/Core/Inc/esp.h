#ifndef esp_header
#define esp_header
#include "stdlib.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "usart.h"


volatile uint8_t esp_recv_char;
volatile uint8_t esp_char_counter;
char esp_pattern[6];
volatile uint8_t esp_recv_flag;
volatile char esp_recv_mux;
volatile char esp_recv_buffer[1024];
volatile uint16_t esp_recv_len;
void esp_write_line(UART_HandleTypeDef * handler, char * text);
void esp_read_line(UART_HandleTypeDef * handler, char * buffer, uint16_t buffer_size);
char esp_read_char(UART_HandleTypeDef * handler);
uint8_t esp_send_cmd(UART_HandleTypeDef * uart, char * command);
void esp_send_data_and_close(UART_HandleTypeDef * uart, char mux_id, char * content);
void esp_start_int_recv(UART_HandleTypeDef * uart);
void esp_restart_int_recv(UART_HandleTypeDef * uart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * uart);
uint8_t esp_setup(UART_HandleTypeDef * uart, char * wifi_name, char * wifi_pass);

// Moja funkcja getIP
char *esp_getIP(UART_HandleTypeDef * uart, char[]);
//


#endif
