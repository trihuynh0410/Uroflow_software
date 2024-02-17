void lightRed() {
  DEBUG_PRINT("RED");
  digitalWrite(LEDBLU, LOW);
  digitalWrite(LEDRED, HIGH);
  digitalWrite(LEDGRE, LOW);
}
void lightGreen() {
  //  DEBUG_PRINT("GREEN");
  digitalWrite(LEDBLU, LOW);
  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDGRE, HIGH);
}
void lightYellow() {
  //  DEBUG_PRINT("YELLOW");
  digitalWrite(LEDBLU, LOW);
  digitalWrite(LEDRED, HIGH);
  digitalWrite(LEDGRE, HIGH);
}
void lightBlue() {
  //  DEBUG_PRINT("BLUE");
  digitalWrite(LEDBLU, HIGH);
  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDGRE, LOW);
}
void lightWhite() {
  //  DEBUG_PRINT("BLUE");
  digitalWrite(LEDBLU, HIGH);
  digitalWrite(LEDRED, HIGH);
  digitalWrite(LEDGRE, HIGH);
}
void offLight() {
  DEBUG_PRINT("LEDOFF");
  digitalWrite(LEDBLU, LOW);
  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDGRE, LOW);
}
//void loadConfiguration( Config &config) {
//  // Open file for reading
//  File file = SPIFFS.open("/config.txt");
//  // Allocate a temporary JsonDocument
//  // Don't forget to change the capacity to match your requirements.
//  // Use https://arduinojson.org/v6/assistant to compute the capacity.
//  StaticJsonDocument<512> doc;
//
//  // Deserialize the JSON document
//  DeserializationError error = deserializeJson(doc, file);
//  if (error)
//    Serial.println(F("Failed to read file, using default configuration"));
//
//  // Copy values from the JsonDocument to the Config
//  config.loadCellValue = doc["loadCellValue"] | 1000;
//  //  strlcpy(config.hostname,                  // <- destination
//  //          doc["hostname"] | "example.com",  // <- source
//  //          sizeof(config.hostname));         // <- destination's capacity
//
//  // Close the file (Curiously, File's destructor doesn't close the file)
//  file.close();
//}
//void saveConfiguration( const Config &config) {
//  // Delete existing file, otherwise the configuration is appended to the file
//  SPIFFS.remove("/config.txt");
//
//  // Open file for writing
//  File file = SPIFFS.open("/config.txt");
//  if (!file) {
//    Serial.println(F("Failed to create file"));
//    return;
//  }
//
//  // Allocate a temporary JsonDocument
//  // Don't forget to change the capacity to match your requirements.
//  // Use https://arduinojson.org/assistant to compute the capacity.
//  StaticJsonDocument<512> doc;
//
//  // Set the values in the document
//  doc["loadCellValue"] = config.loadCellValue;
//  doc["port"] = config.port;
//
//  // Serialize JSON to file
//  if (serializeJson(doc, file) == 0) {
//    Serial.println(F("Failed to write to file"));
//  }
//
//  // Close the file
//  file.close();
//}
