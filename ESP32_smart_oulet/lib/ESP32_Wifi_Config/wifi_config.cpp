/*
Wifi config library for ESP32
Author: Phuong Dang
04/2026
*/
#include "wifi_config.h"

Preferences esp_pref;
String Get_wifi_list();

WebServer server(80);

const char web_page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Config Wifi của Phương</title>
  <meta charset="utf-8"> 
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background-color: #f4f7f9; text-align: center; padding-top: 50px; }
    .content { max-width: 350px; margin: 0 auto; padding: 20px; background: white; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
    select, input, button { width: 100%; padding: 12px; margin: 10px 0; border-radius: 5px; border: 1px solid #ccc; box-sizing: border-box; }
    button { background-color: #00afef; color: white; border: none; font-weight: bold; cursor: pointer; }
    h2 { color: #333; }
  </style>
</head>
<body>
  <div class="content">
    <h2>Cài đặt WiFi</h2>
    <form action="/save" method="POST">
      <label style="display:block; text-align:left;">Chọn WiFi:</label>
      <select name="ssid">
        %WIFI_LIST% </select>
      <input type="password" name="pass" placeholder="Mật khẩu WiFi" required>
      <button type="submit">LƯU VÀ KẾT NỐI</button>
    </form>
  </div>
</body>
</html>)rawliteral";

void Handle_Root()
{
  String html = web_page;
  html.replace("%WIFI_LIST%", Get_wifi_list());
  server.send(200, "text/html", html);
}

void Handle_Save()
{
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  if(ssid.length() > 0)
  {
    esp_pref.begin("wifi", false); // namespace "wifi", read/write mode
    esp_pref.putString("ssid", ssid); // Lưu tên wifi vào key "ssid"
    esp_pref.putString("pass", pass); // Lưu pass vào key "pass"
    esp_pref.end(); // Đóng Preferences

    Serial.println("Save WiFi info to Preferences");

    // Phản hồi cho user
    String msg = "<html><head><meta charset='utf-8'></head><body>";
    msg += "<p>Connecting to: <b>" + ssid + "</b></p>";
    msg += "</body></html>";
    server.send(200, "text/html", msg);

    delay(2000);
    ESP.restart();
  }
  else 
  {
    server.send(200, "text/html", "Lỗi: SSID không được để trống!");
  }
}

String Get_wifi_list() 
{
  String list = "";
  int n = WiFi.scanNetworks(); // Quét mạng
  
  if (n == 0) {
    return "<option value=''>Không tìm thấy WiFi nào</option>";
  }

  //Bubble sort (mạng mạnh nhất lên đầu)
  int indices[n];
  for (int i = 0; i < n; i++) indices[i] = i; // Khởi tạo mảng chỉ số

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
        // Tráo đổi vị trí nếu mạng sau mạnh hơn mạng trước
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
      }
    }
  }
  // --- KẾT THÚC SORT ---

  // Tạo danh sách HTML sau khi đã sắp xếp
  for (int i = 0; i < n; ++i) {
    int idx = indices[i];
    int rssi = WiFi.RSSI(idx);
    String ssid = WiFi.SSID(idx);
    
    // Chọn Icon cột sóng dựa trên độ mạnh
    String icon = " •)"; // Mặc định trung bình
    if (rssi >= -55)      icon = " •)))"; // Rất mạnh
    else if (rssi >= -70) icon = " •))";    // Mạnh
    else if (rssi < -85)  icon = " ⚠️";      // Yếu

    list += "<option value='" + ssid + "'>";
    list += ssid + icon;
    list += "</option>";
  }
  
  return list;
}

void Reset_wifi_config()
{
    esp_pref.begin("wifi", false); // namespace "wifi", read/write mode
    esp_pref.clear(); // Xóa sạch mọi thứ trong namespace "wifi"
    esp_pref.end();
    Serial.println("WiFi info cleared from Preferences. Restarting...");
    delay(500);
    ESP.restart();
}

void Server_setup()
{
  server.on("/", Handle_Root);
  server.on("/save", Handle_Save);
  server.begin();
}

void Init_config(const char* ssid, const char* password)
{
  esp_pref.begin("wifi", true); // read only
  String saved_ssid = esp_pref.getString("ssid", ""); // Lấy SSID đã lưu, mặc định là rỗng
  String saved_pass = esp_pref.getString("pass", ""); // Lấy pass đã lưu, mặc định là rỗng
  esp_pref.end();
  
  if (saved_ssid != "") 
  {
    Serial.println("Connecting to: " + saved_ssid);
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());

    // timeout 10s
    uint8_t count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 20) 
    {
      delay(500);
      Serial.print(".");
      count++;
    }
    if (WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("\nWiFi ĐÃ KẾT NỐI!");
      Serial.println(WiFi.localIP());
      for(int i=0; i<30; i++)
      {
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(200);
        digitalWrite(LED_BUILTIN, LOW); 
        delay(200);
      }
      // GỌI HÀM KẾT NỐI BLYNK TẠI ĐÂY
      return; // Thoát setup, không phát AP nữa
    }
  }
  WiFi.softAP(ssid, password); // ssid, password
  Serial.println(WiFi.softAPIP()); // địa chỉ IP của AP
}