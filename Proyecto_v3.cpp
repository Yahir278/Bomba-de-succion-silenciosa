#include "ControlDimmer.h"
#include "ControlPantalla.h"
#include "ControlValvulas.h"
#include "ControlPresion.h" 
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include "PaginaWeb.h"         
#include "recepcion.h"
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> // <--- LIBRERÍA NECESARIA PARA TELEGRAM   

const int PIN_BOTON = 13; 
const int PIN_LED_EMERGENCIA = 27; 
const int PIN_VALVULA_SUCCION = 26;
const int PIN_BUZZER = 35;       

int simulacionBPM = 0;           
int ultimoValorECG = 0;        

enum EstadoSistema {
  INICIO,
  ESPERANDO_MODO,
  MOSTRANDO_INFO_AUTO, // (Mantenido por compatibilidad de memoria)
  ESPERANDO_BOTON_INICIO,
  EJECUTANDO_CALOR,
  ESPERANDO_BOTON_SUCCION,  
  FINALIZADO,
  ESTADO_EMERGENCIA  
};
EstadoSistema estadoActual = INICIO;

AsyncWebServer servidor(80);
AsyncWebSocket ws("/ws");

String comandoWebRecibido = "";
int potenciaWebRecibida = 0;
int tiempoWebRecibido = 0; 
bool hayNuevoComandoWeb = false;

bool flagWebStart = false; 
bool flagWebNext = false;       
bool flagWebStop = false;
bool flagToggleSuccion = false;

// ==========================================
// CONFIGURACIÓN DE TELEGRAM (OFICIAL Y ESTABLE)
// ==========================================
String botToken = "8794500922:AAH60Ya8LA2tnxGhmAe4NS77dKoBLV5wG8o"; 
String chatId = "1524562775";
bool alertaEnviada = false;

// =======================================================
// VARIABLES DE CONTROL PI (LAZO CERRADO)
// =======================================================
int setpointAuto = 0;            // Meta en mmHg (60, 80, 90)
float kp = 0.7;                  // Ganancia Proporcional (Ajustable)
float ki = 0.03;                 // Ganancia Integral (Ajustable)
float errorIntegral = 0.0;
unsigned long ultimoTiempoPI = 0;

int pasoTeclado = 0; 
int tempPotenciaTeclado = 0;

unsigned long tiempoTotalCicloMilis = 0; 
int tiempoSeleccionadoMinutos = 0; 
int potenciaSeleccionada = 0; 
bool esModoAutomatico = false; 

// Relojes del Tren de Pulsos (El "Pulmón")
const unsigned long TIEMPO_PULSO_ON_MS    = 3500;   
const unsigned long TIEMPO_PULSO_OFF_MS   = 2000;   

unsigned long tiempoInicioCicloGlobal = 0; 
unsigned long tiempoUltimoPulso = 0;       
bool estadoPulsoTriac = false;             

unsigned long tiempoUltimoEnvioWeb = 0; 
bool ultimoEstadoBoton = HIGH;
unsigned long tiempoUltimoRebote = 0;

void eventoWebSocket(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      String mensaje = (char*)data;
      
      // NUEVO PARSER: Atrapa el Setpoint Automático (Ej. AUTO:80)
      if (mensaje.startsWith("AUTO:")) {
        setpointAuto = mensaje.substring(5).toInt();
        comandoWebRecibido = "automatico";
        hayNuevoComandoWeb = true;
      } 
      else if (mensaje.startsWith("MANUAL:")) {
        String dataPart = mensaje.substring(7);
        int commaIndex = dataPart.indexOf(',');
        
        if (commaIndex != -1) {
            potenciaWebRecibida = dataPart.substring(0, commaIndex).toInt();
            tiempoWebRecibido = dataPart.substring(commaIndex + 1).toInt();
            comandoWebRecibido = "manual";
            hayNuevoComandoWeb = true;
        }
      }
      else if (mensaje == "START") { flagWebStart = true; }
      else if (mensaje == "NEXT")  { flagWebNext = true;  } 
      else if (mensaje == "STOP")  { flagWebStop = true;  }
      else if (mensaje == "TOGGLE_SUCCION") { flagToggleSuccion = true; }
    }
  }
}

