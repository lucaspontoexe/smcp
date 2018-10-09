#include "EmonLib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define  pino_sct 0
#define  botao 11

String        inputString = "";         // Armazena os dados que estão chegando
boolean       stringComplete  = false;  // se a string está completa
String        stringSerial;

int           estadoBotaoIP;
int           tensao          = 220;
float         calibra         = 18.0;
String        ip = "(conectando)";
String        dados;

double Irms, potencia;

EnergyMonitor emon1;
SoftwareSerial esp8266(9, 10);
LiquidCrystal lcd(4, 3, 5, 6, 7, 8);

#include "display_chars.h"

typedef void (*func_t)(); //ponteiro de função sem argumentos, do tipo void (mas dá pra colocar argumentos também, é só atualizar lá no ponteiro que funciona também)
func_t fptr;              //uma variável que armazena o ponteiro (é tipo OOP) (aqui func_t é basicamente um ponteiro de função)

void setup() {
  pinMode (botao, INPUT);

  lcd.createChar(0, atilmin);
  lcd.createChar(1, atil);
  lcd.createChar(2, aagudo);
  lcd.createChar(3, eagudo);
  lcd.createChar(4, iagudo);

  lcd.begin(16, 2);
  lcd.setCursor(6, 0); lcd.print("SMCP");
  lcd.setCursor(2, 1); lcd.print("Vers"); lcd.write((uint8_t)0); lcd.print("o 0.8");

  emon1.current(pino_sct, calibra);

  inputString.reserve(256);
  esp8266.begin(9600);
  esp8266.println("Ligado");
}

void loop() {
  serialEvent();
  if (stringSerial.indexOf("T:") != -1) {
    String t = stringSerial.substring(stringSerial.indexOf("T:") + 2, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"tensao\": ");
    esp8266.print(t);
    esp8266.println("}");
    tensao = t.toInt();
  }

  if (stringSerial.indexOf("CALIBRA:") != -1) {
    String cal = stringSerial.substring(stringSerial.indexOf("CALIBRA:") + 8, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"calibra\": ");
    esp8266.print(calibra);
    esp8266.println("}");
    calibra = cal.toFloat();
  }


  estadoBotaoIP = digitalRead(botao);
  if (estadoBotaoIP == HIGH) {
    lcd.clear();
    esp8266.println("IP?");
    delay(150);
    serialEvent();
    if (stringSerial.indexOf("IP:") != -1) {
      ip = stringSerial.substring(stringSerial.indexOf("IP:") + 3, stringSerial.indexOf('\r'));
      lcd.clear(); //Apaga a tela assim que achar o IP.
      lcd.setCursor(0, 0); lcd.print("IP:          ");
      lcd.setCursor(0, 1); lcd.print(ip);
      keepRunning(medir, 3000);
    }
  }
  //ip = "(desconectado)";
  stringSerial = "";
  lcd.setCursor(0, 0);  lcd.print("Corr.(A): ");
  lcd.setCursor(0, 1);  lcd.print("Pot. (W): ");
  lcd.setCursor(10, 0); lcd.print(Irms);
  lcd.setCursor(10, 1); lcd.print(potencia, 1);
  keepRunning(medir, 100);
}


void medir() {
  Irms = emon1.calcIrms(1480);
  potencia = Irms * tensao;
  dados = "DADOS:{ \"corrente\": ";
  dados += Irms;
  dados += ", \"potencia\": ";
  dados += potencia;
  dados += " }";
  esp8266.println(dados);
  serialEvent();
}



//A função que define quem vai ficar rodando pelo tempo que for necessário
void keepRunning(func_t callback, long timer) {

  //Armazena a função que foi passada como argumento no ponteiro
  fptr = callback;

  //Anota os tempos
  unsigned long rightnow, pasttime;
  pasttime = millis();
  rightnow = millis();

  while (rightnow - pasttime <= timer) {
    //Enquanto a diferença de tempo for menor que a quantidade pedida, rodar a função callback repetidamente.
    //Atualiza "que horas são"
    rightnow = millis();
    fptr(); //keep dem runnin'
  }
}

void serialEvent() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    //Dica: return inputString;
    stringSerial = inputString;
    // Limpa a string temporária
    inputString = "";
    stringComplete = false;
  }
}

