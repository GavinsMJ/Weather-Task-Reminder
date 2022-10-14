//GM
void init_anime() {

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "LOADING ..");
  display.display();
  delay(100);
  display.clear();
  display.drawString(0, 0, "LOADING ...");
  display.display();
  delay(50);
  display.clear();
  display.drawString(0, 0, "LOADING ....");
  display.display();
  delay(50);
  display.clear();
  display.drawString(0, 0, "LOADING .......");
  display.display();
}

void init_load() {
  display.clear();
  display.drawString(0, 0, "LOADING ...........");
  display.display();
  delay(100);
  display.drawString(0, 10, "TIME AND DATE");
  display.display();
  delay(30);
  display.drawString(0, 20, "TEMPERATURE");
  display.display();
  delay(100);
  display.drawString(0, 30, "HUMIDITY");
  display.display();
  delay(100);
  display.drawString(0, 40, "FORECAST");
  display.display();
  delay(100);
  display.drawString(0, 50, "INITIALIZING GUI");
  display.display();
  delay(300);
  display.drawString(0, 50, "INITIALIZING GUI ~~ GM");
  display.display();
  delay(200);
}
