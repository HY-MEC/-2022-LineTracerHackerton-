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

const uint8_t DXL_ID_1 = 1; //오른쪽 첫번째 다이나믹 셀의 id를 입력  
const uint8_t DXL_ID_0 = 0; //왼쪽 두번째 다이나믹 셀의 id를 입력 
const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl_1;
DynamixelShield dxl_0;

//This namespace is required to use Control table item names
using namespace ControlTableItem;




#include <Wire.h>  //I2C 통신을 위해 wire.h 라이브러리 사용 선언
#include "Adafruit_TCS34725.h"  //컬러 센서 사용을 위해 컬러 센서 전용 라이브러리 사용 선언

#define RGB_INT 6    //컬러 센서에서 외부 인터럽트 제어에 사용할 핀 지정
#define S_LED 12     //컬러 센서의 LED를 제어할 핀 지정
#define RED  11       //RGB LED의 RED LED를 제어하는데 사용할 디지털 핀 번호 지정
#define GREEN  10     //RGB LED의 GREEN LED를 제어하는데 사용할 디지털 핀 번호 지정
#define BLUE 9       //RGB LED의 BLUE LED를 제어하는데 사용할 디지털 핀 번호 지정
int rgb_r, rgb_g, rgb_b, rgb_c;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
volatile boolean rgb_flag = false;




#define POT A0       //조도 핀 번호 지정
int pot;
int change=0;
int color = 10;




const int RR = 3;
const int R  = 4;
const int L  = 5;
const int LL = 13;



int sensing = 0;
int num = 0;

unsigned long time_previous, time_current;
void setup() {

  DEBUG_SERIAL.begin(115200);
  dxl_1.setGoalVelocity(1, 100); //use default raw value
  dxl_0.setGoalVelocity(0, 100); //use default raw value

  dxl_1.begin(57600);
  dxl_0.begin(57600);

  dxl_1.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  dxl_0.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  dxl_1.ping(DXL_ID_1);
  dxl_0.ping(DXL_ID_0);

  dxl_1.torqueOff(DXL_ID_1);
  dxl_1.setOperatingMode(DXL_ID_1, OP_VELOCITY);
  dxl_1.torqueOn(DXL_ID_1);

  dxl_0.torqueOff(DXL_ID_0);
  dxl_0.setOperatingMode(DXL_ID_0, OP_VELOCITY);
  dxl_0.torqueOn(DXL_ID_0);


  pinMode(RGB_INT, INPUT_PULLUP);
  pinMode(S_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(RGB_INT), RGB_isr, FALLING);
  pinMode(L, INPUT);
  pinMode(R, INPUT);
  pinMode(LL, INPUT);
  pinMode(RR, INPUT);
  Serial.begin(57600);
  tcs.begin();
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  time_previous = millis(); //시작 시간
  time_current  = millis(); //현재 시간

}



void loop() {
  
  if(change==0){
    linetracing(); 
  }else if(change == 1){
    color_check();
    linetracing2();
    
  }else if(change == 2){
    linetracing();
    
  }
  

}
  


void RGB_isr(){
  rgb_flag = true;
  digitalWrite(S_LED, LOW);
}


void rgb_led(int red, int green, int blue){
  analogWrite(RED, red);
  analogWrite(GREEN,green);
  analogWrite(BLUE, blue);
}


void LEDOn(){
  
    pot = analogRead(A5);
   if (pot >= 910) {
      rgb_led(255, 255, 255);
      delay(5000);
      rgb_led(0, 0, 0); 
      change=1;
    }
    else {
      
      rgb_led(0, 0, 0); 
  }
}
void color_check() { // 색 인식
  int rgb_r=0; //가공된 컬러센서 red값
int rgb_g=0; //가공된 컬러센서 green값
int rgb_b=0; //가공된 컬러센서 blue값
int rgb_c=0; //가공된 컬러센서 밝기값
  digitalWrite(S_LED,HIGH);
  uint16_t r_raw, g_raw, b_raw, sum; // raw value
  float r, g, b; // 0~255 value

  tcs.getRawData(&r_raw, &g_raw, &b_raw, &sum);

  rgb_r = r_raw * 2;
  rgb_g = g_raw * 2;
  rgb_b = b_raw * 2;
  rgb_c = sum * 2;

  }
void get_color() { //어떤 색이 인식됐는지 판단
  
  if (rgb_c > 800 && rgb_c < 4000) { //검정과 흰색이 아닌 경우(rgb_C값을 통해 판단)
    if (rgb_r > rgb_g && rgb_r > rgb_b) { //red 계열 색 분류(red와 purple)
      if (rgb_c > 2500 && rgb_c < 3500 && rgb_r > 2000) {
        //red
        color = 1;
      }
      else if (rgb_c > 1300 && rgb_c < 2000 && rgb_r > 900) {
        //purple
        color = 4;
      }
      else if (rgb_c > 800 && rgb_c < 1300 && rgb_r > 500) {
        //purple
        color = 4;
      }
    }
    else if (rgb_g > rgb_r && rgb_g > rgb_b) { //green 분류
      //green
      color = 2;
    }
    else if (rgb_b > rgb_r && rgb_b > rgb_g) { //blue 분류
      //blue
      color = 3;
    }
    else { //그 이외
      color = 5;
    }
  }
  else { //검정과 흰색
    color = 0;
  }
}