void enviarAlertaTelegram(String tipoAlerta) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Evita errores de SSL para un envío ultra rápido
    HTTPClient http;

    // Armamos el mensaje con formato Markdown para que salga en negrita
    String mensaje = "%E2%9A%A8+*CODIGO+AZUL*+%E2%9A%A8%0APaciente+en+peligro.+Alerta:+*" + tipoAlerta + "*";
    
    // URL oficial de la API de Telegram
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + mensaje + "&parse_mode=Markdown";
    
    http.begin(client, url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("Alerta Telegram enviada con éxito. Código: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error enviando Telegram. Código: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error: WiFi no conectado.");
  }
}

void setup() {
  Serial.begin(115200);
  initRecepcion(); 
  
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_LED_EMERGENCIA, OUTPUT);      
  digitalWrite(PIN_LED_EMERGENCIA, LOW);    
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  iniciarValvulas(); 
  iniciarDimmer();
  iniciarSensorPresion(); 
  iniciarPantalla(); 
  
  mostrarConectandoWiFi(); 

  WiFiManager wm;
  bool conexionExitosa = wm.autoConnect("BOMBA_VACIO_ESPOL", "Admin1234"); 
  if (!conexionExitosa) { ESP.restart(); }

  ws.onEvent(eventoWebSocket);
  servidor.addHandler(&ws);
  servidor.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });
  servidor.begin();

  if (MDNS.begin("bombadevacio")) { Serial.println("mDNS: bombadevacio.local"); }
  
  estadoActual = ESPERANDO_MODO;
  mostrarEsperandoModo();
}

