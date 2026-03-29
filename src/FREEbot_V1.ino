#include <WiFi.h>
#include <WebServer.h>
#include <AsyncUDP.h>
#include <Arduino.h>
#include "DataParser.h"

// Change to Access Point mode instead of connecting to existing WiFi
const char* ssid = "RobotCarWifi";      // This will be the network name
const char* password = "12345678";   // Password for the network

DataParser dataParser;

const int udpPort = 12345;

// Motor pins
int in1 = 27;
int in2 = 26;
int ena = 14;
int in3 = 25;
int in4 = 33;
int enb = 32;

int Speed = 150;
int Right_speed = 0;
int Left_speed = 0;

// Flag to track if robot is stopped/locked
bool isStopped = false;

AsyncUDP udp;
WebServer server(80);

// Web page HTML with toggle stop functionality
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>Robot Car Controller</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            user-select: none;
        }
        
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            background: white;
            border-radius: 20px;
            padding: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            max-width: 500px;
            width: 100%;
        }
        
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 10px;
            font-size: 24px;
        }
        
        .status {
            text-align: center;
            padding: 10px;
            margin: 10px 0;
            border-radius: 10px;
            font-weight: bold;
            transition: all 0.3s;
        }
        
        .status.active {
            background: #d4edda;
            color: #155724;
        }
        
        .status.stopped {
            background: #f8d7da;
            color: #721c24;
        }
        
        .ip-info {
            background: #e3f2fd;
            padding: 10px;
            border-radius: 8px;
            margin: 10px 0;
            text-align: center;
            font-size: 14px;
            word-break: break-all;
        }
        
        .control-panel {
            margin: 20px 0;
        }
        
        .dpad {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 15px;
            margin: 20px 0;
        }
        
        .dpad-row {
            display: flex;
            gap: 15px;
            justify-content: center;
        }
        
        .control-btn {
            width: 80px;
            height: 80px;
            border-radius: 50%;
            border: none;
            background: #667eea;
            color: white;
            font-size: 24px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.1s;
            box-shadow: 0 4px 10px rgba(0,0,0,0.2);
        }
        
        .control-btn:active {
            transform: scale(0.95);
            background: #5a67d8;
        }
        
        .control-btn.disabled {
            opacity: 0.5;
            cursor: not-allowed;
            background: #999;
        }
        
        .control-btn.disabled:active {
            transform: none;
            background: #999;
        }
        
        .control-btn.stop {
            background: #e74c3c;
            width: 100px;
            height: 100px;
            font-size: 20px;
        }
        
        .control-btn.stop.active-stop {
            background: #c0392b;
            animation: pulse 1s infinite;
        }
        
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.05); }
            100% { transform: scale(1); }
        }
        
        .control-btn.stop:active {
            background: #c0392b;
        }
        
        .speed-control {
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 10px;
        }
        
        .speed-control label {
            display: block;
            margin-bottom: 10px;
            font-weight: bold;
            color: #333;
        }
        
        .speed-control input {
            width: 100%;
            margin: 10px 0;
        }
        
        .speed-value {
            text-align: center;
            font-size: 18px;
            font-weight: bold;
            color: #667eea;
        }
        
        .command-log {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 10px;
            margin-top: 20px;
            max-height: 150px;
            overflow-y: auto;
            font-family: monospace;
            font-size: 12px;
        }
        
        .instructions {
            background: #fff3cd;
            border: 1px solid #ffc107;
            border-radius: 8px;
            padding: 10px;
            margin-bottom: 15px;
            font-size: 12px;
            color: #856404;
        }
        
        @media (max-width: 480px) {
            .control-btn {
                width: 70px;
                height: 70px;
                font-size: 20px;
            }
            .control-btn.stop {
                width: 90px;
                height: 90px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🤖 WiFi Robot Car</h1>
        <div id="statusDisplay" class="status active">Robot Active - Ready to Move</div>
        <div class="instructions">
            📡 Connect your phone/computer to WiFi network:<br>
            <strong>"RobotCarWifi"</strong> with password <strong>"12345678"</strong><br>
            🛑 Press STOP button once to LOCK robot in stop mode<br>
            🔄 Press STOP again to UNLOCK and resume control
        </div>
        <div class="ip-info" id="ipInfo">Loading robot IP...</div>
        
        <div class="control-panel">
            <div class="dpad">
                <div class="dpad-row">
                    <button id="btnForward" class="control-btn" ontouchstart="sendCommand('f')" ontouchend="sendCommand('s')" 
                            onmousedown="sendCommand('f')" onmouseup="sendCommand('s')">▲<br><small>FWD</small></button>
                </div>
                <div class="dpad-row">
                    <button id="btnLeft" class="control-btn" ontouchstart="sendCommand('l')" ontouchend="sendCommand('s')"
                            onmousedown="sendCommand('l')" onmouseup="sendCommand('s')">◄<br><small>LEFT</small></button>
                    <button id="btnStop" class="control-btn stop" onclick="toggleStop()">⏹️<br><small>STOP</small></button>
                    <button id="btnRight" class="control-btn" ontouchstart="sendCommand('r')" ontouchend="sendCommand('s')"
                            onmousedown="sendCommand('r')" onmouseup="sendCommand('s')">►<br><small>RIGHT</small></button>
                </div>
                <div class="dpad-row">
                    <button id="btnBackward" class="control-btn" ontouchstart="sendCommand('b')" ontouchend="sendCommand('s')"
                            onmousedown="sendCommand('b')" onmouseup="sendCommand('s')">▼<br><small>BACK</small></button>
                </div>
            </div>
            
            <div class="speed-control">
                <label>Speed Control</label>
                <input type="range" id="speedSlider" min="0" max="255" value="150" onchange="updateSpeed()">
                <div class="speed-value">Speed: <span id="speedValue">150</span></div>
            </div>
        </div>
        
        <div class="command-log">
            <strong>📋 Command Log</strong><br>
            <div id="logContent">Ready to control...</div>
        </div>
    </div>
    
    <script>
        let currentSpeed = 150;
        let isLocked = false;
        
        // Get robot IP
        fetch('/ip')
            .then(response => response.text())
            .then(ip => {
                document.getElementById('ipInfo').innerHTML = '🌐 Robot IP: http://' + ip;
            })
            .catch(() => {
                document.getElementById('ipInfo').innerHTML = '🌐 Robot IP: http://192.168.4.1';
            });
        
        // Function to toggle stop lock
        function toggleStop() {
            fetch('/toggleStop')
                .then(response => response.text())
                .then(status => {
                    if (status === 'stopped') {
                        isLocked = true;
                        addToLog('🔒 Robot STOPPED and LOCKED - Press STOP again to unlock');
                        document.getElementById('statusDisplay').className = 'status stopped';
                        document.getElementById('statusDisplay').innerHTML = '🔴 STOPPED - Robot Locked';
                        document.getElementById('btnStop').classList.add('active-stop');
                        
                        // Disable movement buttons visually
                        document.getElementById('btnForward').classList.add('disabled');
                        document.getElementById('btnBackward').classList.add('disabled');
                        document.getElementById('btnLeft').classList.add('disabled');
                        document.getElementById('btnRight').classList.add('disabled');
                    } else {
                        isLocked = false;
                        addToLog('🔓 Robot UNLOCKED - Ready to move');
                        document.getElementById('statusDisplay').className = 'status active';
                        document.getElementById('statusDisplay').innerHTML = 'Robot Active - Ready to Move';
                        document.getElementById('btnStop').classList.remove('active-stop');
                        
                        // Enable movement buttons
                        document.getElementById('btnForward').classList.remove('disabled');
                        document.getElementById('btnBackward').classList.remove('disabled');
                        document.getElementById('btnLeft').classList.remove('disabled');
                        document.getElementById('btnRight').classList.remove('disabled');
                    }
                });
        }
        
        function sendCommand(direction) {
            // Check current stop status before sending
            fetch('/getStopStatus')
                .then(response => response.text())
                .then(status => {
                    if (status === 'stopped' && direction !== 's') {
                        addToLog('❌ Robot is LOCKED - Press STOP to unlock first');
                        return;
                    }
                    
                    let command = direction;
                    if (direction !== 's') {
                        command = direction + ',' + currentSpeed;
                    } else {
                        command = 's,0';
                    }
                    
                    fetch('/command?cmd=' + encodeURIComponent(command))
                        .then(response => response.text())
                        .then(data => {
                            if (direction !== 's') {
                                addToLog('Sent: ' + command);
                            }
                        })
                        .catch(err => {
                            addToLog('Error: ' + err);
                        });
                });
        }
        
        function updateSpeed() {
            currentSpeed = parseInt(document.getElementById('speedSlider').value);
            document.getElementById('speedValue').innerText = currentSpeed;
            
            fetch('/speed?value=' + currentSpeed)
                .then(response => response.text())
                .then(data => {
                    addToLog('Speed set to: ' + currentSpeed);
                });
        }
        
        function addToLog(message) {
            const logDiv = document.getElementById('logContent');
            const time = new Date().toLocaleTimeString();
            logDiv.innerHTML = '[' + time + '] ' + message + '<br>' + logDiv.innerHTML;
            
            let lines = logDiv.innerHTML.split('<br>');
            if (lines.length > 10) {
                logDiv.innerHTML = lines.slice(0, 10).join('<br>');
            }
        }
        
        // Get initial stop status on page load
        function updateInitialStatus() {
            fetch('/getStopStatus')
                .then(response => response.text())
                .then(status => {
                    if (status === 'stopped') {
                        isLocked = true;
                        document.getElementById('statusDisplay').className = 'status stopped';
                        document.getElementById('statusDisplay').innerHTML = '🔴 STOPPED - Robot Locked';
                        document.getElementById('btnStop').classList.add('active-stop');
                        document.getElementById('btnForward').classList.add('disabled');
                        document.getElementById('btnBackward').classList.add('disabled');
                        document.getElementById('btnLeft').classList.add('disabled');
                        document.getElementById('btnRight').classList.add('disabled');
                    }
                });
        }
        
        // Keyboard controls
        document.addEventListener('keydown', function(event) {
            const key = event.key.toLowerCase();
            let command = null;
            
            switch(key) {
                case 'arrowup':
                case 'w':
                    command = 'f';
                    event.preventDefault();
                    break;
                case 'arrowdown':
                case 's':
                    command = 'b';
                    event.preventDefault();
                    break;
                case 'arrowleft':
                case 'a':
                    command = 'l';
                    event.preventDefault();
                    break;
                case 'arrowright':
                case 'd':
                    command = 'r';
                    event.preventDefault();
                    break;
                case ' ':
                case 'x':
                    toggleStop();
                    event.preventDefault();
                    return;
            }
            
            if (command) {
                sendCommand(command);
                if (command !== 's') {
                    const releaseHandler = function(e) {
                        if (e.key.toLowerCase() === key) {
                            sendCommand('s');
                            document.removeEventListener('keyup', releaseHandler);
                        }
                    };
                    document.addEventListener('keyup', releaseHandler);
                }
            }
        });
        
        // Initialize status on page load
        updateInitialStatus();
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Motor pin setup
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);
  
  Stop();
  
  // Start Access Point (ESP32 creates its own WiFi network)
  Serial.println("Starting Access Point...");
  WiFi.softAP(ssid, password);
  
  Serial.println("Access Point Started");
  Serial.print("Network Name (SSID): ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("Robot IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup web server routes
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  
  // Command endpoint - respects stop lock
  server.on("/command", []() {
    if (server.hasArg("cmd")) {
      String command = server.arg("cmd");
      Serial.print("Command received: ");
      Serial.println(command);
      
      // Check if robot is stopped/locked - only allow stop command when locked
      int commaIndex = command.indexOf(',');
      String direction;
      
      if (commaIndex > 0) {
        direction = command.substring(0, commaIndex);
      } else {
        direction = command;
      }
      
      // If robot is stopped and command is not stop, reject it
      if (isStopped && direction != "s") {
        Serial.println("Command rejected - robot is locked");
        server.send(403, "text/plain", "Robot is locked");
        return;
      }
      
      int speedValue = Speed;
      
      if (commaIndex > 0) {
        direction = command.substring(0, commaIndex);
        speedValue = command.substring(commaIndex + 1).toInt();
        if (direction != "s") {
          Speed = speedValue;
          Left_speed = Speed;
          Right_speed = Speed;
        }
      } else {
        direction = command;
      }
      
      if (direction == "f") {
        forward(Left_speed, Right_speed);
        Serial.println("Forward");
      } 
      else if (direction == "b") {
        backward(Left_speed, Right_speed);
        Serial.println("Backward");
      }
      else if (direction == "l") {
        left(Left_speed, Right_speed);
        Serial.println("Left");
      }
      else if (direction == "r") {
        right(Left_speed, Right_speed);
        Serial.println("Right");
      }
      else if (direction == "s") {
        Stop();
        Serial.println("Stop");
      }
      
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Missing command");
    }
  });
  
  // Toggle stop lock endpoint
  server.on("/toggleStop", []() {
    isStopped = !isStopped;
    if (isStopped) {
      Stop();  // Ensure motors are stopped when locking
      Serial.println("Robot STOPPED and LOCKED");
      server.send(200, "text/plain", "stopped");
    } else {
      Serial.println("Robot UNLOCKED - Ready to move");
      server.send(200, "text/plain", "active");
    }
  });
  
  // Get stop status endpoint
  server.on("/getStopStatus", []() {
    if (isStopped) {
      server.send(200, "text/plain", "stopped");
    } else {
      server.send(200, "text/plain", "active");
    }
  });
  
  server.on("/speed", []() {
    if (server.hasArg("value")) {
      Speed = server.arg("value").toInt();
      Left_speed = Speed;
      Right_speed = Speed;
      Serial.print("Speed set to: ");
      Serial.println(Speed);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Missing speed value");
    }
  });
  
  server.on("/ip", []() {
    server.send(200, "text/plain", WiFi.softAPIP().toString());
  });
  
  server.begin();
  Serial.println("Web server started");
  Serial.println("=================================");
  Serial.println("TO CONTROL THE ROBOT:");
  Serial.println("1. On your phone/computer, look for WiFi network: RobotCarWifi");
  Serial.println("2. Connect with password: 12345678");
  Serial.println("3. Open browser and go to: http://192.168.4.1");
  Serial.println("=================================");
  Serial.println("STOP BUTTON FEATURE:");
  Serial.println("- Press STOP once: Robot stops and LOCKS");
  Serial.println("- Press STOP again: Robot UNLOCKS for movement");
  Serial.println("=================================");
  
  // UDP setup (optional)
  if(udp.listen(udpPort)) {
    udp.onPacket([](AsyncUDPPacket packet) {
      String IncomingData = (char*)packet.data();
      dataParser.parseData(IncomingData, ',');
      // Only process UDP commands if not stopped
      if (!isStopped) {
        Speed = (dataParser.getField(1)).toInt();
        Left_speed = Speed;
        Right_speed = Speed;
      }
    });
  }
}

void loop() {
  server.handleClient();
  
  // Only process UDP commands if robot is not stopped/locked
  if (!isStopped) {
    if(dataParser.getField(0) == "f") {
      forward(Left_speed, Right_speed);
    }
    else if(dataParser.getField(0) == "b") {
      backward(Left_speed, Right_speed);
    }
    else if(dataParser.getField(0) == "l") {
      left(Left_speed, Right_speed);
    }
    else if(dataParser.getField(0) == "r") {
      right(Left_speed, Right_speed);
    }
    else if(dataParser.getField(0) == "s") {
      Stop();
    }
  }
}

void right(int left_speed, int right_speed) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void left(int left_speed, int right_speed) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void backward(int left_speed, int right_speed) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void forward(int left_speed, int right_speed) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void Stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(ena, 0);
  analogWrite(enb, 0);
}