#include "esp.h"

char esp_IP2 [1000]; //[30]
// Funkcja wysyłająca podany ciąg znaków przez interfejs UART
void esp_write_line(UART_HandleTypeDef * handler, char * text) {
HAL_UART_Transmit(handler, text, strlen(text), 1000);
HAL_UART_Transmit(handler, "\r\n", 2, 100);
}
// Funkcja odbierająca linię tekstu przez interfejs UART
void esp_read_line(UART_HandleTypeDef * handler, char * buffer, uint16_t buffer_size) {
HAL_StatusTypeDef status;
char current_char;
uint16_t char_counter = 0;
while (char_counter < buffer_size - 1) {
status = HAL_UART_Receive(handler, &current_char, 1, 1);
if (status == HAL_OK) {
if (current_char == '\r' || current_char == '\n')
if (char_counter == 0) continue;
else break;
*(buffer + char_counter++) = current_char;
}
}
*(buffer + char_counter) = '\0';
}
// Funkcja odczytująca pojedynczy znak odebrany przez UART
char esp_read_char(UART_HandleTypeDef * handler) {
char buffer = '\0';
HAL_UART_Receive(handler, &buffer, 1, 1000);
return buffer;
}
// Funkcja wysyłająca polecenie do modułu ESP8266
// i oczekująca na jego potwierdzenie
uint8_t esp_send_cmd(UART_HandleTypeDef * uart, char * command) {
char response[1000]; //[30]
response[0] = '\0';
int i = 1000;//dołożone
esp_write_line(uart, command);
__HAL_UART_FLUSH_DRREGISTER(&huart1);

//while(i <= 1000){  // cała pętla dołożona
//esp_read_line(uart, response, 1000);// kopiowane z niżej, podwojone 30)
//i++;
//}
//strcat(esp_IP2,response);
//return 1;

while (strcmp(response, "OK") != 0 && strcmp(response, "no change") != 0 && strcmp(response, "ERROR") != 0)
esp_read_line(uart, response, 30);
if (strcmp(response, "ERROR") == 0) return 0;
else return 1;
}
// Moja funkcja Get ip
char *esp_getIP(UART_HandleTypeDef * uart, char ip[]) {  // sprawdzić dla i bez '*'
	char response[40];
	response[0] = '\0';
	if (!esp_send_cmd(uart, "AT+CIPSTA?")) return 0;
	esp_restart_int_recv(&huart1);
	while (strcmp(response, "OK") != 0
		&& strcmp(response, "no change") != 0
		&& strcmp(response, "ERROR") != 0)
		esp_read_line(uart, response, 30);
	strcat(ip,response); // dołożone
	return ip;
}
//

// Funkcja wysyłająca dane przez nawiązane połączenie TCP
// i zamykająca to połączenie
void esp_send_data_and_close(UART_HandleTypeDef * uart, char mux_id, char * content) {
	char cmd[17];
	sprintf(cmd, "AT+CIPSEND=%c,%d", mux_id, strlen(content));
	esp_write_line(uart, cmd);
	HAL_Delay(20);
	HAL_UART_Transmit(uart, content, strlen(content),5000 ); /// 5000
	HAL_Delay(100);
	sprintf(cmd, "AT+CIPCLOSE=%c", esp_recv_mux);
	esp_write_line(uart, cmd);


}
// Funkcja uruchamiająca obsługę przerwań
void esp_start_int_recv(UART_HandleTypeDef * uart) {
	__HAL_UART_FLUSH_DRREGISTER(uart);
	HAL_UART_Receive_IT(uart, &esp_recv_char, 1);
}
// Funkcja wznawiająca obsługę przerwań
void esp_restart_int_recv(UART_HandleTypeDef * uart) {
	esp_recv_flag = 0;
	HAL_UART_Receive_IT(uart, &esp_recv_char, 1);
}

// Funkcja obsługująca przerwanie, wywoływana w momencie odebrania
// przez interfejs UART pojedynczego bajtu danych
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * uart) {
	if (esp_recv_char == esp_pattern[esp_char_counter]) {
		esp_char_counter++;
		if (esp_char_counter == 5) {
			// Jeśli odbierzemy ciąg znaków "+IPD,":
			// Odczytujemy numer połączenia do zmiennej esp_recv_mux
			esp_recv_mux = esp_read_char(uart);
			esp_read_char(uart);
			// Odczytujemy długość odebranych danych do esp_recv_len
			char length_str[5];
			char current_char = 0;
			uint8_t char_counter = 0;
			do {
				current_char = esp_read_char(uart);
				length_str[char_counter++] = current_char;
			} while (current_char != ':');

			length_str[char_counter] = '\0';
			uint16_t esp_recv_len = atoi(&length_str);
			// Odbieramy dane do bufora esp_recv_buffer
			HAL_UART_Receive(uart, esp_recv_buffer,
			esp_recv_len, 1000);
			esp_recv_flag = 1;

			return;
		}
	} else esp_char_counter = 0;
	// Ponowne uruchomienie przerwania
	HAL_UART_Receive_IT(uart, &esp_recv_char, 1);
}
// Funkcja przesyłająca do modułu ESP8266 polecenia konfigurujące
uint8_t esp_setup(UART_HandleTypeDef * uart, char * wifi_name, char * wifi_pass) {
esp_char_counter = 0;
strcpy(esp_pattern, "+IPD,");
esp_recv_flag = 0;
char conn_str[100];
sprintf(conn_str, "AT+CWJAP_CUR=\"%s\",\"%s\"", wifi_name, wifi_pass);
HAL_Delay(500); // Oczekujemy na uruchomienie modułu
if (!esp_send_cmd(uart, "AT+CWMODE=1")) return 0;
if (!esp_send_cmd(uart, conn_str)) return 0;
if (!esp_send_cmd(uart, "AT+CIPMUX=1")) return 0;
if (!esp_send_cmd(uart, "AT+CIPSERVER=1,80")) return 0;
//if (!esp_send_cmd(uart, "AT+CWJAP")) return 0; // dołożone
if (!esp_send_cmd(uart, "AT+CIPSTA?")) return 0; // dołożone
HAL_Delay(1500); // dołożone
esp_start_int_recv(uart);
return 1;
}
