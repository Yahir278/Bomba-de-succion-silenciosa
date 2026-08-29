#include "ControlValvulas.h"

// La válvula de calor fue eliminada físicamente del sistema
const int PIN_VALVULA_SUCCION = 26;

// =========================================================================
// LÓGICA REDISEÑADA: 1 SOLA VÁLVULA NC + RELÉ ACTIVE-LOW
// =========================================================================
// Relé Active-LOW: Recibir LOW lo enciende, recibir HIGH lo apaga.
// Válvula NC: Necesita estar ENCENDIDA para mantenerse abierta y succionar.

#define ABRIR_SUCCION  LOW   // 0V al relé -> Relé ENCIENDE -> 12V a Válvula -> SE ABRE (Flujo Libre)
#define CERRAR_SUCCION HIGH  // 3.3V al relé -> Relé APAGA -> 0V a Válvula -> SE CIERRA (Bloqueo)

void iniciarValvulas() {
  // Según la nueva lógica, la succión siempre está abierta por defecto
  digitalWrite(PIN_VALVULA_SUCCION, ABRIR_SUCCION);
  pinMode(PIN_VALVULA_SUCCION, OUTPUT);
}

void liberarSuccion() {
  digitalWrite(PIN_VALVULA_SUCCION, ABRIR_SUCCION);
}

void bloqueoEmergencia() {
  digitalWrite(PIN_VALVULA_SUCCION, CERRAR_SUCCION);
}