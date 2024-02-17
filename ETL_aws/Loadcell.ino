void loadCellSetup() {
  scale.begin(DOUT, CLK);
  float scaleValue = preferences.getFloat("scaleValue", 50);
  scale.set_scale(scaleValue);
  delay(1000);
  scale.tare();
}
void RTCsetup() {

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    //    while (1) delay(10);
  }
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  //      rtc.setTime(16, 11, 0);     // Set the time to 12:00:00 (24hr format)
  //      rtc.setDate(6, 8, 2019);   // Set the date to Dec 7th, 2014
  // Check xem SD có chạy được hay không.
  //  if (!sd.begin(chipSelect)) {
  //    // Nếu không báo đèn đỏ
  //    digitalWrite(LEDBLU, LOW);
  //    digitalWrite(LEDRED, HIGH);
  //  }
}
