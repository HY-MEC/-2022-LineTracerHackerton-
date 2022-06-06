#include <DynamixelShield.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART RX/TX
#define DEBUG_SERIAL soft_serial
#elif defined(ARDUINO_SAM_DUE) ||dxl.setGoalVelocity(1, 100); //use default raw value defined(ARDUINO_SAM_ZERO)
#define DEBUG_SERIAL SerialUSB
#else
#define DEBUG_SERIAL Serial
#endif

#define R 8
#define G 9
#define B 10
#define LED 12

int pot;

int ww = 80;
int sp1 = 110;
int sp2 = 0;

int mode = 1;
int count = 0;
int tcount = 0;
int gcount = 0;

void linetracing();
void go();
void go2();

const uint8_t DXL_ID_1 = 1; //첫번째 다이나믹 셀의 id를 입력
const uint8_t DXL_ID_0 = 0; //두번째 다이나믹 셀의 id를 입력
const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl_1;
DynamixelShield dxl_0;

//This namespace is required to use Control table item names
using namespace ControlTableItem;

Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


void setup() {
  pinMode(A0, INPUT);  //디지털 4번핀을 입력 모드로 설정(IR 센서)
  pinMode(A1, INPUT);  //디지털 3번핀을 입력 모드로 설정(IR 센서)
  pinMode(A2, INPUT);  //디지털 4번핀을 입력 모드로 설정(IR 센서)
  pinMode(A3, INPUT);  //디지털 3번핀을 입력 모드로 설정(IR 센서)
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(A4, INPUT);

  Serial.begin(57600);
  // put your setup code here, to run once:
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);
  //  dxl_1.setGoalVelocity(1, 100); //use default raw value
  //  dxl_0.setGoalVelocity(0, 100); //use default raw value
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


  dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(850);
  dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(400);
}

