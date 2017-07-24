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
String             temp_str;

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

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
}

void loop() {
  serialEvent();
  yield();

  server.handleClient();
  webSocket.loop();
}
