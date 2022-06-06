#include <TimerFive.h>
#include <DynamixelShield.h>
#include <Wire.h>  //I2C 통신을 위해 wire.h 라이브러리 사용 선언
#include <Adafruit_TCS34725.h>  //컬러 센서 사용을 위해 컬러 센서 전용 라이브러리 사용 선언
#define sw0 A0                                  //3 2 1 0 
#define sw1 A1
#define sw2 A2
#define sw3 A3
#define jodo A4
#define l_red 11
#define l_green 12
#define l_blue 13
int shadow;
int irin0;
int irin1;
int irin2;
int irin3;
int incnt;
int task1 = 0;              //어디서부터할지 시작 고르는 곳 *******중요*******
int task2 = 0;
int stock = 0;              //50ms 이상 big cicle
int first = 0;              //미션 진행상황 저장 변수
bool t5_flag = 0;
long remember0;
long remember1;
int nomore = 0;
float red, green, blue;
unsigned int t5_index = 0;  //switch문을 쓰기위한 변수  //50ms 이하 small cycle

void T5ISR() {
  t5_flag = true;                               //T5 인터럽트가 발동하면 t5_flag의 값을 참으로 만듭니다
}

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

DynamixelShield dxl_1;
DynamixelShield dxl_0;


// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];
int originshadow;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X);

