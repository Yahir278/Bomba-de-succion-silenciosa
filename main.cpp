/*Código para recibir datos del heart monitor y enviarlos,
Código para recibir input del teclado matriz y enviarlos*/
#include <Arduino.h>
#include "matrix.h"
#include "monitor.h"

unsigned long tiempoAnterior = 0;
const int intervaloMuestreo = 10; // 10 ms = 100 Hz de muestreo para el ECG

void setup() {
  Serial.begin(115200); 
  initTeclado();
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void loop() {
  char tecla = ingresoTeclado();
  if (tecla != '\0') {
    // Si se presionó una tecla, la enviamos con el prefijo K:
    Serial.print("K:");
    Serial.println(tecla);
  }

  // 2. Lectura del monitor cardíaco (no bloqueante)
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervaloMuestreo) {
    tiempoAnterior = tiempoActual;
    
    // Obtenemos el valor del sensor
    int valorMonitor = monitorOut(); 
    
    // Lo enviamos con el prefijo M:
    Serial.print("M:");
    Serial.println(valorMonitor);
  }
}