/* INCLUIR: 
 * Correção do URLencoded
 * Método alternativo ao Serial.find()
 * Testar o yield()
 * Testar WebSockets
 */


#include <ESP8266WiFi.h>
#include "FS.h"

String headerJSON = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nContent-Type: application/json;charset=utf-8\nConnection: close\n\n";
String headerHTML = "HTTP/1.1 200 OK\nContent-Type: text/html;charset=utf-8\nConnection: close\n\n";
String pagina, dados, tensao, calibra, apnome, apsenha;
int t, a, b, c, timeout;
int client_timeout;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1200);
  Serial.println("\r\n");
  Serial.println("ESP8266 0.5.4");
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

  //Copia as variáveis apnome e apsenha para uma sequência constante de char
  const char* ssid = apnome.c_str();
  const char* senha = apsenha.c_str();

  //Envia as informações para o ATMega
  Serial.print("T:");       Serial.println(tensao);
  Serial.print("CALIBRA:"); Serial.println(calibra);

  Serial.print("Conectando ao ponto: ");
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
  Serial.println("OK");
  Serial.print("IP:"); Serial.println(WiFi.localIP());
}

void loop() {

  if (Serial.available() > 0) {
    if (Serial.find("IP?")) {
      delay(50);
      if (WiFi.status() != WL_CONNECTED) {
        Serial.print("IP:");
        Serial.println("192.168.4.1");
      } else  {
        Serial.print("IP:"); Serial.println(WiFi.localIP()); 
      }
    }
  }

  //Verifica se algum dispositivo (cliente) se conectou
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  //Espera o cliente enviar alguns dados
  Serial.println("Novo cliente");
  while (!client.available()) {
    delay(1);
  }

  //Lê a primeira linha do cabeçalho.
  //Para ler as outras, use esse fullReq aqui.
  String req = client.readStringUntil('\r');
  //String fullReq = client.readString();
  Serial.println(req);
  client.flush();

  //Executa um determinado código para cada tipo de requisição
  if (req.indexOf("GET / HTTP/") != -1) {
    File index = SPIFFS.open("/interface.html", "r");
    String arq = index.readString();
    index.close();
    pagina = headerHTML + arq;
    client.flush();
  }
  
  if (req.indexOf("GET /indice HTTP/") != -1) {
    client.flush();
    pagina = "<a href=\"/dados\">Ir para a página dos dados</a>, <a href=\"/configurar\">configurar</a> ou <a href=\"/teste\">ver o que tá acontecendo com o arquivo da configuração</a>";
  }

  if (req.indexOf("GET /info HTTP/") != -1) {
    client.flush();
    pagina = "Conectado ao ponto";
    pagina += ssid;
    pagina += ". Acesse <a href=\"http://"
    pagina += WiFi.localIP();
    pagina += "> a interface</a> para visualizar os dados."
  }

  if (req.indexOf("/dados") != -1) {
    client.flush();
    Serial.println("DADOS?");
    delay(10);
    //Recebe os dados do ATMega
    while (!Serial.available()) {
      delay(1);
    }
    if (Serial.find("DADOS:")) {
      dados = Serial.readStringUntil('\r');
      delay(20);
    }
    pagina = headerJSON + dados;
  }

  if (req.indexOf("GET /configurar HTTP/") != -1) {
    File index = SPIFFS.open("/ajustes.html", "r");
    String arq = index.readString();
    index.close();
    pagina = headerHTML + arq;
    client.flush();
  }

  if (req.indexOf("GET /teste") != -1) {
    client.flush();
    File f = SPIFFS.open("/config.txt", "r");
    pagina = f.readString();
  }

  if (req.indexOf("GET /salvar") != -1) {
    //if (fullReq.indexOf("?") != -1)
    client.flush();

    //Lê os dados enviados na página de configuração
    tensao  = req.substring(req.indexOf("tensao=")   + 7, a = req.indexOf('&'));
    calibra = req.substring(req.indexOf("calibra=")  + 8, b = req.indexOf('&', a + 1));
    apnome  = req.substring(req.indexOf("AP_nome=")  + 8, req.lastIndexOf('&'));      //req.indexOf('&',b + 1) se for pra incluir mais
    apsenha = req.substring(req.indexOf("AP_senha=") + 9, req.indexOf(" HTTP/"));     //Última parte
    
    //Salva os novos dados no config.txt
    File f = SPIFFS.open("/config.txt", "w");
    f.print("tensao=");   f.println(tensao);
    f.print("calibra=");  f.println(calibra);
    f.print("AP_nome=");  f.println(apnome);
    f.print("AP_senha="); f.println(apsenha);
    f.close();
    Serial.println("Dados salvos. Reiniciando...");
    client.print("OK");
    delay(1000);
    ESP.restart();
  }
  client.print(pagina);
  client.setTimeout(1000);
  delay(2);
  Serial.println("Cliente desconectado");
}