void linetracing(){
 if(digitalRead(R)== HIGH && digitalRead(L) == HIGH)// 두 개 검은색 감지일때 
    {
    dxl_1.setGoalVelocity(DXL_ID_1,110);
    dxl_0.setGoalVelocity(DXL_ID_0, 110);  //직진
    LEDOn();
  }
  
 else if(digitalRead(L) == LOW && digitalRead(R) == LOW){
    if(digitalRead(RR) == LOW && digitalRead(LL) == LOW){
        
  dxl_0.setGoalVelocity(DXL_ID_0, 110);  
  dxl_1.setGoalVelocity(DXL_ID_1, 110);  
  delay(950);
        
  dxl_1.setGoalVelocity(DXL_ID_1, -110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 90도 회전 
  delay(1100);
    }
 }
 else if(digitalRead(L) == LOW && digitalRead(R) == HIGH)// 왼쪽만 흰색 감지일때 오른쪽으로
 {
  if(digitalRead(LL) == LOW&& digitalRead(RR) == HIGH)// 사이드 왼쪽 감지될때
        {
  dxl_0.setGoalVelocity(DXL_ID_0, 110);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);//직진 후 회전
  delay(950);
        
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110);//왼으로 90도 회전  
  delay(1150);
      }else 
    dxl_1.setGoalVelocity(DXL_ID_1, 130); 
    dxl_0.setGoalVelocity(DXL_ID_0, 0);  //라인트레이싱 왼쪽
       }
else if(digitalRead(L) == HIGH && digitalRead(R) == LOW){ 

   if(digitalRead(LL) == HIGH && digitalRead(RR) == LOW){

    dxl_1.setGoalVelocity(DXL_ID_1, 110);
    dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 회전
    delay(350);
    dxl_1.setGoalVelocity(DXL_ID_1, -110);
    dxl_0.setGoalVelocity(DXL_ID_0, 100);//오른쪽으로 회전
    delay(800); 
   }
//
//  dxl_1.setGoalVelocity(DXL_ID_1, 110);
//  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 회전
//  delay(500); 
//  
//  dxl_1.setGoalVelocity(DXL_ID_1, -110);
//  dxl_0.setGoalVelocity(DXL_ID_0, 100);//오른쪽으로 회전
//  delay(200); 
//  
//  dxl_0.setGoalVelocity(DXL_ID_0, -110);
//  dxl_1.setGoalVelocity(DXL_ID_1, -110);//후진
//  delay(800); 
//  }
//  else
    dxl_1.setGoalVelocity(DXL_ID_1,0 ); 
    dxl_0.setGoalVelocity(DXL_ID_0, 130);  //라인트레이싱 오른쪽
         
}


   
}
void linetracing2() {
while(digitalRead(RR) ==LOW &&digitalRead(R) ==LOW){  
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
}
dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(500);
  dxl_1.setGoalVelocity(DXL_ID_1, -110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 90도 회전 
  delay(500);

  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(1500);
   dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110);//오른쪽으로 90도 회전 
  delay(500);

  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(1500);
   dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110);//오른쪽으로 90도 회전 
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(2200);
  dxl_1.setGoalVelocity(DXL_ID_1, -110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 90도 회전 
  delay(1000);
  
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(3000);
    dxl_1.setGoalVelocity(DXL_ID_1, -110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 90도 회전 
  delay(1000);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(1800);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, -110);//오른쪽으로 90도 회전 
  delay(1000);
  dxl_1.setGoalVelocity(DXL_ID_1, 110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//약직진
  delay(3000);
   dxl_1.setGoalVelocity(DXL_ID_1, -110);
  dxl_0.setGoalVelocity(DXL_ID_0, 110);//오른쪽으로 90도 회전 
  delay(1000);
   dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);//약직진
  delay(1500);
  
  change =  2;

  
}
  /*
   if(digitalRead(R)== HIGH && digitalRead(L) == HIGH)// 두 개 검은색 감지일때 
    {
    dxl_1.setGoalVelocity(DXL_ID_1,110);
    dxl_0.setGoalVelocity(DXL_ID_0, 110);  //직진
   
  }
  else if(digitalRead(L) == LOW && digitalRead(R) == HIGH){
   
    dxl_1.setGoalVelocity(DXL_ID_1, 150); 
    dxl_0.setGoalVelocity(DXL_ID_0, 0);  //라인트레이싱 왼쪽
  }
  else if(digitalRead(L) == HIGH && digitalRead(R) == LOW){
     if(digitalRead(LL) == HIGH && digitalRead(RR) == LOW){
    dxl_1.setGoalVelocity(DXL_ID_1,110);
    dxl_0.setGoalVelocity(DXL_ID_0, 110);  //직진
    
     }
    dxl_1.setGoalVelocity(DXL_ID_1,0 ); 
    dxl_0.setGoalVelocity(DXL_ID_0, 150);  //라인트레이싱 오른쪽
}*/
/*
if(color ==0){
  rgb_led(0, 0, 0);
}else if(color ==1){
  rgb_led(255, 0, 0);
}else if(color ==3){
  rgb_led(0, 255, 0);
}
}
*/
