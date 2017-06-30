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
    lcd.setCursor(0, 0); lcd.print("TALI");
    b(1, 0, 0);
    lcd.setCursor(5, 0); lcd.print("TALI");
    b(1, 0, 0);
    lcd.setCursor(5, 1); lcd.print("T"); lcd.write((uint8_t)2); lcd.print(" LIGADO");
    b(2, 0, 0);
    lcd.clear();
    lcd.setCursor(1, 0); lcd.print("QUE N"); lcd.write((uint8_t)1); lcd.print("O "); lcd.write((uint8_t)3); lcd.print(" PRA");
    lcd.setCursor(2, 1); lcd.print("QUALQUER UM");
    b(4, 0, 0);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("SMCP ATUALIZADO");
    lcd.setCursor(3, 1); lcd.write((uint8_t)3); lcd.print(" O 0.7.1");
    b(8, 0, 0);
    lcd.clear();
  }
}

void cemporcento() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("100% ATUALIZADO");
  b(4, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.write((uint8_t)3); lcd.print(" RUIM DE ATURAR");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SMCP VIROU MODA");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("TODO MUNDO");
  lcd.setCursor(3, 1); lcd.print("QUER USAR");
  b(4, 1, 0);

  lcd.clear();
  lcd.setCursor(5, 0); lcd.print("COM A");
  lcd.setCursor(2, 1); lcd.print("NOSSA EQUIPE");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("NINGU"); lcd.write((uint8_t)3); lcd.print("M BATE");
  lcd.setCursor(3, 1); lcd.print("DE FRENTE");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SMCP "); lcd.write((uint8_t)3); lcd.print(" NERVOSO");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("N"); lcd.write((uint8_t)1); lcd.print("O D"); lcd.write((uint8_t)2); lcd.print(" CHANCE");
  lcd.setCursor(1, 1); lcd.print("AO CONCORRENTE");
  b(4, 1, 0);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SE LIGA A"); lcd.write((uint8_t)4); lcd.print(" MANO");
  b(3, 1, 0);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("ESCUTE O QUE EU");
  lcd.setCursor(3, 1); lcd.print("VOU DIZER");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("QUEM FECHA");
  lcd.setCursor(4, 1); lcd.print("COM SMCP");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("SEMPRE VAI");
  lcd.setCursor(3, 1); lcd.print("FORTALECER");
  b(4, 0, 0);
  lcd.clear();

  lcd.setCursor(2, 0); lcd.print("PORQUE TUDO");
  lcd.setCursor(2, 1); lcd.print("QUE "); lcd.write((uint8_t)3); lcd.print(" BOM");
  b(5, 0, 0);
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("A GENTE REPETE");
  b(3, 0, 0);
  lcd.clear();
  lcd.setCursor(3, 0); lcd.write((uint8_t)3); lcd.print(" POR ISSO");
  lcd.setCursor(2, 1); lcd.print("QUE EU FECHO");
  b(4, 0, 0);
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("COM A EQUIPE");
  lcd.setCursor(6, 1); lcd.print("SMCP");
  b(4, 0, 0);
  lcd.clear();
}

void EASTEREGG() {
  ocupado = true;
  lcd.clear();
  //Só pra garantir
  esp8266.println("DADOS:{\"running\": true}");
  delay(5);
  esp8266.println("DADOS:{\"running\": true}");
  delay(5);
  esp8266.println("DADOS:{\"running\": true}");
  
  b(1, 0, 0);
  tali();
  cemporcento();
  tali();
  b(3, 0, 0);
  lcd.setCursor(4, 0); lcd.print("-SAMPLES-");
  lcd.setCursor(0, 1); lcd.print("INCOMPREENS"); lcd.write((uint8_t)4); lcd.print("VEIS");
  b(24, 0, 0);
  lcd.clear();
  ocupado = false;
}
#endif
