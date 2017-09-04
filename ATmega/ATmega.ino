#include "EmonLib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const byte    numChars        = 48;
char          receivedChars[numChars];  // Array para manter os caracteres de serial
boolean       newData         = false;
unsigned long previousMillis  = 0;      // Salva a última vez que os dados foram atualizados
unsigned long previousMillis1 = 0;		  // Salva a última vez que o display mostrou o IP
const long    interval        = 200;    // Intervalo entre medições
boolean       ocupado         = true;

String        inputString = "";         // Armazena os dados que estão chegando
boolean       stringComplete  = false;  // se a string está completa
String        stringSerial;

const int     pino_sct        = 0;
const int     botao           = 11;
int           estadoBotaoIP;
int           tensao          = 220;
float         calibra         = 18.0;
String        ip = "(conectando)";
String        dados;
String        t, cal;

EnergyMonitor emon1;
SoftwareSerial esp8266(9, 10);
LiquidCrystal lcd(4, 3, 5, 6, 7, 8);

#include "display_chars.h"
#include "easteregg.h"

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

    stringSerial = inputString;
    // Limpa a string temporária
    inputString = "";
    stringComplete = false;
  }
}

void setup() {
  pinMode (botao, INPUT);
  lcd.createChar(0, atilmin);
  lcd.createChar(1, atil);
  lcd.createChar(2, aagudo);
  lcd.createChar(3, eagudo);
  lcd.createChar(4, iagudo);
  lcd.begin(16, 2);
  lcd.setCursor(6, 0); lcd.print("SMCP");
  //Corrigir o problema
  lcd.setCursor(2, 1); lcd.print("Vers"); lcd.write((uint8_t)0); lcd.print("o 0.7.1");

  emon1.current(pino_sct, calibra);

  inputString.reserve(256);
  esp8266.begin(9600);
  esp8266.println("Ligado");
  ocupado = false;
}

void loop() {
  unsigned long currentMillis = millis();
  double Irms = emon1.calcIrms(1480);
  double potencia = Irms * tensao;

  dados = "DADOS:{ \"corrente\": ";
  dados += Irms;
  dados += ", \"potencia\": ";
  dados += potencia;
  dados += " }";
  esp8266.println(dados);

  serialEvent();

  if (stringSerial.indexOf("T:") != -1) {
    t = stringSerial.substring(stringSerial.indexOf("T:") + 2, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"tensao\": ");
    esp8266.print(t);
    esp8266.println("}");

    tensao = t.toInt();
  }
  if (stringSerial.indexOf("CALIBRA:") != -1) {
    cal = stringSerial.substring(stringSerial.indexOf("CALIBRA:") + 8, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"calibra\": ");
    esp8266.print(calibra);
    esp8266.println("}");
    calibra = cal.toFloat();
  }

  if (stringSerial.indexOf("EASTEREGG") != -1) {
    EASTEREGG();
  }

  estadoBotaoIP = digitalRead(botao);
  if (estadoBotaoIP == HIGH) {
    lcd.clear();
    esp8266.println("IP?");
    delay(150);
    serialEvent();
    if (stringSerial.indexOf("IP:") != -1) {
      ip = stringSerial.substring(stringSerial.indexOf("IP:") + 3, stringSerial.indexOf('\r'));
      ocupado = true;
      lcd.clear(); //Apaga a tela assim que achar o IP.
    }
    unsigned long previousMillis1 = millis();
    unsigned long currentMillis1 = millis();
    while (currentMillis1 - previousMillis1 <= 3000) {
      currentMillis1 = millis();
      lcd.setCursor(0, 0); lcd.print("IP:          ");
      lcd.setCursor(0, 1); lcd.print(ip);

      unsigned long currentMillis = millis();
      double Irms = emon1.calcIrms(1480);
      double potencia = Irms * tensao;

      dados = "DADOS:{ \"corrente\": ";
      dados += Irms;
      dados += ", \"potencia\":";
      dados += potencia;
      dados += " }";
      esp8266.println(dados);
      serialEvent();
    }
  }
  //ip = "(desconectado)";
  ocupado = false;
  stringSerial = "";

  if (currentMillis - previousMillis >= interval) {
    //Marca tempo para verificar se já se passou o tempo para uma nova medição
    previousMillis = currentMillis;

    if (ocupado == false) {
      lcd.setCursor(0, 0);  lcd.print("Corr.(A): ");
      lcd.setCursor(0, 1);  lcd.print("Pot. (W): ");
      lcd.setCursor(10, 0); lcd.print(Irms);
      lcd.setCursor(10, 1); lcd.print(potencia, 1);
    }
  }
}
