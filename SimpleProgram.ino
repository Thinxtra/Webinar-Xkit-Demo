/*
  SimpleProgram.ino - Demo application for Xkit with Arduino board
  Version 1.0
  Created by Gildas Seimbille, Thinxtra Solution Pty.
  May 20, 2017.
  Using Adafruit_BMP280_Library and Adafruit_MMA8451_Library
  Copyright (c) 2012, Adafruit Industries
  Modified by Thomas Ho
  Released into the public domain.
*/

// Include librairies
#include <WISOL.h>
#include <Tsensors.h>
#include <Wire.h>
#include <math.h>

Isigfox *Isigfox = new WISOL();
Tsensors *tSensors = new Tsensors();

// Init function
void setup() {
  Wire.begin();
  Wire.setClock(100000);

  // Init serial connection between Arduino and Modem
  Serial.begin(9600);

  // WISOL modem test
  Isigfox->initSigfox();
  Isigfox->testComms();

  // Init sensors on Thinxtra Module
  tSensors->initSensors();

  // Init an interruption on the button of the Xkit
  tSensors->setButton(buttonIR);
}

// Infinite loop of the program
void loop() {
  float axeX = getAxeX();
  Serial.print("Check Axe X: "); Serial.println(axeX);
  if (axeX <= -0.4 or axeX >= 0.4) {
    // Get Temperature
    float temp = getTemp();
    // Send Temperature to Sigfox
    Send_Pload((const char*)&temp, sizeof(temp));
    // Wait 20s
    delay(20000);
  }
  delay(1000);
}

// Return the acceleration on Axe X
float getAxeX() {
  acceleration_xyz *xyz_g;

  xyz_g = (acceleration_xyz *)malloc(sizeof(acceleration_xyz));
  tSensors->getAccXYZ(xyz_g);

  float axeX = (float)xyz_g->x_g;
  free(xyz_g);

  return axeX;
}

// Get the temperature
float getTemp() {
  float temp = round(tSensors->getTemp() * 10) / 10.0;
  Serial.print("Sending Temp: "); Serial.println(temp);

  return temp;
}

// SendPayload Function => Send messages to the Sigfox Network
void Send_Pload(uint8_t *sendData, int len) {
  recvMsg *RecvMsg;

  RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
  Isigfox->sendPayload(sendData, len, 0, RecvMsg);
  for (int i = 0; i < RecvMsg->len; i++) {
    Serial.print(RecvMsg->inData[i]);
  }
  Serial.println("");
  free(RecvMsg);
}

// Button Interruption
void buttonIR(){
    float temp = getTemp();
    Send_Pload((const char*)&temp, sizeof(temp));
}
