#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting...");

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  bool numberFound = false;
  while (file) {
    Serial.print("Reading FILE: ");
    Serial.println(file.name());

    while (file.available()) {
      String line = file.readStringUntil('\n');
      if (line.indexOf("113303") >= 0) {
        numberFound = true;
        Serial.println("Number 113303 found in the file.");
        break; // Break out of the loop if the number is found
      }
    }

    if (!numberFound) {
      Serial.println("Number 113325 not found in the file.");
    }

    file = root.openNextFile();
  }
}

void loop() {
  // Empty loop
}
