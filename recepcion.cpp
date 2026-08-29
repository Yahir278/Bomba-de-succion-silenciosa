#include "recepcion.h"
#include <Arduino.h>

String bufferNumeros = "";
String datoTecladoListo = "";

// Variables para el procesamiento del ECG
int bpmCalculado = 0;
bool monitorActualizado = false;
unsigned long tiempoUltimoLatido = 0;
bool picoDetectado = false;

// UMBRAL: Deberás ajustar este valor (ej. 600, 700 o 800) dependiendo 
// de qué tan altos sean los números crudos cuando el corazón da el "pico"
const int UMBRAL_PICO = 650; 

void initRecepcion() {
    Serial2.begin(9600); 
    Serial2.setTimeout(10); 
}

void actualizarRecepcion() {
    while (Serial2.available() > 0) {
        String linea = Serial2.readStringUntil('\n');
        linea.trim(); 

        // PROCESAMIENTO DEL SENSOR ECG
        if (linea.startsWith("M:")) {
            int valorCrudo = linea.substring(2).toInt(); //[cite: 4]
            
            if (valorCrudo == -1) {
                // Electrodos desconectados[cite: 2]
                bpmCalculado = 0; 
                monitorActualizado = true;
            } else {
                // Algoritmo de Detección de Picos
                if (valorCrudo > UMBRAL_PICO && !picoDetectado) {
                    unsigned long tiempoActual = millis();
                    unsigned long deltaT = tiempoActual - tiempoUltimoLatido;
                    
                    // Filtro anti-ruido: un latido humano rara vez supera los 220 BPM (delta > 270ms)
                    if (deltaT > 270) {
                        bpmCalculado = 60000 / deltaT;
                        tiempoUltimoLatido = tiempoActual;
                        monitorActualizado = true; // Avisamos que hay un nuevo BPM válido
                    }
                    picoDetectado = true; // Bloqueamos hasta que la onda baje
                } 
                // Histéresis: La onda debe bajar al menos 50 puntos para armar el próximo latido
                else if (valorCrudo < (UMBRAL_PICO - 50)) {
                    picoDetectado = false; 
                }
            }
        } 
        
        // PROCESAMIENTO DEL TECLADO
        else if (linea.startsWith("K:")) {
            char tecla = linea.charAt(2); //[cite: 4]
            
            if (isDigit(tecla)) {
                bufferNumeros += tecla; //[cite: 4]
            } 
            else if (tecla == '#') {
                if (bufferNumeros.length() > 0) {
                    datoTecladoListo = bufferNumeros; //[cite: 4]
                    bufferNumeros = "";  //[cite: 4]
                }
            } 
            else {
                datoTecladoListo = String(tecla); //[cite: 4]
                bufferNumeros = "";  //[cite: 4]
            }
        }
    }
}

int obtenerDatoMonitor() {
    if (monitorActualizado) {
        monitorActualizado = false; //[cite: 4]
        return bpmCalculado;
    }
    return -9999; //[cite: 4]
}

String obtenerDatoTeclado() {
    String resultado = datoTecladoListo; //[cite: 4]
    datoTecladoListo = ""; //[cite: 4]
    return resultado; //[cite: 4]
}