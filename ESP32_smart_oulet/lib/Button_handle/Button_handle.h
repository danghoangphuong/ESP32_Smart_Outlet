/*
Button handle library for ESP32
Author: Phuong Dang
06/2020
*/
#ifndef BUTTON_HANDEL_H
#define BUTTON_HANDLE_H

#include "Arduino.h"

typedef struct
{
	uint8_t count;
	uint8_t btn_current;
	uint8_t btn_last;
	uint8_t btn_filter;
	uint8_t is_debouncing;
	uint32_t time_debounce;
	uint32_t time_start_press;
	uint8_t  is_press_timeout;
	uint8_t	button_id; 
	uint8_t mode;
	uint8_t GPIO_pin;
} Button_TypeDef;

void Button_handle(Button_TypeDef *Buttonx);
void Button_init(Button_TypeDef *Buttonx, uint8_t pin_mode, uint8_t gpio_pin);
uint32_t Esp_gettick(void);

#endif

