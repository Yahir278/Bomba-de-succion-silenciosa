#include <Arduino.h>
#include "ControlPresion.h"

// Bornera del Módulo MPS20N0040D + HX710B
const int PIN_DOUT = 34; 
const int PIN_SCK  = 32; 

long offsetCero = 0;             
float factorEscala = 45000.0;    

// ========================================================
// CONFIGURACIÓN DE DIRECCIÓN FÍSICA
// Si al contraerse el aire (vacío) el HMI muestra números 
// negativos, cambia este valor a 'true' para invertirlo.
// ========================================================
bool invertirSensor = true; 

float ultimaPresion = 0.0;
int contadorEstabilidad = 0;

// Variables del Filtro Pasa Bajas (EMA)
float presionFiltrada = 0.0;
const float alphaFiltro = 0.15; 
bool primerEscaneo = true;

long leerHX710B_Crudo() {
  unsigned long tiempoInicio = millis();
  
  while (digitalRead(PIN_DOUT) == HIGH) {
    if (millis() - tiempoInicio > 150) { return offsetCero; }
  }

  long conteo = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(PIN_SCK, HIGH);
    delayMicroseconds(1);
    conteo = conteo << 1;
    if (digitalRead(PIN_DOUT) == HIGH) { conteo++; }
    digitalWrite(PIN_SCK, LOW);
    delayMicroseconds(1);
  }

  digitalWrite(PIN_SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_SCK, LOW);

  if (conteo & 0x800000) { conteo |= 0xFF000000; }
  return conteo;
}

void iniciarSensorPresion() {
  pinMode(PIN_SCK, OUTPUT);
  pinMode(PIN_DOUT, INPUT);
  digitalWrite(PIN_SCK, LOW);

  Serial.println("\n[Sensor Vacío] Calibrando el Cero Atmosférico...");
  delay(1000);

  long suma = 0;
  for (int i = 0; i < 10; i++) {
    suma += leerHX710B_Crudo();
    delay(80);
  }
  offsetCero = suma / 10;
  primerEscaneo = true; 

  Serial.print("[Sensor Vacío] Tara de reposo fijada en el valor crudo: ");
  Serial.println(offsetCero);
}

// Devuelve el valor instantáneo crudo
float leerPresionKPa() {
  long lecturaInstante = leerHX710B_Crudo();
  
  // Diferencia cruda respetando el signo algebraico (+ o -)
  long delta = lecturaInstante - offsetCero;

  // Ya no usamos abs(). Convertimos directamente.
  float presionKPa = (float)delta / factorEscala;
  float presion_mmHg = presionKPa * 7.50062;

  // Inversión lógica para que el VACÍO sea positivo en tu HMI
  if (invertirSensor) {
    presion_mmHg = presion_mmHg * -1.0;
  }

  return presion_mmHg; 
}

// Devuelve el valor purificado y constante para el PI
float leerPresionFiltrada() {
  float lecturaActual = leerPresionKPa();
  
  if (primerEscaneo) {
      presionFiltrada = lecturaActual;
      primerEscaneo = false;
  } else {
      presionFiltrada = (alphaFiltro * lecturaActual) + ((1.0 - alphaFiltro) * presionFiltrada);
  }
  
  return presionFiltrada;
}

bool evaluarEquilibrio(float presionActual) {
  float diferencia = abs(presionActual - ultimaPresion);
  if (diferencia <= 2.2) { contadorEstabilidad++; } 
  else { contadorEstabilidad = 0; }
  ultimaPresion = presionActual;
  return (contadorEstabilidad >= 4); 
}