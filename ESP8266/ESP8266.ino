#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Hash.h>
#include "FS.h"

//Serial
String inputString = "";         // Armazena os dados que estão chegando
boolean stringComplete = false;  // se a string está completa
String line;

//WebSockets
uint8_t            socketNumber;
String             socket_cmd;

//Configurações e resto
String        pagina, tensao, calibra, apnome, apsenha;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webSocket(81);

#include "functions.h"

void setup() {
  //wifi_set_sleep_type(MODEM_SLEEP_T);
  Serial.begin(9600);
  delay(1000);

  //Texto (piscada) de início
  Serial.println("ESP8266 0.7.1");
  Serial.print("Iniciando SPIFFS... ");
  yield();

  //Iniciando SPIFFS
  SPIFFS.begin();
  Serial.println("OK.");
  yield();

  //Carrega e configura tudo
  loadConfig();
  setupWiFi();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("TRANSMITINDO SERIAL");
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
  inputString.reserve(256);
  httpUpdater.setup(&server);
  server.begin();
  yield();

  //Lê arquivos e executa funções dependendo do URL
  server.on("/", HTTP_GET, []() {
    handleFileRead("/");
  });
  server.on("/config", HTTP_GET, []() {
    handleFileRead("/ajustes.html");
  });
  server.on("/salvar", saveConfig);
  server.on("/notepad", testeNotepad);

  //Esse não é o branch SENAI, né?
  server.on("/easteregg", HTTP_GET, []() {
    handleFileRead("/easteregg.htm");
  });

  server.on("/config.txt", HTTP_GET, []() {
    //Inserir um server.authenticate aqui.
    //Acho que é assim:
    /*if (!server.authenticate("1diadeaula","clebinho"))*/server.send(403, "text/plain", "Você quase conseguiu a senha do WiFi. Quase.");
  });
  
  server.on("/wifilist", HTTP_GET, []() {
    server.send(200, "text/json", listWifi());
  });

  server.on("/dados", HTTP_GET, []() {
    //Decidir o que fazer quando não tiver nada no serial.
    String tmpline = line.substring(6);
    server.send(200, "text/json", tmpline);
  });

  
  server.on("/edit", HTTP_GET, []() {
    //server.send(200, "text/html", "<form method='POST' action='/edit' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Enviar'></form>");
    handleFileRead("edit.htm");
  });
  //Cria um arquivo
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //Apaga um arquivo
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //Primeiro callback indica que a requisição (com os argumentos) aconteceu
  //O segundo callback envia o arquivo.
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "Arquivo enviado ou [100%] atualizado");
  }, handleFileUpload);
  server.on("/list", HTTP_GET, handleFileList);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
}

void loop() {
  serialEvent();
  yield();

  if (line.indexOf("DADOS:") != -1) {
    String tmpline = line.substring(6);
    webSocket.broadcastTXT(tmpline);
  }

  if (line.indexOf("IP?") != -1) {
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
  }

  server.handleClient();
  webSocket.loop();

  //Recebe comandos por WebSockets (APENAS CARACTERES ASCII)
  if (socket_cmd.indexOf("WIFILIST") != -1) {
    String memoriapraque = listWifi();
    webSocket.sendTXT(socketNumber, memoriapraque);
  }

  socket_cmd = "";
}
