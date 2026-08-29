#ifndef CONTROL_VALVULAS_H
#define CONTROL_VALVULAS_H
#include <Arduino.h>

void iniciarValvulas();
void liberarSuccion();      // Mantiene la válvula abierta (Estado Normal)
void bloqueoEmergencia();   // Cierra la válvula de golpe (Estado de Emergencia)

#endif