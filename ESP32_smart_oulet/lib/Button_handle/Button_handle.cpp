/*
Button handle library for ESP32
Author: Phuong Dang
06/2020
*/
#include "Button_handle.h"
__attribute__((weak)) void Button_press_callback(Button_TypeDef *Buttonx)
{
    // Callback function
}

__attribute__((weak)) void Button_short_press_callback(Button_TypeDef *Buttonx)
{
    // Callback function
}

__attribute__((weak)) void Button_long_press_callback(Button_TypeDef *Buttonx)
{
    // Callback function
}

__attribute__((weak)) void Button_release_callback(Button_TypeDef *Buttonx)
{
    // Callback function
}

void Button_handle(Button_TypeDef *Buttonx)
{
    uint8_t status = digitalRead(Buttonx->GPIO_pin); 
    if(status != Buttonx->btn_filter)
    {
        Buttonx->btn_filter = status; // init_stt = 0
		Buttonx->is_debouncing = 1;
		Buttonx->time_debounce = Esp_gettick(); // get time
    }

    if(Buttonx->is_debouncing && (Esp_gettick() - Buttonx->time_debounce >= 15)) //if stt change & current time - last time >=15
	{
		Buttonx->btn_current = Buttonx->btn_filter;
		Buttonx->is_debouncing = 0; 
	}
    //==============================push/ release handle====================
	if(Buttonx->btn_current != Buttonx->btn_last)
	{
		  if(Buttonx->btn_current == 0)
		  {
				Buttonx->is_press_timeout = 1;
			    Button_press_callback(Buttonx); // nhấn phát nhận luôn
				Buttonx->time_start_press = Esp_gettick();
		  }
		  else
		  {
			if(Esp_gettick() - Buttonx->time_start_press <= 1000)
			{
				Button_short_press_callback(Buttonx); // nhấn nhả
			}	
			
			Button_release_callback(Buttonx); // nhả
			Buttonx->is_press_timeout = 0;
		  }
		  Buttonx->btn_last = Buttonx->btn_current;
	}
    //===========================long press handle===========================
    if(Buttonx->is_press_timeout && (Esp_gettick() - Buttonx->time_start_press >= 3000))
	{
		Button_long_press_callback(Buttonx);
		Buttonx->is_press_timeout = 0;
	}

}

uint32_t Esp_gettick(void) //Equivalent to HAL_Gettick() in STM32 API
{
  return static_cast<uint32_t>(esp_timer_get_time()/1000);
}

void Button_init(Button_TypeDef *Buttonx, uint8_t pin_mode, uint8_t gpio_pin)
{
    Buttonx->btn_current = 1;
    Buttonx->btn_last = 1;
    Buttonx->btn_filter = 1;
    Buttonx->mode = pin_mode;
    Buttonx->GPIO_pin = gpio_pin;
	pinMode(Buttonx->GPIO_pin, Buttonx->mode);
}