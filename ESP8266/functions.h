void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { //olho no typo

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      yield();
      break;

    case WStype_CONNECTED: {                  // Braces required http://stackoverflow.com/questions/5685471/error-jump-to-case-label
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        yield();
        socketNumber = num;
        break;
      }

    case WStype_TEXT:
      //Comandos que comecem com '#' são envidados diretamente pro serial
      //Da mesma forma, é possível enviar dados para o navegador (ver a programação do ATmega)
      //Ou seja, serial remoto.
      if (payload[0] == '#') {
        //String tmp = payload.substring(1);
        Serial.printf("[%u] get Text: %s\n", num, payload);
        yield();
      }

      if (payload[0] == '*') {
      for (int i = 1; i < lenght; i++) {
        socket_cmd += payload[i];
      }
      
      }

      break;

    case WStype_ERROR:
      Serial.printf("Error [%u] , %s\n", num, payload);
      yield();
  }
}


String getContentType(String filename) {
  yield();
  if (server.hasArg("download"))      return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html"))return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js"))  return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".m4a")) return "audio/m4a";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz"))  return "application/x-gzip";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  return "text/plain";
}


bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);

  if (path.endsWith("/"))
  {
    path += "index.html";
  }

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  Serial.println("PathFile: " + pathWithGz);

  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz)) path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  yield();
  return false;
}


void loadConfig() {
  //Abre as configurações salvas no config.txt.
  //SPIFFS é um sistema de arquivos interno, algo como um HD.
  File f = SPIFFS.open("/config.txt", "r");
  if (f.find("tensao="));   tensao =  f.readStringUntil('\r');
  if (f.find("calibra="));  calibra = f.readStringUntil('\r');
  if (f.find("AP_nome="));  apnome =  f.readStringUntil('\r');
  if (f.find("AP_senha=")); apsenha = f.readStringUntil('\r');
  f.close();
}

void setupWiFi()
{
  int t = 0;
  const char* ssid = apnome.c_str();
  const char* senha = apsenha.c_str();
  //Conectar ponto de acesso
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    t++;
    if (t > 20) {
      Serial.println("\r\nNão foi possivel conectar ao ponto de acesso. Iniciando apenas em modo AP");
      Serial.println("IP:192.168.4.1");
      WiFi.mode(WIFI_AP);
      WiFi.softAP("SMCP", "25324646");
      break;
    }
  }
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    } else {
      inputString += inChar;
    }
  }
  yield();
  if (stringComplete) {

    line = inputString;
    // Limpa a string temporária
    inputString = "";
    stringComplete = false;
  }
}

void saveConfig() {
  //Dividir esse procedimento em dois:
  //1. A parte das configurações rápidas
  //2. Config. de WiFi (que precisam reiniciar o aparelho)
  //Por que não usar parâmetros?
  tensao  = server.arg("tensao");
  calibra = server.arg("calibra");
  apnome  = server.arg("apnome");
  apsenha = server.arg("apsenha");

  //Salva os novos dados no config.txt
  File f = SPIFFS.open("/config.txt", "w");
  f.print("tensao=");   f.println(tensao);
  f.print("calibra=");  f.println(calibra);
  f.print("AP_nome=");  f.println(apnome);
  f.print("AP_senha="); f.println(apsenha);
  f.close();
  server.send(200, "text/plain", "OK");
  webSocket.sendTXT(socketNumber, "Reiniciando");
  Serial.println("Dados salvos. Reiniciando...");
  yield();
  delay(1000);
  ESP.restart();
}

#define DBG_OUTPUT_PORT Serial
//holds the current upload
File fsUploadFile;

void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  String path;
  if (!server.hasArg("dir")) {
    path = "/";
  }
  else {
    path = server.arg("dir");
  }
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

//Teste rudimentar para envio de arquivos
void testeNotepad() {
  String fname = server.arg("filename");
  String content = server.arg("content");
  File f = SPIFFS.open("/" + fname, "w");
  f.print(content);
  f.close();
  server.send(200, "text/plain", "Provavelmente foi. \r\nFilename: " + fname + "\r\nContent: " + content);
}

String listWifi() { // Por que não String?
  int num = WiFi.scanNetworks();
  int i;
  String tmp; //Achar alguma solução que gaste menos memória, porque olha...
  tmp += "[";
  for (i = 0; i < (num - 1); i++) {
    tmp += "{\"ssid\":\"";
    tmp += (WiFi.SSID(i));
    tmp += "\",\"rssi\":";
    tmp += (WiFi.RSSI(i));
    tmp += "},";
  }
  tmp += "{ \"ssid\":\"";
  tmp += (WiFi.SSID(num));
  tmp += "\",\"rssi\":";
  tmp += (WiFi.RSSI(num));
  tmp += "}]";
  return tmp;
}
