#include "SPIFFS.h"


void setup() {

  Serial.begin(115200);
  delay(2000);
  Serial.println("....");
  Serial.println("Connected");
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
 
  while (file) {
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
