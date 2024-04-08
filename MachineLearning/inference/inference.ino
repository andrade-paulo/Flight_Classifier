// WiFi
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Artificial inteligence model
#include <Flight_Classifier_inferencing.h>

// Pins and consts
#define RCPin 26
#define led 17

// Global variables
Adafruit_MPU6050 mpu;
sensors_event_t a, g, t;
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
size_t feature_ix = 0;
static unsigned long last_interval_ms = 0;

// PWM reading
volatile long StartTime = 0;
volatile long CurrentTime = 0;
volatile long Pulses = 0;
short PulseWidth = 0;

// WiFi credentials
const char* ssid = "EspNet";
const char* password = "alanturing";

// Webpage HTML
String webpage = "<!DOCTYPE html><html> <style> * { margin: 0; padding: 0; box-sizing: border-box; } html { margin: 0; padding: 0; } body { margin: 0; padding: 0; text-align: center; background-color: #EEE; } header { margin: 0; padding: 0; height: 10vh; display: flex; justify-content: center; align-items: center; } main { margin: 0; height: 70vh; display: flex; flex-direction: column; justify-content: center; background-color: #e1e1e1; } h1 { font-size: 2em; } #label { font-size: 8em; } p { font-size: 1.2em; } </style> <head> <title>WebAero</title> </head> <body> <header> <h1>WebAero Classifier</h1> </header> <main> <div> <p>the current flight is </p> <p><span id='label'>-</span></p> <p id='description' style='align-self: flex-end;'>with <span id='value'>-</span> of probabillity</p> </div> </main> </body> <script> var Socket; function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function processCommand(event) { var obj = JSON.parse(event.data); if (obj.value > 0.6) { document.getElementById('value').style.color = 'green'; } else { document.getElementById('value').style.color = 'red'; } obj.label = obj.label.toUpperCase(); obj.value = obj.value * 100; document.getElementById('label').innerHTML = obj.label; document.getElementById('value').innerHTML = obj.value + '%'; console.log(obj.label); console.log(obj.value); } window.onload = function(event) { init(); } </script></html>";
WebServer server(80);  // the server uses port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // the websocket uses port 81

void setup() {
  Serial.begin(115200);
  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {  
    // wait until WiFi is connected
    delay(1000);
    Serial.print(".");
  }

  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP());

  // Server config
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();

  // MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
  }

  if (mpu.setTemperatureStandby(true)) {
    Serial.println("Temperature sensor in standby mode");
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Interruption
  pinMode(RCPin, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // Interruption of RCPin for better performance
  attachInterrupt(RCPin, PulseTimer, CHANGE);

  // Classifier info
  Serial.print("Features: ");
  Serial.println(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  Serial.print("Label count: ");
  Serial.println(EI_CLASSIFIER_LABEL_COUNT);
}


void loop() {
  // Server loop
  server.handleClient();
  webSocket.loop(); 
  
  if (Pulses < 3000) {
    PulseWidth = Pulses;
  }

  //Serial.println(PulseWidth);

  if (PulseWidth > 1100) {
    last_interval_ms = millis();
    digitalWrite(led, HIGH);

    // Data collection
    mpu.getEvent(&a, &g, &t);

    features[feature_ix++] = a.acceleration.x;
    features[feature_ix++] = a.acceleration.y;
    features[feature_ix++] = a.acceleration.z;
    features[feature_ix++] = g.gyro.x;
    features[feature_ix++] = g.gyro.y;
    features[feature_ix++] = g.gyro.z;

    //  Features reading
    if (feature_ix == EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
      signal_t signal;
      ei_impulse_result_t result;
      int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
      if (err != 0) {
        ei_printf("Failed to create signal from buffer (%d)\n", err);
        return;
      }

      // Classifier
      EI_IMPULSE_ERROR res = run_classifier(&signal, &result);

      if (res != 0) return;

      // Predictions
      byte predictLabel = 0;

      for (int i = 1; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > result.classification[predictLabel].value) predictLabel = i;
      }

      //ei_printf("Prediction: ");
      //ei_printf("    %s: %.5f\n", result.classification[predictLabel].label, result.classification[predictLabel].value);

      // Send prediction to website
      String jsonString = "";
      StaticJsonDocument<200> doc;
      JsonObject object = doc.to<JsonObject>();
      object["label"] = result.classification[predictLabel].label;
      object["value"] = result.classification[predictLabel].value;
      serializeJson(doc, jsonString);
      Serial.println(jsonString);
      webSocket.broadcastTXT(jsonString);

      feature_ix = 0;
    }
  } else {
    digitalWrite(led, LOW);
  }
}


void PulseTimer() {
  CurrentTime = micros();
  if (CurrentTime > StartTime) {
    Pulses = CurrentTime - StartTime;
    StartTime = CurrentTime;
  }
}


byte max(ei_impulse_result_t result) {
  byte bigger = 0;

  for (int i = 1; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > result.classification[bigger].value) bigger = i;
  }

  return bigger;
}


void ei_printf(const char *format, ...) {
  static char print_buf[1024] = { 0 };

  va_list args;
  va_start(args, format);
  int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);

  if (r > 0) {
    Serial.write(print_buf);
  }
}
