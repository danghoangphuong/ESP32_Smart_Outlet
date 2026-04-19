#include <Arduino.h>
#include <Button_handle.h>
#include "wifi_config.h"
#include <BlynkSimpleEsp32.h>
#define BLYNK_TEMPLATE_ID "TMPL6XC10hGT2"
#define BLYNK_TEMPLATE_NAME "Home Outlet"
#define BLYNK_AUTH_TOKEN "oYUSrMOl7aWy_7B9HzfzvTCVhcO6piQo"

#define SLOT_1_PIN 32
#define SLOT_2_PIN 33
#define SLOT_3_PIN 34
#define RELAY_SLOT_1 V1
#define RELAY_SLOT_2 V2
#define RELAY_SLOT_3 V3

Button_TypeDef button_rst;
Wifi_config wifi_conf(80); // initialize
Wifi_config_TypeDef wifi_info;

uint8_t is_blynk_connected = 0;
unsigned long last_blynk_connect_attempt = 0;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(RELAY_SLOT_1,RELAY_SLOT_2,RELAY_SLOT_3);
}

typedef enum
{
  WIFI_DISCONNECTED,
  WIFI_CONNECTED,
  WIFI_RESETTING
}WIFI_STATUS;
WIFI_STATUS wifi_sta;

BLYNK_WRITE(RELAY_SLOT_1){
  int p = param.asInt();
  digitalWrite(SLOT_1_PIN, p);
}

BLYNK_WRITE(RELAY_SLOT_2){
  int p = param.asInt();
  digitalWrite(SLOT_2_PIN, p);
}

BLYNK_WRITE(RELAY_SLOT_3){
  int p = param.asInt();
  digitalWrite(SLOT_3_PIN, p);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(SLOT_1_PIN, OUTPUT);
  pinMode(SLOT_2_PIN, OUTPUT);
  pinMode(SLOT_3_PIN, OUTPUT);
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk-cloud.com", 80);

  Button_init(&button_rst, INPUT_PULLUP, 35);// nút rst wifi
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

  if (wifi_conf.Is_connected()) 
  {
    if (!Blynk.connected()) 
    {
      // Cứ mỗi 10 giây mới thử kết nối Blynk 1 lần nếu đang mất kết nối
      if (millis() - last_blynk_connect_attempt > 10000) 
      {
        last_blynk_connect_attempt = millis();
        Serial.println("Connecting to Blynk...");
        Blynk.connect();
      }
    } 
    else 
    {
      Blynk.run();
    }
  }
}
