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

Esta tabla resume tus justificaciones cualitativas con datos duros para el documento:

| Componente Evaluado | Alternativa 1 (Seleccionada) | Alternativa 2 (Descartada) | Costo Est. | Precisión / Resolución | Latencia / Vida Útil |
|---|---|---|---|---|---|
| Procesamiento | ESP32 | ATmega328P | ~$4.00 vs $2.00 | Dual-Core 240MHz vs Single 16MHz | Asíncrono / Evita bloqueos |
| Conversor ADC | HX710B (Externo) | ADC Interno ESP32 | ~$1.50 vs $0.00 | 24 bits (Lineal) vs 12 bits (No lineal) | Alta inmunidad EMI |
| Conmutación Válvulas | MOSFET | Relé Mecánico | ~$0.80 vs $1.20 | Estado Sólido | <1 ms / Conmutación infinita |
| Control Térmico | Triac (Ángulo de Fase) | Relé (ON/OFF) | ~$1.00 vs $1.20 | Modulación lineal | Previene choque térmico |


## Criterios de Prueba Cuantitativos
El documento menciona el rango de presión y tiempos de alarma, pero falta la métrica de éxito.

| Prueba (Test) | Valor Esperado (Target) | Margen de Tolerancia | Criterio de Aceptación (Pass/Fail) |
|---|---|---|---|
| Regulación de Vacío | 105 mmHg (Centro de banda) | Rango: 90 a 120 mmHg | Pass: La presión se mantiene en el rango por >15 min continuos. |
| Respuesta de Alarma | Activación < 5 segundos | + 0.5 segundos | Pass: Buzzer y LED encendidos en <5s tras desconectar el sensor. |
| Paro de Emergencia | 0V en calentador Triac | 0 milisegundos | Pass: Corte absoluto de AC y válvulas cerradas instantáneamente. |
