#include <Arduino.h>
#include "ControlPantalla.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void iniciarPantalla() {
  Wire.begin(21, 19);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error OLED"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void mostrarInicio() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println(F("BOMBA DE"));
  display.setCursor(30, 30);
  display.println(F("VACIO"));
  display.setTextSize(1);
  display.setCursor(35, 50);
  display.println(F("Iniciando..."));
  display.display();
}

void mostrarEsperandoModo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.println(F("SISTEMA OK"));
  display.setCursor(0, 25);
  display.println(F("Elija Modo en Web"));
  display.setCursor(0, 40);
  display.println(F("o Teclado (A / B)"));
  display.display();
}

void mostrarEsperandoPotencia() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("MODO MANUAL (1/2)"));
  display.setCursor(0, 25);
  display.println(F("1. Ingrese Potencia%"));
  display.setCursor(0, 45);
  display.println(F("y presione [#]"));
  display.display();
}

void mostrarEsperandoTiempo(int pot) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("MODO MANUAL (2/2)"));
  display.setCursor(0, 15);
  display.print(F("Potencia fijada: "));
  display.print(pot);
  display.println(F("%"));
  display.setCursor(0, 35);
  display.println(F("2. Ingrese Tiempo"));
  display.setCursor(0, 45);
  display.println(F("en Minutos + [#]"));
  display.display();
}

// ==========================================
// NUEVAS PANTALLAS MODO AUTOMÁTICO PI
// ==========================================
void mostrarEsperandoSetpoint() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("MODO AUTO CONTINUO"));
  display.setCursor(0, 15);
  display.println(F("Elija el Setpoint:"));
  display.setCursor(0, 30);
  display.println(F("1) 60 mmHg"));
  display.setCursor(0, 42);
  display.println(F("2) 80 mmHg"));
  display.setCursor(0, 54);
  display.println(F("3) 90 mmHg"));
  display.display();
}

void mostrarConfiguracionAuto(int setpoint) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("AUTO CONFIGURADO"));
  display.setCursor(0, 25);
  display.print(F("Meta: "));
  display.print(setpoint);
  display.println(F(" mmHg"));
  display.setCursor(0, 45);
  display.println(F("Presione Tecla [C]"));
  display.display();
}
// ==========================================

void mostrarEsperandoBoton(int pot, int tiempo) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("CONFIGURACION LISTA"));
  display.setCursor(0, 20);
  display.print(F("Pot: "));
  display.print(pot);
  display.print(F("% | T: "));
  display.print(tiempo);
  display.println(F(" min"));
  display.setCursor(0, 45);
  display.println(F("Presione Tecla [C]"));
  display.display();
}

void mostrarEjecutando(int pot) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.println(F("CALENTANDO"));
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(F("Salida AC: "));
  display.print(pot);
  display.println(F("% Pot."));
  display.display();
}

void mostrarEsperandoSuccion() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("CALOR TERMINADO"));
  display.setCursor(0, 20);
  display.println(F("Opere valvulas Web"));
  display.setCursor(0, 40);
  display.println(F("Presione [D] para"));
  display.setCursor(0, 50);
  display.println(F("FINALIZAR/SELLAR"));
  display.display();
}

void mostrarEsperandoApagar() {
  display.clearDisplay();
  display.display();
}

void mostrarFinalizado(int disparos) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("CICLO FIN"));
  display.setTextSize(1);
  display.setCursor(0, 28);
  display.println(F("Para reiniciar:"));
  display.setCursor(0, 45);
  display.println(F("Presione Tecla [D]"));
  display.display();
}

void mostrarPresionEnVivo(float presion, bool enEquilibrio) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CAMARA DE VACIO:");
  display.setTextSize(2);
  display.setCursor(10, 22);
  display.print(presion, 1); 
  display.setTextSize(1);
  display.print(" mmHg");
  display.setCursor(0, 50);
  if (enEquilibrio) {
    display.print("Estado: [ ESTABLE ]");
  } else {
    display.print("Estado: [ Succionando ]");
  }
  display.display();
}

void mostrarAlarmaEmergencia() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("BLOQUEO!");
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println("PARO DE EMERGENCIA");
  display.setCursor(0, 45);
  display.println("Presione boton(13)");
  display.setCursor(0, 55);
  display.println("fisico para salir");
  display.display();
}

void mostrarConectandoWiFi() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println(F("Conectando WiFi..."));
  display.setCursor(0, 30);
  display.println(F("Red: BOMBA_VACIO"));
  display.display();
}