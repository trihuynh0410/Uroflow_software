void spiffsSetup() {
   if (!SPIFFS.begin(true)) {
     Serial.println("An Error has occurred while mounting SPIFFS");
     lightRed();
     while (1) {}
     return;
   }
  // if (!SD.begin()) {
  //   Serial.println("Card Mount Failed");
  //   lightRed();
  //   delay(5000);
  //   return;
  // }
}

void writeHeader() {
  DateTime now = rtc.now();
  String filename = "/";
  String u = String(now.unixtime());
  filename.concat(u);
  Serial.println(filename);
  filedata = SPIFFS.open(filename, FILE_WRITE);
  if (!filedata) {
    Serial.println("There was an error opening the file for writing");
    lightRed();
    while (1) {}
    return;
  }
  filedata.print("#,");
  filedata.println(now.unixtime());
  initLoadCell = bufferData[0];
}

void dataWrite(long miliTime, int data ) {
  filedata.print(miliTime);
  filedata.write(',');
  filedata.print(data);
  filedata.println();
}

void closeFile() {
  if (filedata) {
    filedata.flush();
    filedata.close();
  }
}
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

//String getContentType(String filename) {
//  if (server.hasArg("download")) {
//    return "application/octet-stream";
//  } else if (filename.endsWith(".htm")) {
//    return "text/html";
//  } else if (filename.endsWith(".html")) {
//    return "text/html";
//  } else if (filename.endsWith(".css")) {
//    return "text/css";
//  } else if (filename.endsWith(".js")) {
//    return "application/javascript";
//  } else if (filename.endsWith(".png")) {
//    return "image/png";
//  } else if (filename.endsWith(".gif")) {
//    return "image/gif";
//  } else if (filename.endsWith(".jpg")) {
//    return "image/jpeg";
//  } else if (filename.endsWith(".ico")) {
//    return "image/x-icon";
//  } else if (filename.endsWith(".xml")) {
//    return "text/xml";
//  } else if (filename.endsWith(".pdf")) {
//    return "application/x-pdf";
//  } else if (filename.endsWith(".zip")) {
//    return "application/x-zip";
//  } else if (filename.endsWith(".gz")) {
//    return "application/x-gzip";
//  }
//  return "text/plain";
//}
//
//bool exists(String path) {
//  bool yes = false;
//  File file = FILESYSTEM.open(path, "r");
//  if (!file.isDirectory()) {
//    yes = true;
//  }
//  file.close();
//  return yes;
//}
//
//bool handleFileRead(String path) {
//  DEBUG_PRINT("handleFileRead: " + path);
//  if (path.endsWith("/")) {
//    path += "index.htm";
//  }
//  String contentType = getContentType(path);
//  String pathWithGz = path + ".gz";
//  if (exists(pathWithGz) || exists(path)) {
//    if (exists(pathWithGz)) {
//      path += ".gz";
//    }
//    File file = FILESYSTEM.open(path, "r");
//    server.streamFile(file, contentType);
//    file.close();
//    return true;
//  }
//  return false;
//}
//
//void handleFileUpload() {
//  if (server.uri() != "/edit") {
//    return;
//  }
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    String filename = upload.filename;
//    if (!filename.startsWith("/")) {
//      filename = "/" + filename;
//    }
//    DEBUG_PRINT("handleFileUpload Name: "); DEBUG_PRINT(filename);
//    fsUploadFile = FILESYSTEM.open(filename, "w");
//    filename = String();
//  } else if (upload.status == UPLOAD_FILE_WRITE) {
//    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
//    if (fsUploadFile) {
//      fsUploadFile.write(upload.buf, upload.currentSize);
//    }
//  } else if (upload.status == UPLOAD_FILE_END) {
//    if (fsUploadFile) {
//      fsUploadFile.close();
//    }
//    DEBUG_PRINT("handleFileUpload Size: "); DEBUG_PRINT(upload.totalSize);
//  }
//}
//
//void handleFileDelete() {
//  if (server.args() == 0) {
//    return server.send(500, "text/plain", "BAD ARGS");
//  }
//  String path = server.arg(0);
//  DEBUG_PRINT("handleFileDelete: " + path);
//  if (path == "/") {
//    return server.send(500, "text/plain", "BAD PATH");
//  }
//  if (!exists(path)) {
//    return server.send(404, "text/plain", "FileNotFound");
//  }
//  FILESYSTEM.remove(path);
//  server.send(200, "text/plain", "");
//  path = String();
//}
//
//void handleFileCreate() {
//  if (server.args() == 0) {
//    return server.send(500, "text/plain", "BAD ARGS");
//  }
//  String path = server.arg(0);
//  DEBUG_PRINT("handleFileCreate: " + path);
//  if (path == "/") {
//    return server.send(500, "text/plain", "BAD PATH");
//  }
//  if (exists(path)) {
//    return server.send(500, "text/plain", "FILE EXISTS");
//  }
//  File file = FILESYSTEM.open(path, "w");
//  if (file) {
//    file.close();
//  } else {
//    return server.send(500, "text/plain", "CREATE FAILED");
//  }
//  server.send(200, "text/plain", "");
//  path = String();
//}
//
//void handleFileList() {
//  if (!server.hasArg("dir")) {
//    server.send(500, "text/plain", "BAD ARGS");
//    return;
//  }
//
//  String path = server.arg("dir");
//  DEBUG_PRINT("handleFileList: " + path);
//
//
//  File root = FILESYSTEM.open(path);
//  path = String();
//
//  String output = "[";
//  if (root.isDirectory()) {
//    File file = root.openNextFile();
//    while (file) {
//      if (output != "[") {
//        output += ',';
//      }
//      output += "{\"type\":\"";
//      output += (file.isDirectory()) ? "dir" : "file";
//      output += "\",\"name\":\"";
//      output += String(file.name()).substring(1);
//      output += "\"}";
//      file = root.openNextFile();
//    }
//  }
//  output += "]";
//  server.send(200, "text/json", output);
//}
//void FSBrowserSetup() {
//  FILESYSTEM.begin();
//  {
//    File root = FILESYSTEM.open("/");
//    File file = root.openNextFile();
//    while (file) {
//      String fileName = file.name();
//      size_t fileSize = file.size();
//      //      DEBUG_PRINTF("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
//      file = root.openNextFile();
//    }
//    DEBUG_PRINT("\n");
//  }
//
//
//  //WIFI INIT
//  //  DEBUG_PRINF("Connecting to %s\n", ssid);
//  if (String(WiFi.SSID()) != String(ssid)) {
//    WiFi.mode(WIFI_STA);
//    WiFi.begin(ssid, password);
//  }
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    DEBUG_PRINT(".");
//  }
//  DEBUG_PRINT("");
//  DEBUG_PRINT("Connected! IP address: ");
//  DEBUG_PRINT(WiFi.localIP());
//
//  MDNS.begin(host);
//  DEBUG_PRINT("Open http://");
//  DEBUG_PRINT(host);
//  DEBUG_PRINT(".local/edit to see the file browser");
//
//
//  //SERVER INIT
//  //list directory
//  server.on("/list", HTTP_GET, handleFileList);
//  //load editor
//  server.on("/edit", HTTP_GET, []() {
//    if (!handleFileRead("/edit.htm")) {
//      server.send(404, "text/plain", "FileNotFound");
//    }
//  });
//  //create file
//  server.on("/edit", HTTP_PUT, handleFileCreate);
//  //delete file
//  server.on("/edit", HTTP_DELETE, handleFileDelete);
//  //first callback is called after the request has ended with all parsed arguments
//  //second callback handles file uploads at that location
//  server.on("/edit", HTTP_POST, []() {
//    server.send(200, "text/plain", "");
//  }, handleFileUpload);
//
//  //called when the url is not defined here
//  //use it to load content from FILESYSTEM
//  server.onNotFound([]() {
//    if (!handleFileRead(server.uri())) {
//      server.send(404, "text/plain", "FileNotFound");
//    }
//  });
//
//  //get heap status, analog input value and all GPIO statuses in one json call
//  server.on("/all", HTTP_GET, []() {
//    String json = "{";
//    json += "\"heap\":" + String(ESP.getFreeHeap());
//    json += ", \"analog\":" + String(analogRead(A0));
//    json += ", \"gpio\":" + String((uint32_t)(0));
//    json += "}";
//    server.send(200, "text/json", json);
//    json = String();
//  });
//  server.begin();
//  DEBUG_PRINT("HTTP server started");
//}
