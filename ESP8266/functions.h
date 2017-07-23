void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

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

    String line = inputString;
    // Limpa a string temporária
    inputString = "";
    stringComplete = false;

    if (line.indexOf("DADOS:") != -1) { 
      String tmpline = line.substring(6);
      webSocket.broadcastTXT(tmpline);
    }

    if (line.indexOf("IP?") != -1) {
      Serial.print("IP:");
      Serial.println(WiFi.localIP());
    }
  }
}

void saveConfig() {
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