void setup() {
  pinMode(sw0, INPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  pinMode(jodo, INPUT);
  pinMode(l_red, OUTPUT);
  pinMode(l_green, OUTPUT);
  pinMode(l_blue, OUTPUT);
  originshadow = analogRead(jodo);
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);
  Serial.setTimeout(30);
  digitalWrite(2,HIGH);
  if (tcs.begin()) {
    DEBUG_SERIAL.println("Found sensor");
  }
  else {
    DEBUG_SERIAL.println("No TCS34725 found... check your connections");
    while (1);
  }
  tcs.write8(TCS34725_PERS, TCS34725_PERS_2_CYCLE);

  dxl_1.setGoalVelocity(1, 100); //use default raw value
  dxl_0.setGoalVelocity(0, 100); //use default raw value


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

  //tcs.setInterrupt(false);
  Timer5.initialize(10000);                     //타이머5 10msec 마다
  Timer5.attachInterrupt(T5ISR);                //T5ISR 실행
  

  // use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
  ledcAttachPin(redpin, 1);
  ledcSetup(1, 12000, 8);
  ledcAttachPin(greenpin, 2);
  ledcSetup(2, 12000, 8);
  ledcAttachPin(bluepin, 3);
  ledcSetup(3, 12000, 8);
#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
  delay(1000);
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to 
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.

void loop() {       //메인 루프 함수
  // put your main code here, to run repeatedly:

  // Please refer to e-Manual(http://emanual.robotis.com) for available range of value.
  // Set Goal Velocity using RAW unit
  if (t5_flag) {                                //10ms마다 실행
    t5_flag = false;                            //t5_flag를 false로 초기화
    switch (task1) {
      case 0: sttart(); break;  //10ms마다 여기서 이 함수 호출!
      case 1: go1(); break;
      case 2: left_1(); break;
      case 3: right_1(); break;
      case 4: go2(); break;
      case 5: colorcheck(); break;
      case 6: right45(); break;
      case 7: redcross(); break;
      case 8: bluecross(); break;
      case 9: right_over(); break;
      case 10: right45_over(); break;
      case 11: go3(); break;
      case 12: left_2(); break;
      case 13: right_2(); break;
      case 14: pass_2(); break;
      
      
      default:
        break;
    }
  }
}

void sttart() {       //0번 -> 1번//2초(2000ms) 대기 후 50ms마다 하나의 작업 반복
  if (stock++ < 100) {}
  else {                //stock>=200
    if (stock > 300) {
      stock = 0;
      //탈출 코드 입력
      task1 = 1;
    }
    else if (stock > 200) {
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else {
      dxl_0.setGoalVelocity(DXL_ID_0, -70);
      dxl_1.setGoalVelocity(DXL_ID_1, 70);
    }
  }
}

void go1() {         //1번 ->
  switch (++t5_index) {                         //t5_index에 1더한 값을 저장
    case 0:                                   //t5_index가 0이면
      break;                                  //switch문 탈출
    case 1: //10msec                          //t5_index가 1이면   //ir 센서 입력 받고 어떤 명령을 수행할지 결정하는 곳//보통 준비작업 수행
      irin0 = analogRead(sw0);                //나란한 방향으로 봤을때
      irin1 = analogRead(sw1);
      irin2 = analogRead(sw2);
      break;                                  //switch문 탈출
    case 2: //20msec                          //t5_index가 2이면   //여기서 특수 함수 호출
      if (irin2 < 80) {
        dxl_0.setGoalVelocity(DXL_ID_0, 0);
        dxl_1.setGoalVelocity(DXL_ID_1, 0);
        switch (++first) {
          case 1:
            task1 = 3;
            break;
          case 2:
            task1 = 2;
            break;
          case 3:
            task1 = 2;
            break;
          case 4:
            task1 = 3;
            break;
          case 5:
            task1 = 2;
            break;
          case 6:
            task1 = 3;
            break;
          case 7:
            task1 = 4;
            first = 0;
            break;
        }
      }
      else {
        if (irin0 < 80) {
          dxl_0.setGoalVelocity(DXL_ID_0, 0);
          dxl_1.setGoalVelocity(DXL_ID_1, 110);
        }
        else if (irin1 < 80) {
          dxl_0.setGoalVelocity(DXL_ID_0, -110);
          dxl_1.setGoalVelocity(DXL_ID_1, 0);
        }
        else {
          dxl_0.setGoalVelocity(DXL_ID_0, -110);
          dxl_1.setGoalVelocity(DXL_ID_1, 110);
        }
      }
      break;                                //switch문 탈출
    case 3: //30msec                        //t5_index가 3이면     //rgb 입력 받고 해당 특수 명령을 수행할지 말지 결정하는곳 해야하면 여기서 함수 호출
      break;                                //switch문 탈출
    case 4: //40msec                        //t5_index가 4이면
      break;                                //switch문 탈출
    case 5: //50msec                        //t5_index가 5이면
      t5_index = 0;                               //t5_index 초기화(계속 반복하기위해)
      break;                                      //switch문 탈출
    default:                                      //t5_index에 0,1,2,3,4,5외의 값이 입력되면
      break;                                      //switch문 탈출
  }
}

void left_1() {                                   //2번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 470){
      stock = 0;
      task1 = 1;
    }
    else if(stock > 370){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 355){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 255){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 175){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else if(stock > 75){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void right_1() {                                   //3번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 470){
      stock = 0;
      task1 = 1;
    }
    else if(stock > 370){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 355){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 255){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 175){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 75){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void go2(){     //4번코드 -> 5번 갈림길45도 6번 crossred 7번 crossblue
  switch (++t5_index) {                         //t5_index에 1더한 값을 저장
    case 0:                                   //t5_index가 0이면
      break;                                  //switch문 탈출
    case 1: //10msec                          //t5_index가 1이면   //ir 센서 입력 받고 어떤 명령을 수행할지 결정하는 곳//보통 준비작업 수행
      irin0 = analogRead(sw0);                //나란한 방향으로 봤을때
      irin1 = analogRead(sw1);
      irin2 = analogRead(sw2);
      irin3 = analogRead(sw3);
      break;                                  //switch문 탈출
    case 2: //20msec                          //t5_index가 2이면   //여기서 특수 함수 호출
      if (irin0 < 80) {
        dxl_0.setGoalVelocity(DXL_ID_0, 0);
        dxl_1.setGoalVelocity(DXL_ID_1, 110);
      }
      else if (irin1 < 80) {
        dxl_0.setGoalVelocity(DXL_ID_0, -110);
        dxl_1.setGoalVelocity(DXL_ID_1, 0);
      }
      else {
        dxl_0.setGoalVelocity(DXL_ID_0, -110);
        dxl_1.setGoalVelocity(DXL_ID_1, 110);
      }
      break;                                //switch문 탈출
    case 3: //30msec                        //t5_index가 3이면     //rgb 입력 받고 해당 특수 명령을 수행할지 말지 결정하는곳 해야하면 여기서 함수 호출
      if(irin3<20){
        Serial.println(irin3);
        dxl_0.setGoalVelocity(DXL_ID_0, -110);
        dxl_1.setGoalVelocity(DXL_ID_1, 110);
        sensor();
        task1 = 5;
      }
      break;                                //switch문 탈출
    case 4: //40msec                        //t5_index가 4이면
      shadow = analogRead(jodo);
      Serial.println(shadow);
      if(originshadow - shadow > 10 || originshadow - shadow < -10 || shadow < 770){
        digitalWrite(l_red,HIGH);
        digitalWrite(l_green,HIGH);
        digitalWrite(l_blue,HIGH);
      }
      else {
        digitalWrite(l_red,LOW);
        digitalWrite(l_green,LOW);
        digitalWrite(l_blue,LOW);
      }
      break;                                //switch문 탈출
    case 5: //50msec                        //t5_index가 5이면
      t5_index = 0;                               //t5_index 초기화(계속 반복하기위해)
      break;                                      //switch문 탈출
    default:                                      //t5_index에 0,1,2,3,4,5외의 값이 입력되면
      break;                                      //switch문 탈출
  }
}

void colorcheck(){
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 120){
      stock = 0;
      switch(++first){
        case 1: task1 = 6; break;
        case 2: task1 = 7;break;
        case 3: task1 = 8; 
        case 4: task1 = 9; 
        case 5: task1 = 10; first = 0; break;
        default: break;
      }
    }
    else if (stock > 20) {
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
      sensor();
    } //10ms -> 50ms 변환
  }
}

void right45() {               //5번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock >390){
      stock = 0;
      task1 = 4;
    }
    else if(stock > 290){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 250){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 150){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void redcross() {               //6번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, -110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 680){
      stock = 0;
      task1 = 4;
    }
    else if(stock > 580){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 490){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 390){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 210){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 110){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void bluecross() {               //7번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, 110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 645){
      stock = 0;
      task1 = 9;
    }
    else if(stock > 545){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 460){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else if(stock > 360){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 180){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 80){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}


void right_over() {                                   //9번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 470){
      stock = 0;
      task1 = 4;
    }
    else if(stock > 370){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 355){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 255){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 175){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 75){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void right45_over() {               //10번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock >390){
      stock = 0;
      task1 = 11;
    }
    else if(stock > 290){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 250){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 150){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void go3() {         //11번 
  switch (++t5_index) {                         //t5_index에 1더한 값을 저장
    case 0:                                   //t5_index가 0이면
      break;                                  //switch문 탈출
    case 1: //10msec                          //t5_index가 1이면   //ir 센서 입력 받고 어떤 명령을 수행할지 결정하는 곳//보통 준비작업 수행
      irin0 = analogRead(sw0);                //나란한 방향으로 봤을때
      irin1 = analogRead(sw1);
      irin2 = analogRead(sw2);
      break;                                  //switch문 탈출
    case 2: //20msec                          //t5_index가 2이면   //여기서 특수 함수 호출
      if (irin2 < 80) {
        dxl_0.setGoalVelocity(DXL_ID_0, 0);
        dxl_1.setGoalVelocity(DXL_ID_1, 0);
        switch (++first) {
          case 1:
            task1 = 13; //go2에서 오른쪽 회전
            break;
          case 2:
            task1 = 14; //pass
            break;
          case 3:
            task1 = 12; //왼쪽으로 냅다 돌기
            break;
          case 4:
            task1 = 13; // 오른쪽 회전
            break;
          case 5:
            task1 = 14; //pass
            break;
          case 6:
            task1 = 12; //왼쪽 회전
            break;
          case 7:
            task1 = 14; //pass
            break;
          case 8:
            task1 = 14; //pass
            break;
          case 9:
            task1 = 12; //왼쪽 회전
            break; 
          case 10:
            task1 = 14; //pass
            break;
          case 11:
            task1 = 14; //pass
            break;
          case 12:
            task1 = 12; //왼쪽 회전
            break;
          case 13:
            task1 = 14; //pass
            break; 
          case 14:
            task1 = 13; //오른쪽
            break;  
          case 15:
            task1 = 13;
          case 16 :
            task1 = 14;
            first = 15;
            break;
        }
      }
      else {
        if (irin0 < 80) {
          dxl_0.setGoalVelocity(DXL_ID_0, 0);
          dxl_1.setGoalVelocity(DXL_ID_1, 110);
        }
        else if (irin1 < 80) {
          dxl_0.setGoalVelocity(DXL_ID_0, -110);
          dxl_1.setGoalVelocity(DXL_ID_1, 0);
        }
        else {
          dxl_0.setGoalVelocity(DXL_ID_0, -110);
          dxl_1.setGoalVelocity(DXL_ID_1, 110);
        }
      }
      break;                                //switch문 탈출
    case 3: //30msec                        //t5_index가 3이면     //rgb 입력 받고 해당 특수 명령을 수행할지 말지 결정하는곳 해야하면 여기서 함수 호출
      break;                                //switch문 탈출
    case 4: //40msec                        //t5_index가 4이면
      break;                                //switch문 탈출
    case 5: //50msec                        //t5_index가 5이면
      t5_index = 0;                               //t5_index 초기화(계속 반복하기위해)
      break;                                      //switch문 탈출
    default:                                      //t5_index에 0,1,2,3,4,5외의 값이 입력되면
      break;                                      //switch문 탈출
  }
}

void left_2() {                                   //12번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 470){
      stock = 0;
      task1 = 11;
    }
    else if(stock > 370){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 355){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 255){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 175){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, -110);
    }
    else if(stock > 75){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void right_2() {                                   //13번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 470){
      stock = 0;
      task1 = 11;
    }
    else if(stock > 370){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 355){
      dxl_0.setGoalVelocity(DXL_ID_0, -110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 255){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
    else if(stock > 175){
      dxl_0.setGoalVelocity(DXL_ID_0, 110);
      dxl_1.setGoalVelocity(DXL_ID_1, 110);
    }
    else if(stock > 75){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void pass_2() {                                   //14//번
  if (stock++ < 5) {
    dxl_0.setGoalVelocity(DXL_ID_0, -110);
    dxl_1.setGoalVelocity(DXL_ID_1, 110);
  } //10ms -> 50ms 변환
  else {
    if(stock > 150){
      stock = 0;
      task1 = 11;    //다음 주행 뭐지
    }
    else if(stock > 50){
      dxl_0.setGoalVelocity(DXL_ID_0, 0);
      dxl_1.setGoalVelocity(DXL_ID_1, 0);
    }
  }
}

void sensor(){
  
  tcs.setInterrupt(false);  // turn on LED

  //delay(5);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED
  Serial.println(irin3);
  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));

//  Serial.print("\t");
//  Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
  Serial.print("\n");

//  uint16_t red, green, blue, clear;
//  
//  tcs.setInterrupt(false);  // turn on LED
//
//  delay(60);  // takes 50ms to read
//
//  tcs.getRawData(&red, &green, &blue, &clear);
//  
//  tcs.setInterrupt(true);  // turn off LED
//
//  Serial.print("C:\t"); Serial.print(int(clear)); 
//  Serial.print("R:\t"); Serial.print(int(red)); 
//  Serial.print("\tG:\t"); Serial.print(int(green)); 
//  Serial.print("\tB:\t"); Serial.print(int(blue));
//  Serial.println();


#if defined(ARDUINO_ARCH_ESP32)
  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);
#else
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);
#endif
}
