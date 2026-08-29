#ifndef PAGINA_WEB_H
#define PAGINA_WEB_H
#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>HMI Bomba de vacío - ESPOL</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;800&display=swap');
    
    :root { 
      --bg-gradient: radial-gradient(circle at 80% -20%, #2b3a4a, #0f141a);
      --panel-bg: rgba(25, 33, 42, 0.5);
      --panel-border: rgba(255, 255, 255, 0.08);
      --accent-blue: #4da8da;
      --accent-green: #39ff14;
      --accent-red: #ff4655;
      --accent-yellow: #f1c40f;
      --text-main: #ffffff;
      --text-muted: #8b949e;
    }
    
    body { 
      font-family: 'Inter', system-ui, sans-serif; 
      background: var(--bg-gradient); 
      color: var(--text-main); 
      margin: 0; 
      padding: 30px 20px; 
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    /* ================= CABECERA ================= */
    .header-container {
      width: 100%;
      max-width: 1200px;
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      margin-bottom: 40px;
      background: rgba(255, 255, 255, 0.03);
      padding: 30px;
      border-radius: 20px;
      backdrop-filter: blur(10px);
      border: 1px solid var(--panel-border);
      box-shadow: 0 10px 30px rgba(0,0,0,0.3);
    }
    
    .title-area h1 { margin: 0; font-size: 2.2rem; font-weight: 800; letter-spacing: -0.5px; }
    .title-area h1 span { color: var(--accent-blue); }
    .title-area p { margin: 8px 0 0 0; color: var(--text-muted); font-size: 0.9rem; font-weight: 600; letter-spacing: 1px; text-transform: uppercase; }

    .status-area { display: flex; flex-direction: column; align-items: flex-end; }
    .time-display { font-size: 2rem; font-weight: 800; margin-bottom: 2px; }
    .date-display { font-size: 0.85rem; color: var(--text-muted); margin-bottom: 15px; }
    
    .status-badge { 
      display: flex; align-items: center; gap: 10px; background: rgba(0,0,0,0.4); 
      padding: 8px 16px; border-radius: 50px; font-size: 0.85rem; font-weight: 600; 
      border: 1px solid var(--panel-border); transition: 0.3s;
    }
    .status-badge.online { background: rgba(77, 168, 218, 0.15); border-color: var(--accent-blue); color: var(--accent-blue); }
    .status-dot { width: 10px; height: 10px; border-radius: 50%; background: #555; transition: 0.3s; }
    .status-badge.online .status-dot { background: var(--accent-blue); box-shadow: 0 0 10px var(--accent-blue); }

    /* ================= DASHBOARD ================= */
    .dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(340px, 1fr)); gap: 25px; width: 100%; max-width: 1200px; }
    
    .panel { 
      background: var(--panel-bg); 
      backdrop-filter: blur(15px); 
      border-radius: 20px; 
      padding: 30px; 
      border: 1px solid var(--panel-border);
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
    }
    
    .panel h2 { margin-top: 0; font-size: 1.1rem; color: var(--text-muted); font-weight: 600; letter-spacing: 0.5px; border-bottom: 1px solid rgba(255,255,255,0.05); padding-bottom: 15px; display: flex; justify-content: space-between;}
    .panel h2 span { font-size: 0.8rem; background: rgba(255,255,255,0.1); padding: 3px 8px; border-radius: 6px; color: #fff;}

    .telemetria-valor { display: block; text-align: center; font-family: 'Inter', sans-serif; font-size: 4rem; font-weight: 800; margin: 15px 0; color: #fff; }
    .unidad { font-size: 1.2rem; color: var(--text-muted); font-weight: 400; }
    canvas { background-color: rgba(0,0,0,0.2); width: 100%; height: 150px; border-radius: 12px; border: 1px solid rgba(255,255,255,0.05); margin-top: 10px; }
    .estado-maquina { background: rgba(0,0,0,0.3); padding: 15px; border-radius: 12px; text-align: center; font-weight: 600; margin-top: 20px; font-size: 1.1rem; color: var(--accent-blue); }

    /* ================= BOTONES Y CONTROLES ================= */
    button { 
      font-family: 'Inter', sans-serif; color: #fff; border: none; padding: 16px; font-size: 1rem; 
      font-weight: 600; border-radius: 12px; cursor: pointer; width: 100%; margin-top: 12px; 
      transition: all 0.2s ease; letter-spacing: 0.5px; 
    }
    button:disabled { background: rgba(255,255,255,0.05) !important; color: #555 !important; cursor: not-allowed; box-shadow: none !important; border: 1px solid transparent !important;}
    
    /* Layout para Setpoints Automáticos */
    .auto-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-top: 10px;}
    .btn-auto { background: rgba(255,255,255,0.1); border: 1px solid rgba(255,255,255,0.2); margin-top: 0; padding: 12px;}
    .btn-auto:hover:not(:disabled) { background: rgba(255,255,255,0.2); border-color: var(--accent-blue); color: var(--accent-blue);}
    
    /* === AÑADIDO: EFECTO LUMINOSO PARA EL BOTÓN ACTIVO === */
    .btn-auto-active { background: rgba(77, 168, 218, 0.2) !important; border-color: var(--accent-blue) !important; color: var(--accent-blue) !important; box-shadow: 0 0 15px rgba(77,168,218,0.3) !important;}

    .btn-manual { background: transparent; border: 1px solid var(--accent-red); color: var(--accent-red); }
    .btn-manual:hover:not(:disabled) { background: rgba(255, 70, 85, 0.1); }
    
    .btn-start { background: rgba(57, 255, 20, 0.15); color: var(--accent-green); border: 1px solid var(--accent-green); font-size: 1rem; font-weight: 800;}
    .btn-start:hover:not(:disabled) { background: var(--accent-green); color: #000; box-shadow: 0 5px 15px rgba(57, 255, 20, 0.3); }

    .btn-next { background: rgba(77, 168, 218, 0.15); color: var(--accent-blue); border: 1px solid var(--accent-blue); font-size: 1rem; font-weight: 800;}
    .btn-next:hover:not(:disabled) { background: var(--accent-blue); color: #000; box-shadow: 0 5px 15px rgba(77, 168, 218, 0.3); }
    
    .btn-stop { background: var(--accent-red); color: #fff;}
    .btn-stop:hover:not(:disabled) { transform: translateY(-2px); box-shadow: 0 8px 20px rgba(255, 70, 85, 0.4); }

    input[type=range] { -webkit-appearance: none; width: 100%; background: transparent; margin: 10px 0 20px 0; }
    input[type=range]::-webkit-slider-runnable-track { width: 100%; height: 8px; cursor: pointer; background: rgba(255,255,255,0.1); border-radius: 10px; }
    input[type=range]::-webkit-slider-thumb { height: 24px; width: 24px; border-radius: 50%; background: var(--text-main); cursor: pointer; -webkit-appearance: none; margin-top: -8px; box-shadow: 0 0 10px rgba(0,0,0,0.5); }
    input[type=range]:disabled { opacity: 0.3; cursor: not-allowed; }

    /* ================= TOGGLES MANUALES ================= */
    .hardware-status { display: flex; align-items: center; justify-content: space-between; background: rgba(0,0,0,0.2); padding: 15px 20px; margin-bottom: 15px; border-radius: 12px; font-weight: 600; color: var(--text-muted);}
    .hardware-status span { color: #fff; font-weight: 400; }
    
    .switch { position: relative; display: inline-block; width: 50px; height: 26px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: rgba(255,255,255,0.1); transition: .4s; border-radius: 34px; border: 1px solid rgba(255,255,255,0.2);}
    .slider:before { position: absolute; content: ""; height: 18px; width: 18px; left: 3px; bottom: 3px; background-color: #8b949e; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: rgba(57, 255, 20, 0.2); border-color: var(--accent-green);}
    input:checked + .slider:before { transform: translateX(24px); background-color: var(--accent-green); box-shadow: 0 0 10px var(--accent-green);}
    input:disabled + .slider { cursor: not-allowed; opacity: 0.4; }

    /* ================= ECG Y ALARMAS ================= */
    .ecg-box { margin-top: 30px; border: 1px solid var(--accent-blue); background: rgba(77, 168, 218, 0.05); transition: 0.3s;}
    .ecg-box .label { color: var(--accent-blue); font-weight: 800; font-size: 0.9rem; transition: 0.3s;}
    .ecg-value { font-family: 'Consolas', monospace; font-size: 1.8rem; font-weight: 800; color: var(--accent-blue); transition: 0.3s;}

    @keyframes latido-rapido { 0% { background: rgba(255, 70, 85, 0.05); } 50% { background: rgba(255, 70, 85, 0.4); box-shadow: 0 0 20px rgba(255, 70, 85, 0.3); } 100% { background: rgba(255, 70, 85, 0.05); } }
    @keyframes latido-lento { 0% { background: rgba(241, 196, 15, 0.05); } 50% { background: rgba(241, 196, 15, 0.4); box-shadow: 0 0 20px rgba(241, 196, 15, 0.3); } 100% { background: rgba(241, 196, 15, 0.05); } }
    
    .ecg-taquicardia { animation: latido-rapido 0.3s infinite; border-color: var(--accent-red) !important; }
    .ecg-taquicardia .label, .ecg-taquicardia .ecg-value { color: var(--accent-red) !important; }
    .ecg-bradicardia { animation: latido-lento 1.5s infinite; border-color: var(--accent-yellow) !important; }
    .ecg-bradicardia .label, .ecg-bradicardia .ecg-value { color: var(--accent-yellow) !important; }

    /* ================= CRONÓMETRO Y SPINNER ================= */
    #boxTimer { display: none; text-align: center; margin: 30px 0; }
    .timer-circle { transform: rotate(-90deg); transform-origin: 50% 50%; }
    .timer-bg { fill: none; stroke: rgba(255,255,255,0.05); stroke-width: 6; }
    .timer-prog { fill: none; stroke: var(--accent-red); stroke-width: 6; stroke-dasharray: 314; stroke-dashoffset: 0; transition: stroke-dashoffset 0.5s linear; stroke-linecap: round; }
    .timer-text { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); font-size: 1.8rem; font-weight: 800; color: #fff; }
    
    .spin-anim { animation: spinCircle 1.5s linear infinite; transform-origin: 50% 50%; }
    @keyframes spinCircle { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
  </style>
</head>
<body>

  <div class="header-container">
    <div class="title-area">
      <h1>Panel de Control <span>ESPOL</span></h1>
      <p>Sistema de Automatización IoT API Local</p>
    </div>
    <div class="status-area">
      <div class="time-display" id="reloj">00:00</div>
      <div class="date-display" id="fecha">--</div>
      <div class="status-badge" id="statusBadge">
        <div class="status-dot" id="statusDot"></div>
        <span id="statusText">Buscando Red...</span>
      </div>
    </div>
  </div>

  <div class="dashboard">
    <div class="panel">
      <h2>Cámara de Vacío <span>HX710B</span></h2>
      <span class="telemetria-valor"><span id="presion">--.-</span><span class="unidad"> mmHg</span></span>
      <canvas id="graficaPresion"></canvas>
      <div class="estado-maquina" id="estado">Iniciando sistema...</div>
    </div>

    <div class="panel">
      <h2>Modulador Térmico <span>AC Control</span></h2>
      
      <!-- Zona Animada de Proceso -->
      <div id="boxTimer">
        <div style="position: relative; width: 120px; height: 120px; margin: 0 auto;">
          <svg width="120" height="120" class="timer-circle">
            <circle class="timer-bg" cx="60" cy="60" r="50"></circle>
            <circle class="timer-prog" id="progresoTimer" cx="60" cy="60" r="50"></circle>
          </svg>
          <div class="timer-text" id="textoTimer">00:00</div>
        </div>
        <div id="textoEstadoTimer" style="color: var(--accent-red); font-weight: 800; margin-top: 15px; letter-spacing: 1px;">CALENTANDO NIQUELINA</div>
      </div>

      <!-- Zona de Configuración -->
      <div id="boxConfiguracion">
        
        <div style="text-align: center; color: var(--text-muted); font-size: 0.85rem; margin-bottom: 5px;">Módulo Automático (Control PI)</div>
        <div class="auto-grid">
          <button id="btnAuto60" class="btn-auto" onclick="enviarComandoAuto(60)" disabled>60 mmHg</button>
          <button id="btnAuto80" class="btn-auto" onclick="enviarComandoAuto(80)" disabled>80 mmHg</button>
          <button id="btnAuto90" class="btn-auto" onclick="enviarComandoAuto(90)" disabled>90 mmHg</button>
        </div>

        <hr style="border: 0; height: 1px; background: rgba(255,255,255,0.05); margin: 25px 0;">

        <div style="margin: 20px 0;">
          <div style="text-align: center; color: var(--text-muted); font-size: 0.85rem; margin-bottom: 20px;">Módulo Manual (Lazo Abierto)</div>
          <div style="display: flex; justify-content: space-between; align-items: center;">
            <span style="color: var(--text-muted); font-size: 0.9rem;">Ajuste Potencia:</span>
            <span style="color: #fff; font-size: 1.2rem; font-weight: 800;"><span id="valPotencia">50</span>%</span>
          </div>
          <input type="range" id="sliderPotencia" min="0" max="100" value="50" oninput="document.getElementById('valPotencia').innerText = this.value" disabled>
          
          <div style="display: flex; justify-content: space-between; align-items: center; margin-top: 10px;">
            <span style="color: var(--text-muted); font-size: 0.9rem;">Ajuste Tiempo:</span>
            <span style="color: #fff; font-size: 1.2rem; font-weight: 800;"><span id="valTiempo">5</span> Min</span>
          </div>
          <input type="range" id="sliderTiempo" min="1" max="30" value="5" oninput="document.getElementById('valTiempo').innerText = this.value" disabled>

          <button id="btnManual" class="btn-manual" onclick="enviarComandoManual()" disabled>Fijar Valores Manuales</button>
        </div>
      </div>
      
      <!-- Gatillos -->
      <div style="border-top: 1px solid rgba(255,255,255,0.05); margin-top: 25px; padding-top: 10px;">
        <button id="btnStart" class="btn-start" onclick="enviarComando('START')" disabled>▶ 1. Iniciar Ciclo</button>
        <button id="btnSiguiente" class="btn-next" onclick="enviarComando('NEXT')" disabled>⏩ 2. Avanzar Fase / Detener</button>
        <button id="btnStop" class="btn-stop" onclick="enviarComando('STOP')" disabled>■ Paro de Emergencia</button>
      </div>
    </div>

    <div class="panel">
      <h2>Monitor de Dispositivos <span>Actuadores</span></h2>
      
      <div style="margin-top: 25px;">
        <div class="hardware-status">
          <span>Válvula de Succión (Bomba)</span>
          <label class="switch">
            <input type="checkbox" id="chkSuccion" onchange="enviarComando('TOGGLE_SUCCION')" disabled>
            <span class="slider"></span>
          </label>
        </div>
        
        <div class="hardware-status ecg-box">
          <span class="label">Monitor ECG (BPM)</span>
          <span class="ecg-value" id="valorECG">--</span>
        </div>
      </div>
    </div>
  </div>

  <script>
    function actualizarReloj() {
      const ahora = new Date();
      let horas = ahora.getHours().toString().padStart(2, '0');
      let minutos = ahora.getMinutes().toString().padStart(2, '0');
      document.getElementById('reloj').innerText = `${horas}:${minutos}`;
      
      const opcionesFecha = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
      document.getElementById('fecha').innerText = ahora.toLocaleDateString('es-ES', opcionesFecha);
    }
    setInterval(actualizarReloj, 1000);
    actualizarReloj();

    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var canvas = document.getElementById('graficaPresion');
    var ctx = canvas.getContext('2d');
    var historialPresion = new Array(50).fill(0); 

    function dibujarGrafica() {
      canvas.width = canvas.clientWidth;
      canvas.height = canvas.clientHeight;
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.strokeStyle = 'rgba(255,255,255,0.05)';
      ctx.lineWidth = 1;
      for(let i=0; i<5; i++) {
        ctx.beginPath();
        ctx.moveTo(0, (canvas.height/5)*i);
        ctx.lineTo(canvas.width, (canvas.height/5)*i);
        ctx.stroke();
      }
      ctx.beginPath();
      ctx.strokeStyle = '#4da8da'; 
      ctx.lineWidth = 3;
      for(let i=0; i < historialPresion.length; i++) {
        let x = (canvas.width / 49) * i;
        let y = canvas.height - (historialPresion[i] * (canvas.height / 800));
        if(i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      }
      ctx.stroke();
    }

    function setConnectionStatus(isOnline) {
      const badge = document.getElementById('statusBadge');
      const text = document.getElementById('statusText');
      
      if(isOnline) {
         badge.className = 'status-badge online';
         text.innerText = 'Enlace Activo';
      } else {
         badge.className = 'status-badge';
         text.innerText = 'Desconectado';
         document.getElementById('estado').innerText = "Fallo de comunicación...";
         
         document.getElementById('btnAuto60').disabled = true;
         document.getElementById('btnAuto80').disabled = true;
         document.getElementById('btnAuto90').disabled = true;
         document.getElementById('btnManual').disabled = true;
         document.getElementById('sliderPotencia').disabled = true;
         document.getElementById('sliderTiempo').disabled = true;
         document.getElementById('btnStart').disabled = true;
         document.getElementById('btnSiguiente').disabled = true;
         document.getElementById('btnStop').disabled = true;
         document.getElementById('chkSuccion').disabled = true;
      }
    }

    function initWebSocket() {
      websocket = new WebSocket(gateway);
      websocket.onopen = function(event) { setConnectionStatus(true); };
      
      websocket.onmessage = function(event) {
        var datos = JSON.parse(event.data);
        
        document.getElementById('presion').innerText = datos.presion;
        document.getElementById('estado').innerText = datos.estado;
        
        if(datos.ecg !== undefined && datos.ecg != -9999) {
          document.getElementById('valorECG').innerText = datos.ecg;
          var cajaECG = document.querySelector('.ecg-box');
          cajaECG.className = 'hardware-status ecg-box'; 
          if (datos.alertaECG === 'Taquicardia') { cajaECG.classList.add('ecg-taquicardia'); } 
          else if (datos.alertaECG === 'Bradicardia') { cajaECG.classList.add('ecg-bradicardia'); }
        }
        
        historialPresion.push(parseFloat(datos.presion));
        historialPresion.shift(); 
        dibujarGrafica();
        
        document.getElementById('chkSuccion').checked = (datos.vSuccion == 1);

        document.getElementById('btnAuto60').disabled = (datos.isBusy == 1);
        document.getElementById('btnAuto80').disabled = (datos.isBusy == 1);
        document.getElementById('btnAuto90').disabled = (datos.isBusy == 1);
        document.getElementById('btnManual').disabled = (datos.isBusy == 1);
        document.getElementById('sliderPotencia').disabled = (datos.isBusy == 1);
        document.getElementById('sliderTiempo').disabled = (datos.isBusy == 1);
        
        document.getElementById('btnStart').disabled = (datos.enabStart == 0);
        document.getElementById('btnSiguiente').disabled = (datos.enabNext == 0);
        document.getElementById('btnStop').disabled = false; 
        document.getElementById('chkSuccion').disabled = (datos.enabToggles == 0);

        // === AÑADIDO: EFECTO ESPEJO PARA EL SETPOINT (MODO AUTO) ===
        if (datos.estado === "Presione INICIAR CALOR" || datos.estado === "Control PI Continuo") {
            document.getElementById('btnAuto60').classList.remove('btn-auto-active');
            document.getElementById('btnAuto80').classList.remove('btn-auto-active');
            document.getElementById('btnAuto90').classList.remove('btn-auto-active');
            
            if(datos.setpoint == 60) document.getElementById('btnAuto60').classList.add('btn-auto-active');
            if(datos.setpoint == 80) document.getElementById('btnAuto80').classList.add('btn-auto-active');
            if(datos.setpoint == 90) document.getElementById('btnAuto90').classList.add('btn-auto-active');
        } else {
            // Limpia los botones si regresamos al inicio
            document.getElementById('btnAuto60').classList.remove('btn-auto-active');
            document.getElementById('btnAuto80').classList.remove('btn-auto-active');
            document.getElementById('btnAuto90').classList.remove('btn-auto-active');
        }

        // === AÑADIDO: EFECTO ESPEJO PARA BARRAS (MODO MANUAL) ===
        var tTotal = parseInt(datos.tTotal);
        if (datos.estado === "Presione INICIAR CALOR" && datos.setpoint == 0) { // Solo si no estamos en auto
            document.getElementById('sliderPotencia').value = datos.potencia;
            document.getElementById('valPotencia').innerText = datos.potencia;
            
            var tiempoMin = tTotal / 60;
            if (tiempoMin > 0) { 
               document.getElementById('sliderTiempo').value = tiempoMin;
               document.getElementById('valTiempo').innerText = tiempoMin;
            }
        }

        var tRestante = parseInt(datos.tiempo);
        var boxTimer = document.getElementById('boxTimer');
        var boxConfig = document.getElementById('boxConfiguracion');
        var circuloFondo = document.getElementById('progresoTimer');
        var txtTimer = document.getElementById('textoTimer');
        var txtEstado = document.getElementById('textoEstadoTimer');
        
        // --- ANIMACIONES DINÁMICAS ---
        if (datos.estado === "Control PI Continuo") {
           boxConfig.style.display = 'none';
           boxTimer.style.display = 'block';
           txtTimer.innerText = '∞';
           txtEstado.innerText = 'CONTROL PI ACTIVO';
           txtEstado.style.color = 'var(--accent-blue)';
           
           circuloFondo.style.stroke = 'var(--accent-blue)';
           circuloFondo.style.strokeDasharray = '157 157'; 
           circuloFondo.style.strokeDashoffset = '0';
           circuloFondo.classList.add('spin-anim'); 
        } 
        else if (tRestante > 0 && datos.estado === "Ciclo Térmico Activo") {
           boxConfig.style.display = 'none';
           boxTimer.style.display = 'block';
           
           var mins = Math.floor(tRestante / 60);
           var secs = tRestante % 60;
           txtTimer.innerText = (mins<10?'0':'') + mins + ':' + (secs<10?'0':'') + secs;
           txtEstado.innerText = 'CALENTANDO NIQUELINA';
           txtEstado.style.color = 'var(--accent-red)';
           
           circuloFondo.style.stroke = 'var(--accent-red)';
           circuloFondo.style.strokeDasharray = '314'; 
           var offset = 314 - (314 * tRestante / tTotal); 
           circuloFondo.style.strokeDashoffset = offset;
           circuloFondo.classList.remove('spin-anim'); 
        } 
        else {
           boxTimer.style.display = 'none';
           boxConfig.style.display = 'block';
           circuloFondo.classList.remove('spin-anim');
        }
      };
      
      websocket.onclose = function(event) { 
        setConnectionStatus(false); 
        setTimeout(initWebSocket, 2000); 
      };
    }

    window.addEventListener('load', () => {
      setConnectionStatus(false); 
      initWebSocket();
    });
    window.addEventListener('resize', dibujarGrafica); 

    function enviarComando(cmd) { websocket.send(cmd); }
    
    function enviarComandoAuto(setpoint) {
      websocket.send("AUTO:" + setpoint);
    }
    
    function enviarComandoManual() {
      var pot = document.getElementById('sliderPotencia').value;
      var tiempo = document.getElementById('sliderTiempo').value;
      websocket.send("MANUAL:" + pot + "," + tiempo); 
    }
  </script>
</body>
</html>
)rawliteral";

#endif