/*
 * web.c
 *
 *  Created on: 23 gru 2021
 *      Author: Kwaśny
 */

#include "web.h"
#include "esp.h"
#include <usart.h>
#include <gpio.h>
#include <tim.h>



void select_web (UART_HandleTypeDef *huart, int *web_state)
{
	// Odczytujemy pierwsze 6 znaków odebranego żądania HTTP
	char request_begining[7];

	for (uint8_t i = 0; i < 6; i++)
		request_begining[i] = esp_recv_buffer[i];
	request_begining[6] = '\0';
	// Jeśli przesłane zostały parametry - początek żądania:
	// "GET /?red=XXX&green=XXX&blue=XXX HTTP/1.1"
	if (strcmp(request_begining, "GET /?") == 0)
	{
		set_color(0,0,80);
		//HAL_Delay(500);

		switch (*web_state)
		{
		case NOT_LOGGED:
			login(&huart1,web_state);
			break;
		case LOGGED:
			handle_request_strona(&huart1,web_state);
			break;
		case LIGHTS:
			lights_page(&huart1,web_state);
			break;
		case HEIZUNG:
			heizung_seite(&huart1,web_state);
		break;
		case PRESENCE:
			presence_page(&huart1,web_state);
		break;
		default:
			break;
		}

////// Sprawdzenie loginu
	}else if (strcmp(request_begining, "GET / ") == 0)
	{
		set_color(200,0,0);
		esp_send_data_and_close(huart, esp_recv_mux, frontpage);
	}
	else
		esp_send_data_and_close(huart, esp_recv_mux, error);
///////////////////////////////////////////////////////////////////////////////
	esp_restart_int_recv(huart);

	//return web_state;

}

