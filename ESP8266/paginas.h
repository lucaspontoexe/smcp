#ifndef _PAGINAS_H
#define _PAGINAS_H

//Realiza algumas funções para cada página
void handleRoot() {
  File file = SPIFFS.open("/interface.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void carregarIndice() {
  pagina = "Conectado ao ponto ";
  pagina += apnome;
  pagina += ". Acesse <a href=\"http://";
  pagina += WiFi.localIP();
  pagina += "> a interface</a> para visualizar os dados.";
  server.send(200, "text/html", pagina);
}
void configurar() {
  File file = SPIFFS.open("/ajustes.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}
void exportudomesmo() {
  File file = SPIFFS.open("/config.txt", "r");
  server.streamFile(file, "text/plain");
  file.close();
}
void EasterEgg() {
  server.send(200, "text/html", "<html><h2 align=\"center\">OLHA PRO DISPLAY</h2></html>");
  Serial.print("EASTEREGG");
}
void QuatroZeroQuatro() {
  server.send(404,"text/plain","Erro 404. Tem coisa errada aí.");
}
#endif
