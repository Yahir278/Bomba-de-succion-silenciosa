#include <Arduino.h>
#include "ControlDimmer.h"

const int ZC_PIN = 22;      
const int TRIAC_PIN = 18;

volatile int potencia = 0;
volatile uint64_t retardo_us = 8200; 
volatile unsigned long last_zc_time = 0; 
volatile int contadorDisparos = 0; 

hw_timer_t * timer_disparo = NULL;

void IRAM_ATTR timer_isr() {
  digitalWrite(TRIAC_PIN, LOW); 
}

void IRAM_ATTR zc_isr() {
  unsigned long ahora = esp_timer_get_time();
  
  if ((ahora - last_zc_time) > 2000) {
    last_zc_time = ahora;

    digitalWrite(TRIAC_PIN, HIGH);
    timerStop(timer_disparo);

    if (potencia > 0 && potencia < 100) {
      timerWrite(timer_disparo, 0); 
      
      // --- SINTAXIS CLÁSICA CORE 2.0 ---
      // 1. Escribimos el valor de la alarma en microsegundos
      timerAlarmWrite(timer_disparo, retardo_us, false); 
      // 2. Habilitamos explícitamente el disparo de la alarma
      timerAlarmEnable(timer_disparo); 
      
      timerStart(timer_disparo);
      contadorDisparos++; 
    } else if (potencia == 100) {
      digitalWrite(TRIAC_PIN, LOW); 
      contadorDisparos++;
    }
  }
}

void iniciarDimmer() {
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN, HIGH); 
  pinMode(ZC_PIN, INPUT_PULLUP);

  // --- SINTAXIS CLÁSICA CORE 2.0 ---
  // timerBegin(Timer_ID [0 a 3], Prescaler [80], CountUp [true])
  // El reloj del ESP32 va a 80MHz. Al dividirlo para 80, obtenemos exactamente 1MHz (1 tick = 1 microsegundo).
  timer_disparo = timerBegin(0, 80, true); 

  // timerAttachInterrupt(Timer, Función_ISR, Flanco [true = Rising])
  timerAttachInterrupt(timer_disparo, &timer_isr, true);

  attachInterrupt(digitalPinToInterrupt(ZC_PIN), zc_isr, RISING);
}

void actualizarPotencia(int nuevaPotencia) {
  if (nuevaPotencia >= 0 && nuevaPotencia <= 100) {
    potencia = nuevaPotencia;
    if (potencia > 0 && potencia < 100) {
      retardo_us = map(potencia, 1, 99, 8200, 200);
    }
  }
}

int obtenerDisparos() {
  return contadorDisparos;
}

void reiniciarDisparos() {
  contadorDisparos = 0;
}