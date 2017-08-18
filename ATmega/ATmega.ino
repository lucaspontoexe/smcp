#include "EmonLib.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const byte    numChars        = 48;
char          receivedChars[numChars];  // Array para manter os caracteres de serial
boolean       newData         = false;
unsigned long previousMillis  = 0;      // Salva a última vez que os dados foram atualizados
unsigned long previousMillis1 = 0;		  // Salva a última vez que o display mostrou o IP
const long    interval        = 200;    // Intervalo entre medições
String        stringSerial;
boolean       ocupado         = true;

const int     pino_sct        = 0;
const int     botao           = 11;
int           estadoBotaoIP;
int           tensao          = 220;
float         calibra         = 18.0;
String        ip = "(conectando)";
String        dados;
String        t, cal;

//CARACTERE 'ã', por pura fresc-PARA MELHOR EXIBIÇÃO, isso.
byte atilmin[8] = {
  0b00101,
  0b01010,
  0b01110,
  0b00001,
  0b01111,
  0b10001,
  0b01111,
  0b00000
};

EnergyMonitor emon1;
SoftwareSerial esp8266(9, 10);
LiquidCrystal lcd(4, 3, 5, 6, 7, 8);

//Detecta se tem informações novas vindo do serial (ESP8266)
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
    stringSerial = receivedChars;
    newData = false;
  }
}


void setup() {
  pinMode (botao, INPUT);

  //Manda o LCD carregar o caractere com acento
  lcd.createChar(0, atilmin);

  //Inicia o display
  lcd.begin(16, 2);
  lcd.setCursor(6, 0); lcd.print("SMCP");
  lcd.setCursor(2, 1); lcd.print("Vers"); lcd.write((uint8_t)0); lcd.print("o 0.7.1");

  //Inicia o serial e o sensor de corrente
  emon1.current(pino_sct, calibra);
  esp8266.begin(9600);
  esp8266.println("Ligado");
  ocupado = false;
}

void loop() {
  //Registra há quanto tempo o dispositivo está ligado.
  //Útil para fazer interrupções sem usar delay();
  unsigned long currentMillis = millis();

  //Calcula a corrente e a potência
  double Irms = emon1.calcIrms(1480);
  double potencia = Irms * tensao;

  //Manda os dados pro ESP.
  //Tudo que começar com "DADOS:" é enviado direto pro navegador
  dados = "DADOS:{ \"corrente\": ";
  dados += Irms;
  dados += ", \"potencia\": ";
  dados += potencia;
  dados += " }";
  esp8266.println(dados);

  //Funções que recebem dados do serial
  recvWithEndMarker();
  showNewData();

  //Assim que o comando "T:" for recebido, a variável "tensao" é ajustada para o número que vier em seguida.
  //Usado para calcular a potência aparente. Eu sei que aparece um 'W' na tela, acontece.
  if (stringSerial.indexOf("T:") != -1) {
    t = stringSerial.substring(stringSerial.indexOf("T:") + 2, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"tensao\": ");
    esp8266.print(t);
    esp8266.println("}");

    tensao = t.toInt();
  }
  //A mesma coisa acontece com a calibração. Não há muita documentação sobre isto, infelizmente.
  if (stringSerial.indexOf("CALIBRA:") != -1) {
    cal = stringSerial.substring(stringSerial.indexOf("CALIBRA:") + 8, stringSerial.indexOf('\r'));
    esp8266.println();
    esp8266.print("DADOS: {\"calibra\": ");
    esp8266.print(calibra);
    esp8266.println("}");
    calibra = cal.toFloat();
  }

  //Verifica se o botão do IP está apertado
  estadoBotaoIP = digitalRead(botao);
  if (estadoBotaoIP == HIGH) {
    lcd.clear();
    
    //Pergunta no serial qual é o IP
    esp8266.println("IP?");
    
    //Tempo para esperar a informação chegar
    delay(150);
    recvWithEndMarker();
    showNewData();
    if (stringSerial.indexOf("IP:") != -1) {
      ip = stringSerial.substring(stringSerial.indexOf("IP:") + 3, stringSerial.indexOf('\r'));
      ocupado = true;
      lcd.clear(); //Apaga a tela assim que achar o IP.
    }
    
    unsigned long previousMillis1 = millis();
    unsigned long currentMillis1 = millis();
    
    //Mostra o IP na tela enquanto continua medindo e enviando dados, por três segundos
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
      recvWithEndMarker();
      showNewData();
    }
  }

  ocupado = false;
  stringSerial = "";

  if (currentMillis - previousMillis >= interval) {
    //Marca tempo para verificar se já se passou o tempo para uma nova medição
    previousMillis = currentMillis;

    if (ocupado == false) {
      //Atualiza o display com informações novas
      lcd.setCursor(0, 0);  lcd.print("Corr.(A): ");
      lcd.setCursor(0, 1);  lcd.print("Pot. (W): ");
      lcd.setCursor(10, 0); lcd.print(Irms);
      lcd.setCursor(10, 1); lcd.print(potencia, 1);
    }
  }
}
