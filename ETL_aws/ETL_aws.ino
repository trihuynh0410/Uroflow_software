#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "WiFi.h"
#include <SPIFFS.h>
#include "sensitive.h"

WiFiClientSecure net = WiFiClientSecure();

void uploadFileToS3() {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    String url = API_URL + String(AWS_S3_BUCKET) + String(file.name());
    Serial.println("Uploading to: " + url);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "text/csv"); 

    String fileContent;
    while (file.available()){
        fileContent += char(file.read());
    }
    int httpResponseCode = http.PUT(fileContent); 
    if (httpResponseCode > 0) {
        Serial.println("Response code: " + String(httpResponseCode));
        Serial.println(http.getString());
    } else {
        Serial.println("Error on sending file: " + String(httpResponseCode));
    }

    file.close();
    http.end();
}


String extractFileName(String path) {
  int lastIndex = path.lastIndexOf('/');
  return path.substring(lastIndex + 1);
}


void setup(){
    Serial.begin(115200);
    delay(1000);
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }
    Serial.println("Wifi connected");
    uploadFileToS3();
}

void loop(){

}