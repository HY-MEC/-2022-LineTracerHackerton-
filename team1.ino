/*******************************************************************************
  Copyright 2016 ROBOTIS CO., LTD.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*******************************************************************************/
#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
#include <DynamixelShield.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART RX/TX
#define DEBUG_SERIAL soft_serial
#elif defined(ARDUINO_SAM_DUE) ||dxl.setGoalVelocity(1, 100); //use default raw value defined(ARDUINO_SAM_ZERO)
#define DEBUG_SERIAL SerialUSB
#else
#define DEBUG_SERIAL Serial
#endif

const uint8_t DXL_ID_1 = 1; //첫번째 다이나믹 셀의 id를 입력
const uint8_t DXL_ID_0 = 0; //두번째 다이나믹 셀의 id를 입력
const float DXL_PROTOCOL_VERSION = 2.0;
//왼쪽 0번 오른쪽 1번
DynamixelShield dxl_1;
DynamixelShield dxl_0;
int ir1 = A8;
int ir2 = A9;
int ir3 = A10;
int ir4 = A11;
int cds = A12;
int led1 = 22;
int led2 = 24;
int led3 = 26;
float red = 0;
float blue = 0;
float green = 0;
int a;
unsigned long time_f = 0;
//This namespace is required to use Control table item names
using namespace ControlTableItem;

void setup() {
  // put your setup code here, to run once:

  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);
  //dxl_1.setGoalVelocity(1, 100); //use default raw value
  //dxl_0.setGoalVelocity(0, 100); //use default raw value


  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl_1.begin(57600);
  dxl_0.begin(57600);


  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl_1.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  dxl_0.setPortProtocolVersion(DXL_PROTOCOL_VERSION);


  // Get DYNAMIXEL information
  dxl_1.ping(DXL_ID_1);
  dxl_0.ping(DXL_ID_0);

  // Turn off torque when configuring items in EEPROM area
  dxl_1.torqueOff(DXL_ID_1);
  dxl_1.setOperatingMode(DXL_ID_1, OP_VELOCITY);
  dxl_1.torqueOn(DXL_ID_1);

  dxl_0.torqueOff(DXL_ID_0);
  dxl_0.setOperatingMode(DXL_ID_0, OP_VELOCITY);
  dxl_0.torqueOn(DXL_ID_0);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(cds, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  if (tcs.begin()) {
    // tcs34725센서가 연결되어 인식했을 경우
    DEBUG_SERIAL.println("Found sensor");
  }
  else { // tcs34725센서가 연경되지 않아 인식하지 못했을 경우
    DEBUG_SERIAL.println("No TCS34725 found ... check your connections");
    while (1); // 중단
  }

}


void loop() {
  // put your main code here, to run repeatedly:
  //직진 0 오른쪽 +150 1은 왼쪽 -150
  // Please refer to e-Manual(http://emanual.robotis.com) for available range of value.
  // Set Goal Velocity using RAW unit
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(500);
  //1구간

  LL(110);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(300);
  RT(150, 1200);
  LT();
  RR(150);
  R(150);
  tunnel();
  R(110);//3번
  digitalWrite(led1, LOW);  //led off
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //우회전 45도
  dxl_0.setGoalVelocity(DXL_ID_0, 110);
  delay(450);
  LR();
  RR(150);
  RR(150);
  LL(150);
  RR(150);
  R(150);
  //별
  RT(150, 1000);

  R(100);//1
  R(150);
  RR(150);
  R(150);
  RR(150);
  LL(150);
  LL(150);
  R(150);
  RR(150);
  LL(150);
  LL(150);
  L();
  L();
  L();
  RR(150);

  RR_45(120);//6-------
  //4

  R_1(110);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(300);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  delay(100);

  R_1(110);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(300);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  delay(1000);

  R_1(90);//빨강

  R_1(110);//빨강
  dxl_0.setGoalVelocity(DXL_ID_0, -90);
  dxl_1.setGoalVelocity(DXL_ID_1, -90);
  delay(300);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  delay(1000);
  R_1(130);//초록
  delay(1000);
  R_1(110);//초록
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(400);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  delay(1000);
  R_1(110);//T

  R_1(110);//T

  R_1(110);//T
  GREEN();

  while (true)
  {
    dxl_0.setGoalVelocity(DXL_ID_0, 0);
    dxl_1.setGoalVelocity(DXL_ID_1, 0);
  }
  //delay(1000);


}
void sensors()
{
  // ir1 인식 180 그냥 870
  // ir2 인식 170 그냥 840
  // ir3 인식 210 그냥 870
  // ir4 인식 40 그냥 640
  // cds 인식 80 그냥 10
  uint16_t r, g, b, c, colorTemp, lux;

  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  ir1 = analogRead(A8);
  ir2 = analogRead(A9);
  ir3 = analogRead(A10);
  ir4 = analogRead(A11);
  cds = analogRead(A12);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.print(" ");
  float red = (float)r * r / g / b > 2.5;
  float green = (float)g * g / r / b > 2;
  float blue =  (float)b * b / r / g > 1.5;
  Serial.print("R: "); Serial.print(red); Serial.print(" ");
  Serial.print("G: "); Serial.print(green); Serial.print(" ");
  Serial.print("B: "); Serial.print(blue); Serial.print(" ");
  Serial.print("ir1: "); Serial.print(ir1); Serial.print(" ");
  Serial.print("ir2: "); Serial.print(ir2); Serial.print(" ");
  Serial.print("ir3: "); Serial.print(ir3); Serial.print(" ");
  Serial.print("ir4: "); Serial.print(ir4); Serial.print(" ");
  Serial.print("cds: "); Serial.print(cds); Serial.println(" ");
}

