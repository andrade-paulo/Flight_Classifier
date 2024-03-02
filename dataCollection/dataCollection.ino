// MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Micro SD Shield
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Global variables
const byte button = 4;
const byte led = 16;

Adafruit_MPU6050 mpu;
volatile byte writing = LOW;  // Writing on file
String formatedData;

const char* fileName = "/teste.csv";


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.println(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}


void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.println(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}


void setup(void) {
  Serial.begin(115200);

  // Initialize Micro SD Shield
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
    
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
    
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // MPU range and bandwidth config
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Interruption
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(button), toggleWriting, FALLING);

  // Header of datalogger
  writeFile(SD, fileName, "Time,Acel. X,Acel. Y,Acel. Z,Rot. X,Rot. Y,Rot. Z");
}


void loop() {
  // Get new sensor events with the readings
  if (writing) {
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);
  
    formatedData = String(millis()) + "," + String(a.acceleration.x) + "," + String(a.acceleration.y) + "," + String(a.acceleration.z)
                   + String(g.gyro.x) + "," + String(g.gyro.y) + "," + String(g.gyro.z);
                   
    Serial.println(formatedData);
    Serial.println();
    
    appendFile(SD, fileName, formatedData.c_str());
  }

  digitalWrite(led, writing);
  delay(100);
}


void toggleWriting() {
  writing = !writing;
}
