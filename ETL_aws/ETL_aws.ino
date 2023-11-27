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

    // bool numberFoundd = false;
    // String fileContent;
    // while (file.available()){
    //     String line = file.readStringUntil('\n');
    //     fileContent += line + "\n"; // Append each line to the file content
    //     if (line.indexOf("113303") >= 0) {
    //         numberFoundd = true;
    //         Serial.println("Number 113303 found in the line.");
    //     }
    // }
    // bool numberFound = false;
    // if (fileContent.indexOf("113303") >= 0) {
    //     numberFound = true;
    //     Serial.println("Number 113303 found in the file.");
    // } else {
    //     Serial.println("Number 113303 not found in the file.");
    // }

    for (int attempt = 0; attempt < 3; attempt++) {
        http.begin(url); // Begin the HTTP request
        http.addHeader("Content-Type", "text/csv");

        int httpResponseCode = http.sendRequest("PUT", &file, file.size());

        if (httpResponseCode > 0) {
            Serial.println("Response code: " + String(httpResponseCode));
            Serial.println(http.getString());
            if (httpResponseCode == 200) {
                break; // Break the loop if the response is 200
            }
        } else {
            Serial.println("Error on sending file: " + String(httpResponseCode));
        }

        http.end(); // End the HTTP connection
        delay(1000); // Delay for 1 second before retrying
    }
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
    // uint32_t notConnectedCounter = 0;
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(100);
    //     Serial.println("Wifi connecting...");
    //     notConnectedCounter++;
    //     if(notConnectedCounter > 150) { // Reset board if not connected after 15s
    //         Serial.println("Resetting due to Wifi not connecting...");
    //         ESP.restart();
    //     }
    // }
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