void LT() {
  sensors();
  while (ir1 > 350 || ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, -90);
  dxl_1.setGoalVelocity(DXL_ID_1, -90);
  delay(1200);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void LL(int a) {
  sensors();
  while (ir1 > 350) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, -90);
  dxl_1.setGoalVelocity(DXL_ID_1, -90);
  delay(1200);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void RT(int a, int b)
{
  sensors();
  while (ir1 > 350 || ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(b);//1200
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void RR(int a)
{
  sensors();
  while (ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(1200);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void RR_45(int a)
{
  sensors();
  while (ir2 > 350 || ir3 > 350) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, 90);
  dxl_1.setGoalVelocity(DXL_ID_1, 90);
  delay(700);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void LR()
{
  sensors();
  while (ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(600);
  dxl_0.setGoalVelocity(DXL_ID_0, -90);
  dxl_1.setGoalVelocity(DXL_ID_1, -90);
  delay(1200);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void T()
{
  sensors();
  while (ir1 > 350 || ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(300);

}
void R(int a)
{
  sensors();
  while (ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(300);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void R_1(int a)
{
  sensors();
  while (ir4 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 80);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, -80);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -a);
      dxl_1.setGoalVelocity(DXL_ID_1, a);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  delay(500);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);

}
void L()
{
  sensors();
  while (ir1 > 300) {
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }
  }
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(300);

}
void tunnel() {
  sensors();
  while (cds > 15) {
    //밝은곳에서 어두운곳
    sensors();
    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }

  }
  digitalWrite(led1, HIGH);  //led on
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  dxl_0.setGoalVelocity(DXL_ID_0, -150);
  dxl_1.setGoalVelocity(DXL_ID_1, 150);
  delay(300);
  sensors();
  while (cds < 20) {
    //어두운에서 밝은곳
    sensors();

    if (ir2 > 350 && ir3 < 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 10);
    }
    else if (ir2 < 350 && ir3 > 350)
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -10);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else
    {
      dxl_0.setGoalVelocity(DXL_ID_0, -150);
      dxl_1.setGoalVelocity(DXL_ID_1, 150);
    }

  }
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  delay(1000);




}


void GREEN() {
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(6500);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
}

void RED() {
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, -110); //좌회전
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110 );
  delay(2500);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //우회전
  dxl_0.setGoalVelocity(DXL_ID_0, 110);
  delay(900);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);  //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110 );
  delay(5500);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
}


void BLUE() {
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //우회전
  dxl_0.setGoalVelocity(DXL_ID_0, 110);
  delay(900);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110); //직진
  dxl_0.setGoalVelocity(DXL_ID_0, -110 );
  delay(2500);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, -110);  //좌회전
  dxl_0.setGoalVelocity(DXL_ID_0, -110);
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
  delay(100);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110 );
  delay(5500);
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
}
