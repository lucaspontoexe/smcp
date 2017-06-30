//NOTA: TROCAR O SERIAL CASO FOR TESTAR
//substituir o "esquema"
#include "EmonLib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>



const byte    numChars        = 48;
char          receivedChars[numChars];  // an array to store the received data
boolean       newData         = false;
unsigned long previousMillis  = 0;      // will store last time LED was updated
unsigned long previousMillis1 = 0;
const long    interval        = 200;    // interval at which to blink (milliseconds)
String        esquema;
boolean       ocupado         = true;

const int     pino_sct        = 0;
const int     botao           = 11;
int           estadoBotaoIP;
int           tensao          = 220;
int           calibra         = 18;
String        ip = "(conectando)";
String        dados;
String        t, cal;

#include "display_chars.h"

EnergyMonitor emon1;
SoftwareSerial esp8266(9, 10);
LiquidCrystal lcd(4, 3, 5, 6, 7, 8);

#include "easteregg.h"

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
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    //esp8266.print("This just in ... ");
    //esp8266.println(receivedChars);
    esquema = receivedChars;
    newData = false;
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
  lcd.setCursor(3, 1); lcd.print("Vers"); lcd.write((uint8_t)0); lcd.print("o 0.7");
  emon1.current(pino_sct, calibra);
  esp8266.begin(9600);
  esp8266.println("Tá ligado, tá ligado?");
  ocupado = false;
}

void loop() {
  unsigned long currentMillis = millis();
  double Irms = emon1.calcIrms(1480);
  double potencia = Irms * tensao;

  dados = "DADOS:{ \"corrente\": ";
  dados += Irms;
  dados += ", \"potencia\":";
  dados += potencia;
  dados += " }";
  esp8266.println(dados);

  recvWithEndMarker();
  showNewData();
  if (esquema.indexOf("T:") != -1) {
    t = esquema.substring(esquema.indexOf("T:") + 2, esquema.indexOf('\r'));
    esp8266.print("Tensão ajustada para ");
    esp8266.println(t);
    tensao = t.toInt();
  }
  if (esquema.indexOf("CALIBRA:") != -1) {
    cal = esquema.substring(esquema.indexOf("CALIBRA:") + 8, esquema.indexOf('\r'));
    esp8266.print("Nível de calibração ajustado para: ");
    esp8266.println(calibra);
    calibra = cal.toInt();
  }

  if (esquema.indexOf("EASTEREGG") != -1) {
    EASTEREGG();
  }

  estadoBotaoIP = digitalRead(botao);
  if (estadoBotaoIP == HIGH) {
    lcd.clear();
    esp8266.println("IP?");
    delay(150);
    recvWithEndMarker();
    showNewData();
    if (esquema.indexOf("IP:") != -1) {
      ip = esquema.substring(esquema.indexOf("IP:") + 3, esquema.indexOf('\r'));
      ocupado = true;
      lcd.clear(); //Apaga a tela assim que achar o IP.
    }
    unsigned long previousMillis1 = millis();
    unsigned long currentMillis1 = millis();
    while (currentMillis1 - previousMillis1 <= 3000) {
      currentMillis1 = millis();
      lcd.setCursor(0, 0); lcd.print("IP:          "); //Não repita tamanha gambiarra em casa.
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
      recvWithEndMarker();
      showNewData();
    }
  }
  //ip = "(desconectado)";
  ocupado = false;
  esquema = "";

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (ocupado == false) {
      lcd.setCursor(0, 0);  lcd.print("Corr.(A): ");
      lcd.setCursor(0, 1);  lcd.print("Pot. (W): ");
      lcd.setCursor(10, 0); lcd.print(Irms);
      lcd.setCursor(10, 1); lcd.print(potencia, 1);
    }
  }
}
