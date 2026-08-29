#ifndef CONTROL_PRESION_H
#define CONTROL_PRESION_H

#include <Arduino.h>

void iniciarSensorPresion();
float leerPresionKPa();       // Retorna el valor instantáneo (ruidoso)
float leerPresionFiltrada();  // <--- NUEVO: Retorna el valor suavizado para el PI
bool evaluarEquilibrio(float presionActual);

#endif