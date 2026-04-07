#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <Button_handle.h>
#include "wifi_config.h"

Button_TypeDef button_reset_wifi;

void setup() 
{
  Button_init(&button_reset_wifi, INPUT_PULLUP, 18); // GPIO18
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  Init_config("SmartOutletSetup", "12345678");
  Server_setup();
}

void loop() 
{
  Button_handle(&button_reset_wifi);
  server.handleClient();
}