void loop() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);

  uint16_t clear, red, green, blue; // unsigned short int형 변수 선언

  TCS.getRawData(&red, &green, &blue, &clear); // 색상 감지 센서에서 측정 값 받아오기

  pot = analogRead(A4);
  switch (mode)
  { case 1:
      {
        if (tcount == 0) {
          linetracing3();
          if (analogRead(A0) < ww)
          {
            go();
            tcount = 1;
            count++;
          }
          if (analogRead(A3) < ww)
          {
            go();
            tcount = 2;
            count++;
          }
        }
        else if (tcount == 1)
        {
          if (count == 4)
          {
            tcount = 0;
          }
          if (count == 6)
          {
            tcount = 0;
            mode++;
          }
          dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
          dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
          if (analogRead(A2) < ww)
          {
            tcount = 0;
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(150);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
          }
        }
        else if (tcount == 2)
        {
          if (count == 4)
          {
            tcount = 0;
          }
          if (count == 6)
          {
            tcount = 0;
            mode++;
          }
          dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
          dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
          if (analogRead(A1) < ww)
          {
            tcount = 0;
            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(150);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
          }
        }

        break;
      }
      { case 2:
        {
          linetracing2();
          if (pot >= 960) { //어두운 곳 진입
            analogWrite(B, 255);
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 110 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 110 속도로 구동
          }
          Serial.println(pot);
          if (pot < 960) { //밝은 곳 진입
            analogWrite(R, 0);
            analogWrite(G, 0);
            analogWrite(B, 0);
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 정지
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 정지
          }
          if (blue > red && blue > green && clear > 280 && clear < 450)
          {
            analogWrite(R, 0);
            analogWrite(G, 0);
            analogWrite(B, 255);
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(450);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(2000);
            mode++;
          }
          break;
        }
      }
      { case 3:
        {
          linetracing();

          if (red > blue && red > green && clear > 450 && clear < 750 && green < 120)
          {

            analogWrite(R, 255);
            analogWrite(G, 0);
            analogWrite(B, 0);
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            go1();

            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
          }
          if (blue > red && blue > green && clear > 240 && clear < 450)
          {

            analogWrite(R, 0);
            analogWrite(G, 0);
            analogWrite(B, 255);
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            go1();

            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(2000);
            mode++;
          }
          break;
        }
      }
      { case 4:
        {
          linetracing();
          if (analogRead(A3) < ww)
          {
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            go();
            analogWrite(R, 122);
            analogWrite(G, 0);
            analogWrite(B, 255);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(600);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(5000);
            mode = 6;
          }
        }
        break;
      }
      { case 5:
        {
          linetracing2();
          if (analogRead(A0) < ww)
          {
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(700);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(600);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            go();
            analogWrite(R, 122);
            analogWrite(G, 0);
            analogWrite(B, 255);
            go();
            dxl_1.setGoalVelocity(DXL_ID_1, 80); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 80);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(5000);
            mode++;
            count = 0;
          }
          /*
                    if (red > blue && red > green && clear > 200 && clear < 700 && green < 150)
                    {

                      dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
                      dxl_0.setGoalVelocity(DXL_ID_0, -110);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
                      delay(450);
                      dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
                      dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
                      delay(500);
                      mode++;
                      count = 0;
                    }
          */
          /*
            if (tcount == 0) {
            linetracing();
            if (analogRead(A0) < ww)
            {
              go();
              tcount = 1;
              count++;
            }
            if (analogRead(A3) < ww)
            {
              go();
              tcount = 2;
              count++;
            }
            }
            else if (tcount == 1)
            {
            if (count == 3 && count == 4 && count == 5)
            {
              tcount = 0;
            }
            if (count == 6)
            {
              tcount = 0;
              dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(1000);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
              go1();
              dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(1000);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
            }
            if (count == 7)
            {
              dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(450);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(2000);
              mode++;
            }
            dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            tcount = 0;
            }
            else if (tcount == 2)
            {
            if (count == 3 && count == 4 && count == 5)
            {
              tcount = 0;
            }
            if (count == 6)
            {
              tcount = 0;
              dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(1000);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
              go1();
              dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(1000);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
            }
            if (count == 7)
            {
              dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(450);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(2000);
              mode++;
            }
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(1000);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            tcount = 0;
            }
          */
        }
        break;
      }
      { case 6:
        {
          linetracing();
          if (red > blue && red > green && clear > 550 && clear < 750 && green < 130)
          {
            analogWrite(R, 255);
            analogWrite(G, 0);
            analogWrite(B, 0);
            dxl_1.setGoalVelocity(DXL_ID_1, 110); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -70);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(600);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            mode++;
          }
        }
        break;
      }
      { case 7:
        {
          if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -90);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 80); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 50);  //DXL_ID_0 다이나믹셀은 정지
            //로봇을 우회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, -50); //DXL_ID_1 다이나믹셀은 정지
            dxl_0.setGoalVelocity(DXL_ID_0, -80);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 좌회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -90);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A3) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -90);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }

          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A0) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -90);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          if (red > blue && red > green && clear > 550 && clear < 750 && green < 150)
          {
            analogWrite(R, 255);
            analogWrite(G, 0);
            analogWrite(B, 0);
            dxl_1.setGoalVelocity(DXL_ID_1, 70); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -110);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            mode++;
          }
        }
        break;
      }
      { case 8:
        {
          if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 정지
            //로봇을 우회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, -60); //DXL_ID_1 다이나믹셀은 정지
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 좌회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A3) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }

          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A0) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          if (green > blue && green > red && clear > 300 && clear < 700 && red < 200)
          {
            analogWrite(R, 0);
            analogWrite(G, 255);
            analogWrite(B, 0);
            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -120);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            mode++;
          }
        }
        break;
      }
      { case 9:
        {
          if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 130); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 130); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 60);  //DXL_ID_0 다이나믹셀은 정지
            //로봇을 우회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, -60); //DXL_ID_1 다이나믹셀은 정지
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 좌회전 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 130); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A3) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 130); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }

          else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A0) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
            dxl_1.setGoalVelocity(DXL_ID_1, 130); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -130);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            //로봇을 직진 시킴
          }
          if (green > blue && green > red && clear > 200 && clear < 700 && red < 200)
          {
            analogWrite(R, 0);
            analogWrite(G, 255);
            analogWrite(B, 0);

            dxl_1.setGoalVelocity(DXL_ID_1, 90); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -110);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(500);
            mode++;
          }
        }
        break;
      }
      { case 10:
        {
          linetracing();
          if (analogRead(A0) < ww)
          {
            dxl_1.setGoalVelocity(DXL_ID_1, 110); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, -90);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(400);
            dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
            dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
            delay(300);
          }
          
            if (blue > red && blue > green && clear > 220 && clear < 450)
            {

              analogWrite(R, 0);
              analogWrite(G, 0);
              analogWrite(B, 255);
              go();
              dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(950);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
              go1();
              go();
              dxl_1.setGoalVelocity(DXL_ID_1, -sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(1000);
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(500);
              go2();
              go1();
              dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
              dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
              delay(30000);



            }

          


        }

        break;
      }
  }
  /*

    uint16_t clear, red, green, blue; // unsigned short int형 변수 선언

    delay(10); // 1초마다 측정하고 출력

    TCS.getRawData(&red, &green, &blue, &clear); // 색상 감지 센서에서 측정 값 받아오기

    int r = map(red, 0, 21504, 0, 1025);    // 빨간색 데이터 값
    int g = map(green, 0, 21504, 0, 1025);  // 초록색 데이터 값
    int b = map(blue, 0, 21504, 0, 1025);   // 파란색 데이터 값
    int c = map(clear, 0, 21504, 0, 1025);   // 파란색 데이터 값

    dxl_1.setGoalVelocity(DXL_ID_1, 50);
    dxl_0.setGoalVelocity(DXL_ID_0, -50);
    delay(500);
    dxl_1.setGoalVelocity(DXL_ID_1, 0);
    dxl_0.setGoalVelocity(DXL_ID_0, 0);
    delay(500);
  */
}

