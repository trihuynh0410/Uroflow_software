void AddCommands() {
//  Cmds.AddCommand(F("GetConnectionColor"), Cmd_ConnectionColor);
//  Cmds.AddCommand(F("Tare"), Cmd_Tare);
//  Cmds.AddCommand(F("SetScale"), Cmd_SetScale);
//  Cmds.AddCommand(F("Update"), Cmd_Update);
//  Cmds.SetDefaultHandler(Cmd_Unknown);
}
//void Cmd_ConnectionColor(CommandParameter &p)
//{
//  SerialBT.println("Blue");
//}
//void Cmd_Tare(CommandParameter &p)
//{
//  delay(500);
//  scale.tare();
//  SerialBT.println("Tared");
//}
//void Cmd_SetScale(CommandParameter &p)
//{
//  float scaleValue = p.NextParameterAsDouble(0);
//  if (scaleValue != 0) {
//    scale.set_scale(scaleValue);
//    SerialBT.print("Set scale:");
//    SerialBT.println(scaleValue);
//  } else {
//    scaleValue = 50;
//    scale.set_scale(scaleValue);
//    SerialBT.print("Set scale:");
//    SerialBT.println(scaleValue);
//  }
//  preferences.putFloat("scaleValue", scaleValue);
//  preferences.end();
//}
//void Cmd_SetTime(CommandParameter &p)
//{
//  float timeUni = p.NextParameterAsDouble(0);
//  if (scaleValue != 0) {
//    scale.set_scale(scaleValue);
//    SerialBT.print("Set scale:");
//    SerialBT.println(scaleValue);
//  } else {
//    scaleValue = 50;
//    scale.set_scale(scaleValue);
//    SerialBT.print("Set scale:");
//    SerialBT.println(scaleValue);
//  }
//  preferences.putFloat("scaleValue", scaleValue);
//  preferences.end();
//}
//void Cmd_Update(CommandParameter &p)
//{
//  SerialBT.print("Weight(g):");
//  SerialBT.println(scale.get_units());
//  
//}
//void Cmd_Unknown()
//{
//  Serial.println(F("I don't know that command. Try another. "));
//  Serial.println(F("!Tare"));
//  Serial.println(F("!SetScale 1234"));
//}
//String getMACinString() {
//  const uint8_t* macAddress = esp_bt_dev_get_address();
//  char charMAC[6];
//
//  sprintf(charMAC, "%02X", (int)macAddress[3]);
//  sprintf(charMAC + 2, "%02X", (int)macAddress[4]);
//  sprintf(charMAC + 4, "%02X", (int)macAddress[5]);
//  //  charMAC[2] = ':';
//  //  sprintf(charMAC + 3, "%02X", (int)macAddress[1]);
//  //  charMAC[5] = ':';
//  //
//  //  sprintf(charMAC + 6, "%02X", (int)macAddress[2]);
//  //  charMAC[8] = ':';
//  //  sprintf(charMAC + 9, "%02X", (int)macAddress[3]);
//  //  charMAC[11] = ':';
//  //
//  //  sprintf(charMAC + 12, "%02X", (int)macAddress[4]);
//  //  charMAC[14] = ':';
//  //  sprintf(charMAC + 15, "%02X", (int)macAddress[5]);
//
//  return (String)charMAC;
//}
