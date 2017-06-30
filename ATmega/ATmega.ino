

#include "EmonLib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const byte    numChars        = 48;     // O programa só recebe 48 caracteres de cada vez
char          receivedChars[numChars];  // Array pra guardar os dados enviados por serial
boolean       newData         = false;  // Muda quando há dados novos pelo serial
unsigned long previousMillis  = 0;      // Salva a última vez em que o display foi atualizado
unsigned long previousMillis1 = 0;      // Usado para contar o tempo em que o IP fica no display
const long    interval        = 200;    // Intervalo de atualização do display
String        comandos;                 // Mesma coisa que o array receivedChars, porém em formato de string.
boolean       ocupado         = true;   // Usada em outra versão, pode ser excluída.

const int     pino_sct        = 0;
const int     botao           = 11;
int           estadoBotaoIP;
int           tensao          = 220;
int           calibra         = 18;
String        ip = "(conectando)";
String        dados;
String        t, cal;

EnergyMonitor emon1;
SoftwareSerial esp8266(9, 10);
LiquidCrystal lcd(4, 3, 5, 6, 7, 8);

//#include "easteregg.h"

//Comunicação serial
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (esp8266.available() > 0 && newData == false) {
    rc = esp8266.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // Termina a string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    comandos = receivedChars;
    newData = false;
  }
}

//Configurações
void setup() {
  pinMode (botao, INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); lcd.print("SMCP      0.5.1");
  lcd.setCursor(0, 1); lcd.print("Iniciando... ");
  emon1.current(pino_sct, calibra);
  esp8266.begin(9600);
  esp8266.println("Ligado");
  ocupado = false;
}

//Loop
void loop() {
  unsigned long currentMillis = millis();
  double Irms = emon1.calcIrms(1480);
  double potencia = Irms * tensao;
  recvWithEndMarker();
  showNewData();

  if (comandos.indexOf("DADOS?") != -1) {
    dados = "DADOS:{ \"corrente\": ";
    dados += Irms;
    dados += ", \"potencia\":";
    dados += potencia;
    dados += " }";
    esp8266.println(dados);
  }
  if (comandos.indexOf("T:") != -1) {
    t = comandos.substring(comandos.indexOf("T:") + 2, comandos.indexOf('\r'));
    esp8266.print("Tensão ajustada para ");
    esp8266.println(t);
    tensao = t.toInt();
  }
  if (comandos.indexOf("CALIBRA:") != -1) {
    cal = comandos.substring(comandos.indexOf("CALIBRA:") + 8, comandos.indexOf('\r'));
    esp8266.print("Nível de calibração ajustado para: ");
    esp8266.println(calibra);
    calibra = cal.toInt();
  }

  
  estadoBotaoIP = digitalRead(botao);
  if (estadoBotaoIP == HIGH) {
    lcd.clear();
    esp8266.println("IP?");
    delay(150);
    recvWithEndMarker();
    showNewData();
    if (comandos.indexOf("IP:") != -1) {
      ip = comandos.substring(comandos.indexOf("IP:") + 3, comandos.indexOf('\r'));
      ocupado = true;
      lcd.clear(); //Apaga a tela assim que achar o IP.
    }
    unsigned long previousMillis1 = millis();
    unsigned long currentMillis1 = millis();
    while (currentMillis1 - previousMillis1 <= 3000) {
      currentMillis1 = millis();
      lcd.setCursor(0, 0); lcd.print("IP:          "); 
      lcd.setCursor(0, 1); lcd.print(ip);
      recvWithEndMarker();
      showNewData();
    }
  }
  ip = "(desconectado)";
  ocupado = false;
  comandos = "";

  if (currentMillis - previousMillis >= interval) {
    // Salva a última vez em que o display foi atualizado
    previousMillis = currentMillis;

    if (ocupado == false) {
      lcd.setCursor(0, 0);  lcd.print("Corr.(A): ");
      lcd.setCursor(0, 1);  lcd.print("Pot. (W): ");
      lcd.setCursor(10, 0); lcd.print(Irms);
      lcd.setCursor(10, 1); lcd.print(potencia, 1);
    }
  }
}