void login(UART_HandleTypeDef * uart, int *web_state)
{
	int l=0; //litera logowania


	for(int i=12 ; i<=17 ; i++)
	{
		check_login[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;

	//////Sprawdzenie hasła
	for(int i=25 ; i<=30 ; i++)
	{
		check_password[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;

///////////////////////Sprawdzenie poprawności logowania
    if((strcmp(check_login,"kwas97")==0)  &&  (strcmp(check_password,"kwas97")==0))
	{
		set_color(250,250,250);
		*web_state=LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, webpage);

	}
    else //((strcmp(check_login,"kwas97")!=0)||(strcmp(check_password,"kwas97")!=0))
	{
		set_color(30,30,30);
		*web_state=NOT_LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, frontpage);
		//select_web(uart,web_state);
	}

    	esp_restart_int_recv(uart);
    		//return web_state;
}




// Funkcja wywoływana w momencie otrzymania danych przez połączenie TCP
void handle_request_strona(UART_HandleTypeDef * uart, int *web_state)
{
	// Odczytujemy pierwsze 6 znaków odebranego żądania HTTP

	// Odczytujemy pierwsze 3 liczby, jakie pojawią się w adresie
	// - są to wartości poszczególnych kolorów składowych (RGB)
	int nums[3] = { 0, 0, 0 };
	int num_counter = 0;
	int l=0;
	uint8_t last_char_was_digit = 0;
	for (int i = 6; i < 41; i++)
	if (esp_recv_buffer[i] >= '0' && esp_recv_buffer[i] <= '9') {
	last_char_was_digit = 1;
	nums[num_counter] *= 10;
	nums[num_counter] += esp_recv_buffer[i] - '0';
	} else if (last_char_was_digit == 1) {
	last_char_was_digit = 0;
	num_counter++;
	if (num_counter == 4)
	break;
	}

	for (int i=6 ; i<=7; i++)
	{
		button_check[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;

	if(strcmp(button_check,"wl")==0){
		*web_state=NOT_LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, frontpage);
	}
	else if(strcmp(button_check,"os")==0){
		*web_state=LIGHTS;
		//select_web(uart,web_state);
		esp_send_data_and_close(uart, esp_recv_mux, lights);
	}
	else if(strcmp(button_check,"og")==0){
		*web_state=HEIZUNG;
		//select_web(uart,web_state);
		esp_send_data_and_close(uart, esp_recv_mux, heizung );
	}

	else if(strcmp(button_check,"ob")==0){
		*web_state=PRESENCE;
		//select_web(uart,web_state);
		esp_send_data_and_close(uart, esp_recv_mux, presence);
	}



	else if(strcmp(button_check,"pw")==0){
		*web_state=LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, webpage);
	}

	else esp_send_data_and_close(uart, esp_recv_mux, webpage);
	//else esp_send_data_and_close(uart, esp_recv_mux, webpage);

	// Ustawiamy kolor na diodzie RGB
	//set_color(nums[0], nums[1], nums[2]);
	// Zwracamy stronę WWW z formularzem wyboru kolorów
	//esp_send_data_and_close(uart, esp_recv_mux, webpage);
	// Jeśli żądanie dotyczy strony głównej - "GET / HTTP/1.1"

	esp_restart_int_recv(uart);
}

void lights_page(UART_HandleTypeDef * uart, int *web_state)
{

	int l=0;

	for (int i=6 ; i<=7; i++)
	{
		button_check[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;

	if(strcmp(button_check,"pw")==0)
	{
		*web_state=LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, webpage);
		//select_web(&huart1,web_state);

	}
	else
	{
	esp_send_data_and_close(uart, esp_recv_mux, lights);
	//esp_restart_int_recv(uart);
	}

}



void heizung_seite(UART_HandleTypeDef * uart ,int *web_state) //uint8_t
{

	int l=0;

	for (int i=21 ; i<=22; i++)
	{
		button_check[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;


	if(strcmp(button_check,"pw")==0) // zmień na pw
	{
		*web_state=LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, webpage);
	}

	else
	{
	esp_send_data_and_close(uart, esp_recv_mux, heizung);
	}

}

void presence_page(UART_HandleTypeDef * uart ,int *web_state) //uint8_t
{

	int l=0;

	for (int i=6 ; i<=7; i++)
	{
		button_check[l]=esp_recv_buffer[i];
		l++;
	}
	l=0;


	if(strcmp(button_check,"pw")==0) // zmień na pw
	{
		*web_state=LOGGED;
		esp_send_data_and_close(uart, esp_recv_mux, webpage);
	}

	else
	{
	esp_send_data_and_close(uart, esp_recv_mux, presence);
	}

}



void web_def (void)
{
//////////////OBECNOŚĆ/////////////////////////////////////////////
	  strcpy(presence, "HTTP/1.1 200 OK\r\n");
	  strcat(presence, "Content-Type: text/html\r\n");
	  strcat(presence, "Content-Lenght: 398\r\n");
	  strcat(presence, "Connection: close\r\n\r\n");
	  strcat(presence, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
	  strcat(presence, "<title>Dioda RGB</title>\r\n</head>\r\n");
	  strcat(presence, "<body>\r\n<form method=\"get\">\r\n");
	  // stąd wyciągnąłem napis obecnosc
	  //strcat(presence, "<p><b>Temperatura wody w zasobniku:    °C</b> </p></p>\r\n"); //°C
	  //strcat(presence, "<p><b>Zadana temperatura (10-33):</b><input type=\"text\"name=\"tm\" value=\"22\" /></p>\r\n");
	  //strcat(presence, "<p><b>  <input type=\"range\" id=\"volume\" name=\"volume\"min=\"0\" max=\"9\"><label for=\"volume\">Volume</label>\r\n");
	  strcat(presence, "<p><b>Obecnosc w pomieszczeniu  </b> </p>\r\n"); //°C
	  strcat(presence, "<p><b>Data:  Godzina:  </b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:50</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:40</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:30</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:20</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:10</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:00</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:50</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:40</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:30</b> </p>\r\n");
	  strcat(presence, "<p><b> --:--:-- , --:--:20</b> </p>\r\n");
	  strcat(presence, "<p><b><b><input type=\"submit\" name=\"pw\" value=\"POWROT\"/>\r\n");

	  // tu poniżej będą dopisywane logi



///////////////////////////////////////////////////////////////


///////////// OGRZEWANIE ///////////////////////////////////////////////

  strcpy(heizung, "HTTP/1.1 200 OK\r\n");
  strcat(heizung, "Content-Type: text/html\r\n");
  strcat(heizung, "Content-Lenght: 398\r\n");
  strcat(heizung, "Connection: close\r\n\r\n");
  strcat(heizung, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
  strcat(heizung, "<title>Dioda RGB</title>\r\n</head>\r\n");
  strcat(heizung, "<body>\r\n<form method=\"get\">\r\n");
  strcat(heizung, "<p><b>Temperatura w pomieszczeniu:    °C</b> </p>\r\n"); //°C
  strcat(heizung, "<p><b>Temperatura wody w zasobniku:    °C</b> </p></p>\r\n"); //°C
  strcat(heizung, "<p><b>Zadana temperatura (10-33):</b><input type=\"text\"name=\"tm\" value=\"22\" /></p>\r\n");
  strcat(heizung, "<p><b>  <input type=\"range\" id=\"volume\" name=\"volume\"min=\"0\" max=\"9\"><label for=\"volume\">Volume</label>\r\n");
  strcat(heizung, "<p><b><b><input type=\"submit\" name=\"pw\" value=\"POWROT\"/>\r\n");

/////////////////////// OSWIETLENIE//////////////////////////////////////

  strcpy(lights, "HTTP/1.1 200 OK\r\n");
  strcat(lights, "Content-Type: text/html\r\n");
  strcat(lights, "Content-Lenght: 398\r\n");
  strcat(lights, "Connection: close\r\n\r\n");
  strcat(lights, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
  strcat(lights, "<title>Dioda RGB</title>\r\n</head>\r\n");
  strcat(lights, "<body>\r\n<form method=\"get\">\r\n");

  strcat(lights, "<p><b><select><option value=\"auto\">automatyczne</option><option value=\"man\">Reczne</option></select>\r\n");
  strcat(lights, "<p><b><b><input type=\"submit\" name=\"pw\" value=\"POWROT\"/>\r\n");
  //strcat(lights, "<form id=\"formName\" action=\"<?php echo $_SERVER['PHP_SELF'];?>\" method=\"get\"><input type =\"checkbox\" name=\"cBox[]\" value = \"3\">3</input><input type =\"checkbox\" name=\"cBox[]\" value = \"4\">4</input><input type =\"checkbox\" name=\"cBox[]\" value = \"5\">5</input><input type=\"submit\" name=\"submit\" value=\"Search\" /></form>");
  //strcat(lights, "<p><b><select><option value=\"auto\">automatyczne</option><option value=\"man\">Reczne</option></select\>\r\n");
  //strcat(lights, "<form action=\"/Home/CategoryChosen\" method=\"get\"><fieldset>Movie Type <select id=\"MovieType\" name=\"MovieType\"><option value=\"\"></option><option value=\"0\">Action</option><option value=\"1\">Drama</option><option selected=\"selected\" value=\"2\">Comedy</option><option value=\"3\">Science Fiction</option></select><p><input type=\"submit\" value=\"Submit\" /> </p></fieldset></form>");
  //  strcat(webpage, "<p><b>Niebieski (0-255):</b> <input type=\"text\"name=\"blue\" value=\"0\" /></p>\r\n");

  ////////////////////////////////////////////////////////////////////////////


////////////////////// STRONA GŁÓWNA ////////////////////////////////////

  strcpy(webpage, "HTTP/1.1 200 OK\r\n");
  strcat(webpage, "Content-Type: text/html\r\n");
  strcat(webpage, "Content-Lenght: 398\r\n");
  strcat(webpage, "Connection: close\r\n\r\n");
  strcat(webpage, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
  strcat(webpage, "<title>Dioda RGB</title>\r\n</head>\r\n");
  strcat(webpage, "<body>\r\n<form method=\"get\">\r\n");




//  strcpy(webpage, "HTTP/1.1 200 OK\r\n");
//  strcat(webpage, "Content-Type: text/html\r\n");
//  strcat(webpage, "Content-Lenght: 398\r\n");
//  strcat(webpage, "Connection: close\r\n\r\n");
//  strcat(webpage, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
//  strcat(webpage, "<title>Dioda RGB</title>\r\n</head>\r\n");
//  strcat(webpage, "<body>\r\n<form method=\"get\">\r\n");


//  strcat(webpage, "<p><b>Czerwony (0-255):</b><input type=\"text\"");
//  strcat(webpage, " name=\"red\" value=\"0\" /></p>\r\n");
//  strcat(webpage, "<p><b>Zielony (0-255):</b> <input type=\"text\"");
//  strcat(webpage, " name=\"green\" value=\"0\" /></p>\r\n");
//  strcat(webpage, "<p><b>Niebieski (0-255):</b> <input type=\"text\"");
//  strcat(webpage, " name=\"blue\" value=\"0\" /></p>\r\n");
//strcat(webpage, "<input type=\"submit\" value=\"Ustaw kolor\" />\r\n");
//strcat(webpage, "</form>\r\n</body>\r\n</html>");
  strcat(webpage, "<p><b><input type=\"submit\" name=\"os\" value=\"OSWIETLENIE\"/>\r\n");
  strcat(webpage, "<p><b><input type=\"submit\" name=\"og\" value=\"OGRZEWANIE\"/>\r\n");
  strcat(webpage, "<p><b><input type=\"submit\" name=\"ob\" value=\"KONTROLA OBECNOSCI\"/>\r\n");
  strcat(webpage, "<p><b><b><input type=\"submit\" name=\"wl\" value=\"WYLOGUJ\"/>\r\n");
 // strcat(webpage, "<p><b><select><option value=\"auto\">automatyczne</option><option value=\"man\">Reczne</option></select\>\r\n");

//strcat(webpage, "<button onclick=\"getElementById('demo').innerHTML = Date()\">What is the time?</button>");   //niby czas
// strcat(webpage, "<p><input type=\"radio\" value=\"temperatura\" />\r\n"); // kropki wyboru
  strcat(webpage, "</form>\r\n</body>\r\n</html>");



 /////////////////////// //strona logowania

  strcpy(frontpage, "HTTP/1.2 200 OK\r\n");
  strcat(frontpage, "Content-Type: text/html\r\n");
  strcat(frontpage, "Content-Lenght: 398\r\n");
  strcat(frontpage, "Connection: close\r\n\r\n");
  strcat(frontpage, "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<html lang=\"pl\">");
  strcat(frontpage, "<title>Dioda RGB</title>\r\n</head>\r\n");
  strcat(frontpage, "<body>\r\n<form method=\"get\">\r\n");
  //strcat(frontpage,"<form><label for=\"username\">Username:</label><br><input type=\"text\" id=\"username\" name=\"username\"><br><label for=\"pwd\">Password:</label><br><input type=\"password\" id=\"pwd\" name=\"pwd\"><br><br><input type=\"submit\" value=\"loguj\"> </form>");
  strcat(frontpage, "<p><b>Login: </b><input type=\"text\"");
  strcat(frontpage, " name=\"login\" value=\"\" /></p>\r\n");
  strcat(frontpage, "<p><b>Haslo:</b> <input type=\"password\"");
  strcat(frontpage, " name=\"haslo\" value=\"\" /></p>\r\n");
  strcat(frontpage, "<input type=\"submit\" value=\"LOGUJ\" />\r\n");
  strcat(frontpage, "</form>\r\n</body>\r\n</html>");

////// error

  strcpy(error, "HTTP/1.1 404 Not Found\r\n");
  strcat(error, "Content-Type: text/html\r\n");
  strcat(error, "Content-Lenght: 48\r\n");
  strcat(error, "Connection: close\r\n\r\n");
  strcat(error, "<html><body><h1>404 Not Foundxdd</h1></body></html>");
}