void linetracing() {
  if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
  else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, 50);  //DXL_ID_0 다이나믹셀은 정지
    //로봇을 우회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, -50); //DXL_ID_1 다이나믹셀은 정지
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 좌회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A3) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }

  else if (analogRead(A1) <= ww && analogRead(A2) <= ww && analogRead(A0) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
}

void linetracing2() {
  if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
  else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp2);  //DXL_ID_0 다이나믹셀은 정지
    //로봇을 우회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp2); //DXL_ID_1 다이나믹셀은 정지
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 좌회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, 20); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, 60);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
}

void linetracing3() {
  if (analogRead(A1) > ww && analogRead(A2) > ww) { //양쪽 IR 센서 모두 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
  else if (analogRead(A1) > ww && analogRead(A2) <= ww) { //왼쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, -sp2);  //DXL_ID_0 다이나믹셀은 정지
    //로봇을 우회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) > ww) { //오른쪽 IR 센서만 검정색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, sp2); //DXL_ID_1 다이나믹셀은 정지
    dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 좌회전 시킴
  }
  else if (analogRead(A1) <= ww && analogRead(A2) <= ww) { //양쪽 IR 센서 모두 흰색을 인식했을때
    dxl_1.setGoalVelocity(DXL_ID_1, 50); //DXL_ID_1 다이나믹셀은 50 속도로 구동
    dxl_0.setGoalVelocity(DXL_ID_0, 50);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
    //로봇을 직진 시킴
  }
}

void go() {
  dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(900);
  dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(400);
}

void go2() {
  dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(3000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(400);
}

void go1() {
  dxl_1.setGoalVelocity(DXL_ID_1, sp1); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, -sp1);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(2000);
  dxl_1.setGoalVelocity(DXL_ID_1, 0); //DXL_ID_1 다이나믹셀은 50 속도로 구동
  dxl_0.setGoalVelocity(DXL_ID_0, 0);  //DXL_ID_0 다이나믹셀은 50 속도로 구동
  delay(400);
}