void loop() {
  ws.cleanupClients(); 
  actualizarRecepcion();
  
  int datoMonitor = obtenerDatoMonitor();
  if (datoMonitor != -9999) { ultimoValorECG = datoMonitor; }

  // ========================================================================
  // LÓGICA DEL TECLADO MATRICIAL (NUEVAS BIFURCACIONES)
  // ========================================================================
  String tecla = obtenerDatoTeclado();
  if (tecla != "" && estadoActual != ESTADO_EMERGENCIA) {
    
    // TRUCO ECG (Solo si estamos en el menú principal)
    if (pasoTeclado == 0) {
      if (tecla == "7") { simulacionBPM = 45; }
      else if (tecla == "9") { simulacionBPM = 135; }
      else if (tecla == "8") { simulacionBPM = 0; }
    }

    if (estadoActual == ESPERANDO_MODO) {
      
      if (pasoTeclado == 0) {
        if (tecla == "B") { // AUTO
          pasoTeclado = 3;
          mostrarEsperandoSetpoint(); 
        } 
        else if (tecla == "A") { // MANUAL
          pasoTeclado = 1; 
          mostrarEsperandoPotencia(); 
        }
      } 
      else if (pasoTeclado == 1) { // Manual -> Potencia
        if (tecla.toInt() > 0 || tecla == "0") { 
          tempPotenciaTeclado = tecla.toInt();
          pasoTeclado = 2;
          mostrarEsperandoTiempo(tempPotenciaTeclado); 
        }
      }
      else if (pasoTeclado == 2) { // Manual -> Tiempo
        if (tecla.toInt() > 0 || tecla == "0") {
          potenciaWebRecibida = tempPotenciaTeclado;
          tiempoWebRecibido = tecla.toInt();
          comandoWebRecibido = "manual";
          hayNuevoComandoWeb = true;
          pasoTeclado = 0; 
        }
      }
      else if (pasoTeclado == 3) { // Auto -> Setpoint
        if (tecla == "1") { setpointAuto = 60; }
        else if (tecla == "2") { setpointAuto = 80; }
        else if (tecla == "3") { setpointAuto = 90; }
        
        if (setpointAuto > 0 && (tecla == "1" || tecla == "2" || tecla == "3")) {
          comandoWebRecibido = "automatico";
          hayNuevoComandoWeb = true;
          pasoTeclado = 0;
        }
      }
    }
  }

  // ========================================================================
  // EVALUADOR DE ALARMAS MÉDICAS 
  // ========================================================================
int bpmA_Mostrar = ultimoValorECG;
  if (simulacionBPM > 0) bpmA_Mostrar = simulacionBPM + random(-3, 4); 

  String estadoECG = "Normal";
  if (bpmA_Mostrar > 0 && bpmA_Mostrar < 60) estadoECG = "Bradicardia";
  else if (bpmA_Mostrar > 100) estadoECG = "Taquicardia";

  // === NUEVA LÓGICA DE ALERTAS Y BUZZER ===
  if (estadoECG == "Taquicardia" || estadoECG == "Bradicardia") {
    
    // Dispara el Buzzer de la máquina local
    if (estadoECG == "Taquicardia") {
        if ((millis() % 400) < 200) digitalWrite(PIN_BUZZER, HIGH); 
        else digitalWrite(PIN_BUZZER, LOW);
    } else {
        if ((millis() % 1500) < 500) digitalWrite(PIN_BUZZER, HIGH); 
        else digitalWrite(PIN_BUZZER, LOW);
    }

    // Dispara el WhatsApp al médico (Solo 1 vez por evento)
    // Dispara el mensaje al enfermero/médico (Solo 1 vez por evento)
    if (!alertaEnviada) {
      Serial.println(">>> ACTIVANDO PROTOCOLO CÓDIGO AZUL (Telegram) <<<");
      enviarAlertaTelegram(estadoECG);
      alertaEnviada = true; 
    }
    
  } else {
    // Si el paciente vuelve a la normalidad
    digitalWrite(PIN_BUZZER, LOW); 
    alertaEnviada = false; // Reinicia la bandera para futuras emergencias
  }

  if (flagToggleSuccion) {
    flagToggleSuccion = false;
    if (estadoActual != EJECUTANDO_CALOR && estadoActual != ESTADO_EMERGENCIA) {
      digitalWrite(PIN_VALVULA_SUCCION, !digitalRead(PIN_VALVULA_SUCCION));
    }
  }

  bool triggerStart = flagWebStart || (tecla == "C");
  bool triggerNext  = flagWebNext  || (tecla == "D" || tecla == "*");
  flagWebStart = false;
  flagWebNext = false;

  bool lecturaFisicaBoton = digitalRead(PIN_BOTON);
  if (lecturaFisicaBoton != ultimoEstadoBoton) { tiempoUltimoRebote = millis(); }

  bool triggerBotonEmergencia = false;
  if ((millis() - tiempoUltimoRebote) > 50 && lecturaFisicaBoton == LOW) {
    triggerBotonEmergencia = true;
  }

  if (triggerBotonEmergencia || flagWebStop) {
    flagWebStop = false; 

    if (estadoActual == ESTADO_EMERGENCIA) {
      estadoActual = ESPERANDO_MODO;
      digitalWrite(PIN_LED_EMERGENCIA, LOW); 
      pasoTeclado = 0;
      liberarSuccion(); 
      mostrarEsperandoModo();
    } else {
      estadoActual = ESTADO_EMERGENCIA;
      actualizarPotencia(0);
      bloqueoEmergencia(); 
      digitalWrite(PIN_LED_EMERGENCIA, HIGH); 
      mostrarAlarmaEmergencia();
    }
    while(digitalRead(PIN_BOTON) == LOW) { delay(10); } 
  }

  // ========================================================================
  // TRANSMISOR DE TELEMETRÍA 
  // ========================================================================
  if (millis() - tiempoUltimoEnvioWeb >= 500) {
    tiempoUltimoEnvioWeb = millis();
    
    if (ws.count() > 0) { 
      float presionActualWeb = leerPresionKPa();
      String textoEstado = "";
      int tiempoRestanteSegundos = 0; 
      
      int maquinaOcupada = (estadoActual == ESPERANDO_MODO) ? 0 : 1;
      if (estadoActual == ESTADO_EMERGENCIA) maquinaOcupada = 1; 

      int valvulaSuccionAbierta = (digitalRead(PIN_VALVULA_SUCCION) == LOW) ? 1 : 0;
      int btnStartEna = 0; int btnNextEna = 0; int togglesEna = 0;

      switch(estadoActual) {
        case ESPERANDO_MODO:            
          textoEstado = "Listo -> Configure Parámetros"; togglesEna = 1; break;
        case ESPERANDO_BOTON_INICIO:    
          textoEstado = "Presione INICIAR CALOR"; btnStartEna = 1; togglesEna = 1; break;
        case EJECUTANDO_CALOR:          
          if (esModoAutomatico) {
            textoEstado = "Control PI Continuo";
            tiempoRestanteSegundos = 1; // Ficticio para mantener HMI activo
          } else {
            textoEstado = "Ciclo Térmico Activo"; 
            tiempoRestanteSegundos = (tiempoTotalCicloMilis - (millis() - tiempoInicioCicloGlobal)) / 1000;
            if (tiempoRestanteSegundos < 0) tiempoRestanteSegundos = 0;
          }
          break;
        case ESPERANDO_BOTON_SUCCION:   
          textoEstado = "Calor Fin. Opere Válvulas -> AVANZAR"; btnNextEna = 1; togglesEna = 1; break;
        case FINALIZADO:                
          textoEstado = "Terminado -> Presione REINICIAR"; btnNextEna = 1; togglesEna = 1; break;
        case ESTADO_EMERGENCIA:
          textoEstado = "¡BLOQUEO DE EMERGENCIA ACTIVO!"; break;
        default:                        
          textoEstado = "Iniciando..."; break;
      }

      int tiempoTotalSegundos = tiempoSeleccionadoMinutos * 60;

      String paqueteJSON = "{\"presion\":\"" + String(presionActualWeb, 1) + "\", " +
                           "\"estado\":\"" + textoEstado + "\", " +
                           "\"vSuccion\":" + String(valvulaSuccionAbierta) + ", " +
                           "\"isBusy\":" + String(maquinaOcupada) + ", " +
                           "\"tiempo\":" + String(tiempoRestanteSegundos) + ", " +
                           "\"tTotal\":" + String(tiempoTotalSegundos) + ", " +
                           "\"potencia\":" + String(potenciaSeleccionada) + ", " + 
                           "\"setpoint\":" + String(setpointAuto) + ", " + 
                           "\"enabStart\":" + String(btnStartEna) + ", " +
                           "\"enabNext\":" + String(btnNextEna) + ", " +
                           "\"enabToggles\":" + String(togglesEna) + ", " +
                           "\"ecg\":" + String(bpmA_Mostrar) + ", " +
                           "\"alertaECG\":\"" + estadoECG + "\"}";
                           
      ws.textAll(paqueteJSON);
    }
  }

  // ========================================================================
  // MÁQUINA DE ESTADOS Y LAZO CERRADO (PI)
  // ========================================================================
  switch (estadoActual) {
    
    case ESPERANDO_MODO:
      if (hayNuevoComandoWeb) {
        hayNuevoComandoWeb = false;
        if (comandoWebRecibido == "automatico") {
          esModoAutomatico = true;
          estadoActual = ESPERANDO_BOTON_INICIO;
          mostrarConfiguracionAuto(setpointAuto);
        } 
        else if (comandoWebRecibido == "manual") {
          potenciaSeleccionada = potenciaWebRecibida;
          tiempoSeleccionadoMinutos = tiempoWebRecibido;
          esModoAutomatico = false;
          estadoActual = ESPERANDO_BOTON_INICIO;
          mostrarEsperandoBoton(potenciaSeleccionada, tiempoSeleccionadoMinutos);
        }
      }
      break;

    case ESPERANDO_BOTON_INICIO:
      if (triggerStart) { 
        tiempoTotalCicloMilis = tiempoSeleccionadoMinutos * 60000; 
        liberarSuccion(); 
        
        reiniciarDisparos();
        tiempoInicioCicloGlobal = millis();
        tiempoUltimoPulso = millis();
        estadoPulsoTriac = true;

        if (esModoAutomatico) {
          errorIntegral = 0;
          ultimoTiempoPI = millis();
          potenciaSeleccionada = 0; // El PI la subirá desde cero
        } else {
          actualizarPotencia(potenciaSeleccionada);
        }
        
        estadoActual = EJECUTANDO_CALOR;
        mostrarEjecutando(potenciaSeleccionada);
      }
      break;

    case EJECUTANDO_CALOR:
    
      // 1. CÁLCULO DEL CONTROLADOR PI (Solo en Modo Auto)
      if (esModoAutomatico) {
        unsigned long tiempoActual = millis();
        float dt = (tiempoActual - ultimoTiempoPI) / 1000.0;
        
        if (dt >= 0.5) { // Ejecuta la ecuación cada 500ms
          float presionPV = leerPresionFiltrada();
          float error = setpointAuto - presionPV; // Error positivo = falta vacío
          
          errorIntegral += (error * dt);
          
          // Anti-Windup (Evita que la memoria integral sature el sistema)
          if (errorIntegral > 500) errorIntegral = 500; 
          if (errorIntegral < -500) errorIntegral = -500;
          
          float salidaPI = (kp * error) + (ki * errorIntegral);
          
          int potenciaCalculada = (int)salidaPI;
          // Mapeo seguro a los límites físicos del Triac
          if (potenciaCalculada > 100) potenciaCalculada = 100;
          if (potenciaCalculada < 0) potenciaCalculada = 0;
          
          potenciaSeleccionada = potenciaCalculada; 
          ultimoTiempoPI = tiempoActual;
        }

        // Condición de Salida del Bucle Infinito
        if (triggerNext) {
          actualizarPotencia(0);
          estadoActual = FINALIZADO;
          mostrarFinalizado(obtenerDisparos());
        }
      } 
      // 2. CONDICIÓN DE SALIDA MODO MANUAL
      else {
        if (millis() - tiempoInicioCicloGlobal >= tiempoTotalCicloMilis) {
          actualizarPotencia(0); 
          liberarSuccion(); 
          delay(1000); 
          estadoActual = ESPERANDO_BOTON_SUCCION; 
          mostrarEsperandoSuccion();
        }
      }

      // 3. TREN DE PULSOS (El "Motor/Pulmón" de la máquina, actúa en ambos modos)
      if (estadoActual == EJECUTANDO_CALOR) {
        if (estadoPulsoTriac) {
          if (millis() - tiempoUltimoPulso >= TIEMPO_PULSO_ON_MS) {
            actualizarPotencia(0); 
            estadoPulsoTriac = false;
            tiempoUltimoPulso = millis();
          } else {
            // Permite al PI actualizar la intensidad en pleno ciclo de calentamiento
            if (esModoAutomatico) actualizarPotencia(potenciaSeleccionada);
          }
        } else {
          if (millis() - tiempoUltimoPulso >= TIEMPO_PULSO_OFF_MS) {
            actualizarPotencia(potenciaSeleccionada);
            estadoPulsoTriac = true;
            tiempoUltimoPulso = millis();
          }
        }
      }
      break;

    case ESPERANDO_BOTON_SUCCION: 
      if (triggerNext) { 
        estadoActual = FINALIZADO;
        mostrarFinalizado(obtenerDisparos());
      }
      break;

    case FINALIZADO:
      if (triggerNext) { 
        estadoActual = ESPERANDO_MODO;
        mostrarEsperandoModo();
      }
      break;

    case ESTADO_EMERGENCIA:
      break;
  }
  ultimoEstadoBoton = lecturaFisicaBoton;
}