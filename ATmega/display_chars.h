void escreverAcentos(String texto) {
  for (int i = 0; i < texto.length(); i++) {
    if (texto[i] == 'ã') lcd.write((uint8_t)0);
    else if (texto[i] == 'Ã') lcd.write((uint8_t)1);
    else if (texto[i] == 'Á') lcd.write((uint8_t)2);
    else if (texto[i] == 'É') lcd.write((uint8_t)3);
    else if (texto[i] == 'Í') lcd.write((uint8_t)4);
    else lcd.write(texto[i]);
  }
}

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

byte aagudo[8] = {
  0b00010,
  0b00100,
  0b01110,
  0b10001,
  0b11111,
  0b10001,
  0b10001,
  0b00000
};

byte eagudo[8] = {
  0b00010,
  0b00100,
  0b11111,
  0b10000,
  0b11110,
  0b10000,
  0b11111,
  0b00000
};

byte atil[8] = {
  0b00101,
  0b01010,
  0b01110,
  0b10001,
  0b11111,
  0b10001,
  0b10001,
  0b00000
};

byte iagudo[8] = {
  0b00010,
  0b00100,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b01110,
  0b00000
};

