#include <Wire.h>
#include <HX711.h>
#include <CircularBuffer.h>
#include <ArduinoJson.h>
#include "RTClib.h"
#include "FS.h"
#include "SPI.h"
#include <Preferences.h>
#include "ArduinoTimer.h"
#include <HTTPClient.h>
#include "WiFi.h"
#include "sensitive.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println (x)
#define DEBUG_PRINF(x)  Serial.printf (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINF(x)
#endif
CircularBuffer<long, 80> bufferData;
CircularBuffer<long, 80> bufferTime;

//File file ;
File filedata ;

//holds the current upload
//File fsUploadFile;
RTC_DS3231 rtc;
HX711 scale;
#define FILESYSTEM SPIFFS
#if FILESYSTEM == SPIFFS
#include <SPIFFS.h>
#endif
#define LEDBLU 04
#define LEDGRE 16
#define LEDRED 17
#define BATTERY 34
#define DOUT 32
#define CLK 33
#define SWITCHWIFI 25
#define BATTERY_PIN 34

int count = 0;
int noChangeCount = 0;
bool record = false;
int initLoadCell = 0;
Preferences preferences;

#define error(msg) sd.errorHalt(F(msg))

// bool fileUploaded = false; // Add a flag to indicate whether the file has been uploaded

void uploadFileToS3()
{
    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection and whether the file has been uploaded

        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        if (!file) {
            Serial.println("Failed to open file for uploading");
            return;
        }
        String url = API_URL + String(AWS_S3_BUCKET) + String(file.name());
        Serial.println("Uploading to: " + url);

        HTTPClient http;

        for (int attempt = 0; attempt < 3; attempt++)
        {
            http.begin(url); 
            http.addHeader(F("Content-Type"), F("text/csv"));

            int httpResponseCode = http.sendRequest("PUT", &file, file.size());

            if (httpResponseCode > 0)
            {
                Serial.print(F("Response code: "));
                Serial.println(httpResponseCode);
                Serial.println(http.getString());
                if (httpResponseCode == 200)
                {
                    // fileUploaded = true; // Set the flag to true after successful upload
                    Serial.println("upload success");
                    break; 
                }
            }
            else
            {
                Serial.print(F("Error on sending file: "));
                Serial.println(httpResponseCode);
            }
            http.end();  
            delay(200); 
        }
        String deletefile = "/" + String(file.name());
        file.close(); // Close the file
        if (SPIFFS.remove(deletefile)) { // Delete the file
          Serial.println("File deleted after upload");
        } else {
          Serial.println("Error deleting file");
        }
        Serial.println("Wait to setup for uroflow");
        lightYellow();
        delay(10000);
    }
}
void setup() {
  String BTname = "IUROF";
  preferences.begin("iurof", false);
  pinMode(LEDBLU, OUTPUT);
  pinMode(LEDGRE, OUTPUT);
  pinMode(LEDRED, OUTPUT);
  lightWhite();
  pinMode(SWITCHWIFI, INPUT_PULLUP);
  Serial.begin(115200);
  spiffsSetup();
  RTCsetup();
  int pinValue = analogRead(BATTERY_PIN);
  if (pinValue < 2200) {
    Serial.println("Low battery");
    for (int i = 0; i < 5; i++) {
      lightYellow();
      delay(200);
      lightWhite();
      delay(200);
    }

  } else {
    delay(2000);
  }
  loadCellSetup();
  delay(1000);
  //  FSBrowserSetup();
  lightBlue();

  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  // wm.resetSettings();

  // wm.setConfigPortalBlocking(false);
  bool res;

  res = wm.autoConnect("UroflowWifi"); // anonymous ap
  Serial.println("check before check res");
  if(!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } 
  else {
    Serial.println("connected...yeey :)");
  }

}
void loop() {
  int j = 0;
  if (true) {
    lightBlue();
    bufferData.push(  scale.get_units() * 10);
    bufferTime.push(  millis());
        Serial.println("Buffer data (not record) is");
        Serial.println(-bufferData[0]);
    //Ghi lại 80 data đầu tiên,tương đương với 1s
    if (((-bufferData[bufferData.size() - 1]) - (-bufferData[0]) > 80) && (bufferData.size() > 50)) {
        Serial.println("Buffer data initial(not record) is");
        Serial.println(-bufferData[0]);
        Serial.println("COndition value is");
        Serial.println(-bufferData[bufferData.size() - 1] + bufferData[0]);
      writeHeader();
      record = true;
      Serial.println("Record start");
      lightGreen();
      while (record) {
        bufferData.push(scale.get_units() * 10);
        bufferTime.push(millis());
        dataWrite(bufferTime[0], -bufferData[0]);
        Serial.println("Buffer data (record 0) is");
        Serial.println(-bufferData[0]);
        Serial.println("Buffer data (record 1) is");
        Serial.println(-bufferData[bufferData.size() - 1]);
        Serial.println("Difference is");
        Serial.println(-bufferData[bufferData.size() - 1] + bufferData[0]);
        Serial.println("check true before after");
        Serial.println(-bufferData[bufferData.size() - 1] + bufferData[0] < 30);
        //         Serial.println("check true 500");
        // Serial.println(-bufferData[0] < (initLoadCell + 500));
        count++;
        if (count % 100 == 0) {
          filedata.flush();
          // Serial.println("saved something");
//          Cmds.Process();
        }
        //        DEBUG_PRINT("0");
        // DEBUG_PRINT(bufferData[bufferData.size()]);
        //                DEBUG_PRINT(bufferTime[bufferTime.size() - 1]);
        //        Serial.println(analogRead(BATTERY_PIN));
        if (-bufferData[bufferData.size() - 1] + bufferData[0] < 30) {
          noChangeCount++;    //  
          Serial.println(noChangeCount);
          if (noChangeCount > 500 && -bufferData[0] < (initLoadCell + 500)) {
            record = false;
            lightBlue();
            noChangeCount = 0;
            closeFile();
            Serial.println("Close file");
            uploadFileToS3(); 
          }
        }
      }
    }
  } else {
    lightYellow();    
    //Wifi mode
//        server.handleClient();
    //    DEBUG_PRINT("1");
    delay(2);//allow the cpu to switch to other tasks
  }

}

