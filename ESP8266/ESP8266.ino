#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FS.h"

const byte    numChars = 48;            // Recebe 48 caracteres por vez, via serial.
char          receivedChars[numChars];  // Cadeia de caracteres para armazenar temporariamente os dados.
boolean       newData = false;
String        received;                 // String para armazenar os caracteres.
String        pagina, tensao, calibra, apnome, apsenha;
int t;
ESP8266WebServer server(80);
WiFiServer trafego(80);

//Inclui funções para a leitura do serial.
#include "serial.h"

//Inclui as outras funções de página, na outra aba.
#include "paginas.h"

void salvarconfig() {
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
  Serial.println("Dados salvos. Reiniciando...");
  delay(1000);
  ESP.restart();
}

void pegarOsDados() {
  Serial.println("DADOS?");
  delay(10);
  //Recebe os dados do ATMega
  recvWithEndMarker();
  showNewData();
  server.send(200, "application/json", received);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  Serial.println("\r\n");
  Serial.println("ESP8266 0.6.0");
  Serial.print("Iniciando SPIFFS... ");
  SPIFFS.begin();
  Serial.println("OK");
  delay(500);
  Serial.println("Carregando configuracoes...");

  //Abre as configurações salvas no config.txt.
  //SPIFFS é um sistema de arquivos interno, algo como um HD.
  File f = SPIFFS.open("/config.txt", "r");
  if (f.find("tensao="));   tensao =  f.readStringUntil('\r');
  if (f.find("calibra="));  calibra = f.readStringUntil('\r');
  if (f.find("AP_nome="));  apnome =  f.readStringUntil('\r');
  if (f.find("AP_senha=")); apsenha = f.readStringUntil('\r');
  f.close();

  //Copia as variáveis apnome e apsenha para uma sequência de char.
  //Por algum motivo, a função que conecta no WiFi só funciona com esse tipo de variável.
  const char* ssid = apnome.c_str();
  const char* senha = apsenha.c_str();

  //Envia as informações para o ATMega
  Serial.print("T:");       Serial.println(tensao);
  Serial.print("CALIBRA:"); Serial.println(calibra);

  Serial.print("Conectando ao ponto de acesso ");
  Serial.println(ssid);
  WiFi.begin(ssid, senha);

  //Conecta ao Wifi e tenta por ~10 segundos.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    t++;
    if (t > 20) {
      Serial.println("\r\nNão foi possivel conectar ao ponto de acesso. Iniciando apenas em modo AP");
      WiFi.mode(WIFI_AP);
      WiFi.softAP("SMCP", "clebinho");
      break;
    }
  }
  t = 0;
  Serial.print("\r\nWiFi conectado. Iniciando servidor... ");
  server.begin();
  Serial.println("OK.");

  //Executa uma função para cada página.
  //Olhe o arquivo paginas.h, na outra aba.
  server.on("/", handleRoot);
  server.on("/indice", carregarIndice);
  server.on("/dados", pegarOsDados);  //Linha 39.
  server.on("/configurar", configurar);
  server.on("/teste", exportudomesmo);
  server.on("/salvar", salvarconfig); //Esta função está na linha 20.
  server.on("/EASTEREGG", EasterEgg);
  server.onNotFound(QuatroZeroQuatro);
}

void loop() {
  recvWithEndMarker();
  showNewData();
  //O serial funciona ou é o meu ESP que tá quebrado?
  Serial.print("\nDEBUG: "); Serial.println(received);
  
  if (received.indexOf("IP?") != -1) {
    Serial.print("IP:"); Serial.println(WiFi.localIP());
  }
  received = "";
  server.handleClient();
}
