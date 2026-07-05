# Sistema Bomba de succión silenciosa

Este repositorio contiene el firmware y el diseño del sistema de control embebido desarrollado en la plataforma ESP32 para la automatización de una termoformadora por vacío.

## Autores
* **Wilmer Adrian Barriga Loor** - Ingeniería en Electrónica y Automatización Industrial.
* **Ana Belen Durango Buenaño** - Ingeniería en Electrónica y Automatización Industrial.

## Características Principales
- **Control Termodinámico:** Modulación de potencia de niquelina mediante disparo de fase con Triac.
- **Procesamiento Digital de Señales (DSP):** Monitoreo neumático asíncrono con sensor de presión MPS20N0040D y ADC de 24 bits.
- **Seguridad Industrial:** Paro de emergencia administrado por interrupción de hardware externa y alarmas fail-safe con buzzer activo.

## Arquitectura de Software
El firmware está diseñado bajo una arquitectura de Máquina de Estados Finitos (FSM) no-bloqueante para garantizar la estabilidad del lazo de potencia AC.
