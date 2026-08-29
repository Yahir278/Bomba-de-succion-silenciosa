#ifndef CONTROL_PANTALLA_H
#define CONTROL_PANTALLA_H
#include <Arduino.h>

void iniciarPantalla();
void mostrarInicio();
void mostrarConectandoWiFi();
void mostrarEsperandoModo();
void mostrarEsperandoPotencia();
void mostrarEsperandoTiempo(int pot); 
void mostrarEsperandoSetpoint(); // <--- NUEVA: Pide la meta (60, 80, 90)
void mostrarConfiguracionAuto(int setpoint); // <--- NUEVA: Confirma la meta elegida
void mostrarEsperandoBoton(int pot, int tiempo); 
void mostrarEjecutando(int pot);
void mostrarEsperandoSuccion();
void mostrarEsperandoApagar();
void mostrarFinalizado(int disparos);
void mostrarPresionEnVivo(float presion, bool enEquilibrio);
void mostrarAlarmaEmergencia();

#endif