#ifndef _EASTEREGG_H
#define _EASTEREGG_H

void b(int ba, int bb, int bc) {
  float tempo =  ba * 461.5;
  tempo += bb * 230.75;
  tempo += bc * 115.375;
  delay(tempo);
}

void tali() {
  for (int i = 0; i < 2; i++) {
    lcd.clear();
    lcd.setCursor(0, 0); escreverAcentos("TALI");
    b(1, 0, 0);
    lcd.setCursor(5, 0); escreverAcentos("TALI");
    b(1, 0, 0);
    lcd.setCursor(5, 1); escreverAcentos("TÁ LIGADO");
    b(2, 0, 0);
    lcd.clear();
    lcd.setCursor(1, 0); escreverAcentos("QUE NÃO É PRA");
    lcd.setCursor(2, 1); escreverAcentos("QUALQUER UM");
    b(4, 0, 0);
    lcd.clear();
    lcd.setCursor(0, 0); escreverAcentos("SMCP ATUALIZADO");
    lcd.setCursor(3, 1); escreverAcentos("É O 0.7.1");
    b(8, 0, 0);
  }
}

void cemporcento() {
  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("100% ATUALIZADO");
  b(4, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("É RUIM DE ATURAR");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("SMCP VIROU MODA");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); escreverAcentos("TODO MUNDO");
  lcd.setCursor(3, 1); escreverAcentos("QUER USAR");
  b(4, 1, 0);

  lcd.clear();
  lcd.setCursor(5, 0); escreverAcentos("COM A");
  lcd.setCursor(2, 1); escreverAcentos("NOSSA EQUIPE");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(2, 0); escreverAcentos("NINGUÉM BATE");
  lcd.setCursor(3, 1); escreverAcentos("DE FRENTE");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("SMCP É NERVOSO");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(1, 0); escreverAcentos("NÃO DÁ CHANCE");
  lcd.setCursor(1, 1); escreverAcentos("AO CONCORRENTE");
  b(4, 1, 0);

  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("SE LIGA AÍ MANO");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); escreverAcentos("ESCUTE O QUE EU");
  lcd.setCursor(3, 1); escreverAcentos("VOU DIZER");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); escreverAcentos("QUEM FECHA");
  lcd.setCursor(4, 1); escreverAcentos("COM SMCP");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); escreverAcentos("SEMPRE VAI");
  lcd.setCursor(3, 1); escreverAcentos("FORTALECER");
  b(4, 0, 0);
  lcd.clear();

  lcd.setCursor(2, 0); escreverAcentos("PORQUE TUDO");
  lcd.setCursor(2, 1); escreverAcentos("QUE É BOM");
  b(5, 0, 0);
  lcd.clear();
  lcd.setCursor(1, 0); escreverAcentos("A GENTE REPETE");
  b(3, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); escreverAcentos("É POR ISSO");
  lcd.setCursor(2, 1); escreverAcentos("QUE EU FECHO");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(2, 0); escreverAcentos("COM A EQUIPE");
  lcd.setCursor(6, 1); escreverAcentos("SMCP");
  b(4, 0, 0);
}

void EASTEREGG() {
  ocupado = true;
  lcd.clear();

  //Só pra garantir
  esp8266.println();
  esp8266.println("DADOS:{\"running\": true}");
  delay(5);
  esp8266.println("DADOS:{\"running\": true}");
  delay(5);
  esp8266.println("DADOS:{\"running\": true}");

  b(1, 0, 0);
  tali();
  cemporcento();
  tali();
  //b(3, 0, 0);
  int q = 0;
  for (q = 0; q < 3; q++) {
    lcd.print("1");
    b(1, 0, 0);
  }
  lcd.clear();
  lcd.setCursor(4, 0); escreverAcentos("-SAMPLES-");
  lcd.setCursor(0, 1); escreverAcentos("INCOMPREENSÍVEIS");
  b(24, 0, 0);
  lcd.clear();
  ocupado = false;
}
#endif
