/*
Wifi config library for ESP32
Author: Phuong Dang
04/2026
*/
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>

typedef struct 
{
    String ssid;
    String password;
} Wifi_config_TypeDef;

class Wifi_config
{
    private:
        WebServer _server;
        Preferences esp_pref;
        Wifi_config_TypeDef _wifi_conf;
        void Handle_Root();
        void Handle_Save();
        void Server_setup();
    public:
        bool Is_connected();
        Wifi_config(uint16_t port = 80); // constructor default port 80
        void Init_config(Wifi_config_TypeDef *conf);
        String Get_wifi_list();
        void Reset_wifi_config();
        void Handle_Client();
};


#endif
