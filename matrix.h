/*Header para el funcionamiento del teclado*/
#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>

const byte countRows = 4;
const byte countColumns = 4;

const byte rowsPins[countRows] = { 11, 10, 9, 8 };
const byte columnsPins[countColumns] = { 7, 6, 5, 4 };

extern const char keys[countRows][countColumns];

// Inicializa los pines del teclado (debes llamarla en el setup de tu .ino)
void initTeclado();

// Escanea el teclado y devuelve el caracter presionado (o '\0' si no hay nada)
char ingresoTeclado();

#endif