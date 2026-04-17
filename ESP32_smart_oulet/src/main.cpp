#include <Arduino.h>
#include <Button_handle.h>
#include "wifi_config.h"

Button_TypeDef button_rst;
Wifi_config wifi_conf(80); // initialize
Wifi_config_TypeDef wifi_info;

typedef enum
{
  WIFI_DISCONNECTED,
  WIFI_CONNECTED,
  WIFI_RESETTING
}WIFI_STATUS;
WIFI_STATUS wifi_sta;

void setup() 
{
  Serial.begin(115200);
  Button_init(&button_rst, INPUT_PULLUP, 34);// nút rst wifi
  pinMode(2, OUTPUT); // báo trạng thái wifi
  wifi_info.ssid = "Phuong Smart Outlet";
  wifi_info.password = "12345678";
  wifi_conf.Init_config(&wifi_info);
}

void Button_long_press_callback(Button_TypeDef *Buttonx)
{
  if(Buttonx == &button_rst)
  {
    wifi_sta = WIFI_RESETTING;
  }
}

void Wifi_status_indicator() // Kết nối thì đèn sáng, ko kết nối thì tắt, reset thì nhấp nháy 
{
  if(wifi_sta != WIFI_RESETTING)
  {
    wifi_sta = wifi_conf.Is_connected() ? WIFI_CONNECTED : WIFI_DISCONNECTED;
  }
  static uint32_t time = 0;
  switch(wifi_sta)
  {
    case WIFI_DISCONNECTED:
      digitalWrite(2, LOW);
      break;
    case WIFI_CONNECTED:
      digitalWrite(2, HIGH);
      break;
    case WIFI_RESETTING:
      if(millis() - time >= 200)
      {
        digitalWrite(2, !digitalRead(2)); // nhấp nháy nhanh là reset
        time = millis();
        static uint8_t count = 0;
        count++;
        if(count >= 30)
        {
          count = 0;
          wifi_sta = WIFI_DISCONNECTED;
          wifi_conf.Reset_wifi_config();
        }
      }
      break;
    default: break;
  }
}

void loop() 
{
  wifi_conf.Handle_Client();
  Button_handle(&button_rst);
  Wifi_status_indicator();
}
