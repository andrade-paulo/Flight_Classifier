// MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Micro SD Shield
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Pins and consts
#define RCPin 26
#define led 2
#define FileName "/flight.csv"

// Global variables
Adafruit_MPU6050 MPU;
byte Cabecalho = LOW;
String FormatedData;
sensors_event_t a, g, t;
File file;

// PWM reading
volatile long StartTime = 0;
volatile long CurrentTime = 0;
volatile long Pulses = 0;
short PulseWidth = 0;

// SD Function
void appendFile(fs::FS &fs, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.println(message)){
        //Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}


void setup(void) {
  Serial.begin(115200);

  // Initialize Micro SD Shield
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    //return;
  }
    
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    //return;
  }
    
  // Initialize MPU6050
  if (!MPU.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    if (MPU.setTemperatureStandby(true)) {
      Serial.println("Temperature sensor in standby mode");
    }
  }

  // MPU range and bandwidth config
  MPU.setAccelerometerRange(MPU6050_RANGE_8_G);
  MPU.setGyroRange(MPU6050_RANGE_500_DEG);
  MPU.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Interruption
  pinMode(RCPin, INPUT);
  pinMode(led, OUTPUT);

  // Datalogger's Header
  appendFile(SD, "Time,Acel. X,Acel. Y,Acel. Z,Rot. X,Rot. Y,Rot. Z");

  // Interruption of RCPin for better performance
  attachInterrupt(RCPin, PulseTimer, CHANGE);
}


void loop() {
  // Get new sensor events with the readings
  if (Pulses < 2000) {
    PulseWidth = Pulses;  
  }
  Serial.println(PulseWidth);
  
  if (PulseWidth > 1100) {
    digitalWrite(led, HIGH);

    if (Cabecalho) {
      file = SD.open(FileName, FILE_APPEND);
      appendFile(SD, "Time,Acel. X,Acel. Y,Acel. Z,Rot. X,Rot. Y,Rot. Z");
      Cabecalho = LOW;
    }
    
    MPU.getEvent(&a, &g, &t);
  
    FormatedData = String(millis()) + "," + String(a.acceleration.x) + "," + String(a.acceleration.y) + "," + String(a.acceleration.z)
                   + "," + String(g.gyro.x) + "," + String(g.gyro.y) + "," + String(g.gyro.z);
                   
    //Serial.println(FormatedData);
    //Serial.println();
    
    appendFile(SD, FormatedData.c_str());
  } else {
    digitalWrite(led, LOW);
    file.close();
    Cabecalho = HIGH;

    Serial.println(Cabecalho);
  }
}


void PulseTimer() {
  CurrentTime = micros();
  if (CurrentTime > StartTime) {
    Pulses = CurrentTime - StartTime;
    StartTime = CurrentTime;
  }
}
