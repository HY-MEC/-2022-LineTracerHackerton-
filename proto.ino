#include <DynamixelShield.h> //DynamixelShield 헤더파일 불러오기

#include "Adafruit_TCS34725.h" //TCS34725 센서헤더파일 불러오기
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) // 우노 또는 메가 보드에서 다이나믹셀 쉴드 사용할 때
#include <SoftwareSerial.h> // SoftwareSerial 헤더파일 불러오기
SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART 통신할 Rx, Tx 핀 설정
#define DEBUG_SERIAL soft_serial // 설정한 soft_serial을 debug serial로 사용
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAM_ZERO) // 두에 또는 제로 보드에서 다이나믹셀 쉴드 사용할 때
#define DEBUG_SERIAL SerialUSB // SerialUSB를 debug serial로 사용
#else
#define DEBUG_SERIAL Serial // 위를 제외한 나머지의 경우 Serial을 debug serial로 사용
#endif

const uint8_t DXL_ID_1 = 1; // 다이나믹셀 아이디 입력(기본적으로 1로 설정되어 있음)
const uint8_t DXL_ID_0 = 0; // 다이나믹셀 아이디2 입력
const float DXL_PROTOCOL_VERSION = 2.0; //XL430 프로토콜 버전 2.0

DynamixelShield dxl_1; // RIGHT
DynamixelShield dxl_0; // LEFT

using namespace ControlTableItem;

#define POT A0
int cds;

int RED_PIN = 40;
int GREEN_PIN = 44;
int BLUE_PIN = 42;

//ir 센서값 저장 변수
int ir_L;
int ir_M;
int ir_R;

//rgb 센서값 저장 변수
int rgb_r, rgb_g, rgb_b, rgb_c;


int count = 0;  //Mode1에서 사용돠는 카운트
int count1 = 0; //Mode2에서 사용되는 카운트
int count2 = 0; //Mode4에서 사용돠는 카운트
int count3 = 0; //Mode5에서 사용돠는 카운트
int count4 = 0; //Mode3에서 사용돠는 카운트
int count5 = 0;


int flag = 0; //교차로 인식 횟수
boolean cross = 0; //교차로 진입 여부
boolean on_color = 0; //색상 테이프 진입 여부
boolean mod3_once = 0; //Mode3에서 사용되는 변수
int mode4_once = 0;
boolean mod5_once = 0;

char color = 'n'; //컬러 센서가 인식한 색상
char color_p = 'n'; //주차 미션에서 사용할 색 지정

int MODE = 0;  //현재 모드(작동 미션) 결정 변수

void setup() {

  pinMode(9, INPUT); // left
  pinMode(10, INPUT);// middle
  pinMode(11, INPUT);// right

  pinMode(13, OUTPUT);

  DEBUG_SERIAL.begin(115200); // 다이나믹셀 쉴드 UART 통신 설정

  dxl_1.begin(57600); // 다이나믹셀 통신 속도랑 포트 통신 속도 일치시키기(설정 가능)
  dxl_0.begin(57600);

  dxl_1.setPortProtocolVersion(DXL_PROTOCOL_VERSION); // 프로토콜 버전 세팅(포트랑 다이나믹셀 일치)
  dxl_0.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  dxl_1.ping(DXL_ID_1); // 설정한 다이나믹셀 동작 가능한지 확인
  dxl_0.ping(DXL_ID_0);

  dxl_1.torqueOff(DXL_ID_1); //초기 모드 설정 전 충돌을 방지하기 위해 전원 OFF
  dxl_1.setOperatingMode(DXL_ID_1, OP_VELOCITY);
  dxl_1.torqueOn(DXL_ID_1); // 모드 설정 후 ON

  dxl_0.torqueOff(DXL_ID_0); //초기 모드 설정 전 충돌을 방지하기 위해 전원 OFF
  dxl_0.setOperatingMode(DXL_ID_0, OP_VELOCITY);
  dxl_0.torqueOn(DXL_ID_0); // 모드 설정 후 ON

  tcs.begin();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

}

void loop() {
  ir_L = digitalRead(9);
  ir_M = digitalRead(10);
  ir_R = digitalRead(11);

  switch (MODE) {
    case 0: line_trace();
      break; //라인트레이싱
    case 1: Mode1();
      break; //미션 구간 1(교차로)
    case 2: Mode2();
      break;  //미션 구간 2(터널)
    case 3:  Mode3();
      break;  //미션 구간 3(차선 변경)
    case 4:  Mode4();
      break;  //미션 구간 4(격자)
    case 5:  Mode5();
      break;  //미션 구간 5(감 가속)
    case 6:  Mode6();
      break;  //미션 구간 6(주차)
    case 7:  vel_S(); delay(1000);
      break;  //정지
  }
}
//미션 함수--------------------------------------------------------------------------------------------------------------------------
void Mode1() { //미션 구간 1(교차로)
  cds = analogRead(POT);

  if (count == 0) {
    vel_M(100);
    delay(500);

    vel_S();
    delay(1000);

    vel_L(100);
    delay(1050);

    vel_S();
    delay(1000);

    vel_M(100);
    delay(1282.05);

    vel_S();
    delay(1000);
    count++;
  }

  else if (count == 1) {
    vel_R(100);
    delay(1050);
    vel_S();
    delay(1000);

    vel_M(100);
    delay(4150);
    vel_S();
    delay(1000);
    count++;
  }
  //좌회전
  else if (count == 2) {

    vel_L(100);
    delay(1100);

    vel_S();
    delay(1000);

    vel_M(100);
    delay(4450);

    vel_R(100);
    delay(1080);

    vel_S();
    delay(1000);
    count++;
  }
  else if (count == 3) {
    line_trace_vel(100);

    if (cds > 450) {
      MODE++;
    }
  }
}
//*******************************************************************************************************************
void Mode2() { //미션 구간 2(터널)
  cds = analogRead(POT);
  color_check();
  get_color();
  if ( cds > 450 ) {
    digitalWrite(40, HIGH);
  }
  if (color == 'n') {
    if (on_color == 1) {
      on_color = 0;
      count1++;
    }
  }
  else {
    if (on_color == 0) {
      on_color = 1;
    }
  }

  if (count1 == 0) {
    line_trace_vel(100);
  }
  else if (count1 == 1) {
    digitalWrite(40, LOW);
    vel_S();
    delay(300);
    MODE++;
  }
}
//*******************************************************************************************************************
void Mode3() { //미션 구간 3(차선 변경)
  color_check();
  get_color();
  if (color == 'n') {
    if (on_color == 1) {
      on_color = 0;
      mod3_once = 0;
      count4++;
    }
  }
  else {
    if (on_color == 0) {
      on_color = 1;
    }
  }

  if (count4 == 0) {
    if (mod3_once == 0) {
      vel_S();
      //digitalWrite(40, 1);
      delay(200);
      vel_M(100);
      delay(100);
      vel_R(100);
      delay(200);
      mod3_once = 1;
    }
    line_trace_vel(100);
  }
  else if (count4 == 1) {
    if (mod3_once == 0) {
      vel_S();
      //digitalWrite(40, 0);
      delay(500);
      vel_L(100);
      delay(850);
      vel_M(100);
      delay(2400);
      vel_R(100);
      delay(800);
      vel_S();
      delay(500);
      mod3_once = 1;
    }
    line_trace_vel(100);
  }
  else if (count4 == 2) {
    if (mod3_once == 0) {
      vel_S();
      //digitalWrite(40, 1);
      delay(500);
      vel_R(100);
      delay(800);
      vel_M(100);
      delay(2400);
      vel_L(100);
      delay(800);
      vel_S();
      delay(500);
      mod3_once = 1;
    }
    line_trace_velog(100);   // 변경 필요
  }
  else if (count4 == 3) {
    vel_S();
    //digitalWrite(40, 0);
    delay(300);
    MODE++;
  }
}
//*******************************************************************************************************************
void Mode4() { // 우회전 -> 직진 -> 좌회전 -> 직진 -> 우회전 -> 직진 -> 좌회전 -> 직진 -> 좌회전 -> 직진 -> 좌회전 -> 직진 -> 우회전 -> 직진 -> 우회전 -> 직진 -> 라인 트레이싱
  if (count2 == 0) {
    if (mode4_once == 0) {
      vel_M(40);
      delay(1000);
      vel_R(60);
      delay(1000);
      mode4_once++;
    }
    else {
      if (ir_L == LOW && ir_M == LOW && ir_R == LOW) {
        count2++;
      }
      else {
        line_trace_vel(100);
      }
    }
  }
  if (count2 == 1) {
    vel_S();
    delay(200);
    vel_M(100);
    delay(900);
    vel_R(100);
    delay(1050);
    vel_S();
    delay(100);
    vel_MM(4200); // 36.5cm
    digitalWrite(44, HIGH);
    vel_L(100);
    delay(1100);
    digitalWrite(44, LOW);
    vel_MM(2310); // 18cm
    vel_R(100);
    delay(1050);
    vel_MM(4680); // 36.5cm
    vel_L(100);
    delay(1100);
    vel_MM(2308); // 53.5cm
    digitalWrite(40, HIGH);
    vel_S();
    delay(2000);
    digitalWrite(40, LOW);
    vel_MM(4552); // 53.5cm
    vel_L(100);
    delay(1100);
    vel_MM(6860); //53.5cm
    vel_L(100);
    delay(1100);
    vel_MM(4680); // 36.5cm
    vel_R(100);
    delay(1050);
    vel_MM(2360); // 18cm
    vel_S();
    delay(1000);
    vel_R(100);
    delay(1100);
    vel_MM(4500); // 20cm
    vel_S();
    delay(1000);
    count2++;
  }
  if (count2 == 2) {
    if (count5 == 0) {
      ir_L = digitalRead(9);
      ir_M = digitalRead(10);
      ir_R = digitalRead(11);

      if (ir_L == LOW && ir_M == LOW && ir_R == LOW) {
        vel_R(20);
        delay(1500);
        count5++;
      }
      else {
        line_trace_vel(100);
      }
    }
    else if (count5 == 1) {
      MODE++;
      vel_S();
      delay(1000);
    }
  }
}
//*******************************************************************************************************************
void Mode5() {
  color_check();
  get_color();
  if (color == 'n') {
    if (on_color == 1) {
      on_color = 0;
      mod5_once = 0;
      count3++;
    }
  }
  else {
    if (on_color == 0) {
      on_color = 1;
      if (count3 == 5) {
        color_p = color;
      }
    }
  }

  if (count3 == 0 || count3 == 1) {
    line_trace_vel(100);
    //digitalWrite(40, 0);
  }
  else if (count3 == 2) {
    line_trace_vel(80);
    //digitalWrite(40, 1);
  }
  else if (count3 == 3) {
    line_trace_vel(100);
    //digitalWrite(40, 0);
  }
  else if (count3 == 4) {
    if (mod5_once == 0) { //초록색에서 가속시 라인 이탈하여 일단 좌회전 시킴
      vel_S();
      delay(500);
      vel_L(100);
      delay(400);
      mod5_once = 1;
    }
    line_trace_vel(120);
    //digitalWrite(40, 1);
  }
  else if (count3 == 5) {
    line_trace_vel(100);
    //digitalWrite(40, 0);
  }
  else if (count3 == 6) {
    vel_S();
    delay(500);
//    vel_L(100);
//    delay(450);
    vel_S();
    delay(1000);
    MODE = 6;
    //digitalWrite(40, 1);
  }
}
//*******************************************************************************************************************
void Mode6() { // parking
  switch (color_p) {
    case 'r':
    case 'p':
    case 'a':
      digitalWrite(40, HIGH);
      vel_M(100);
      delay(1923);
      vel_L(100);
      delay(1100);
      vel_M(100);
      delay(2564);
      vel_R(100);
      delay(1050);
      vel_M(100);
      delay(4600);
      MODE = 7;
      break;
    case 'g':
      digitalWrite(44, HIGH);
      vel_M(100);
      delay(7307.4);
      MODE = 7;
      break;
    case 'b':
      digitalWrite(42, HIGH);
      vel_M(100);
      delay(1923);
      vel_R(100);
      delay(1050);
      vel_M(100);
      delay(2564);
      vel_L(100);
      delay(1100);
      vel_M(100);
      delay(4600);
      MODE = 7;
      break;
  }
}

//컬러 인식--------------------------------------------------------------------------------------------------------------------------
void color_check() { // 색 인식

  uint16_t r_raw, g_raw, b_raw, sum; // raw value
  float r, g, b; // 0~255 value

  tcs.getRawData(&r_raw, &g_raw, &b_raw, &sum);

  rgb_r = r_raw * 2;
  rgb_g = g_raw * 2;
  rgb_b = b_raw * 2;
  rgb_c = sum * 2;
}

void get_color() { //어떤 색이 인식됐는지 판단
  if (rgb_c > 800 && rgb_c < 4000) {
    if (rgb_r > rgb_g && rgb_r > rgb_b) {
      if (rgb_c > 2500 && rgb_c < 3500 && rgb_r > 2000) {
        //red
        color = 'r';
      }
      else if (rgb_c > 1300 && rgb_c < 2000 && rgb_r > 900) {
        //purple
        color = 'p';
      }
      else if (rgb_c > 800 && rgb_c < 1300 && rgb_r > 500) {
        //purple
        color = 'a';
      }
    }
    else if (rgb_g > rgb_r && rgb_g > rgb_b) {
      //green
      color = 'g';
    }
    else if (rgb_b > rgb_r && rgb_b > rgb_g) {
      //blue
      color = 'b';
    }
    else {
      color = 'n';
    }
  }
  else {
    color = 'n';
  }
}
//라인 트레이싱--------------------------------------------------------------------------------------------------------------------------
void line_trace() { // line tracing-mode0(교차로 인식)
  if (flag == 2) {
    vel_R(100);
    delay(800);
    flag = 3;
    MODE = 1;
  }
  else {
    if (ir_L == LOW && ir_M == LOW && ir_R == HIGH || ir_L == LOW && ir_M == HIGH && ir_R == HIGH) {
      vel_SL(100);
    }
    else if (ir_L == HIGH && ir_M == LOW && ir_R == HIGH || ir_L == HIGH && ir_M == HIGH && ir_R == HIGH || ir_L == LOW && ir_M == LOW && ir_R == LOW) {
      vel_M(100);
      if (cross == 0 && ir_L == LOW && ir_M == LOW && ir_R == LOW) { //교차로 진입
        if (flag == 0) {
          cross = 1;
        }
        else if (flag == 1) {
          vel_R(100);
          delay(200);
          vel_M(100);
          delay(100);
          flag = 3;
          MODE = 1;
        }
      }
      else if (cross == 1 && ir_L == HIGH && ir_R == HIGH) { //교차로 탈출
        cross = 0;
        flag++;  //교차로 통과 횟수 1 증가
      }
    }
    else if (ir_L == HIGH && ir_M == LOW && ir_R == LOW || ir_L == HIGH && ir_M == HIGH && ir_R == LOW) {
      vel_SR(100);
    }
  }
}
void line_trace_vel(int a) { // line tracing with velocity setting
  if (ir_L == LOW && ir_M == LOW && ir_R == HIGH || ir_L == LOW && ir_M == HIGH && ir_R == HIGH) {
    vel_SL(a);
  }
  else if (ir_L == HIGH && ir_M == LOW && ir_R == HIGH || ir_L == HIGH && ir_M == HIGH && ir_R == HIGH || ir_L == LOW && ir_M == LOW && ir_R == LOW) {
    vel_M(a);
  }
  else if (ir_L == HIGH && ir_M == LOW && ir_R == LOW || ir_L == HIGH && ir_M == HIGH && ir_R == LOW) {
    vel_SR(a);
  }
}
void line_trace_velog(int a) { // line tracing with velocity setting
  if (ir_L == LOW && ir_M == LOW && ir_R == HIGH || ir_L == LOW && ir_M == HIGH && ir_R == HIGH) {
    vel_SLL(a);
  }
  else if (ir_L == HIGH && ir_M == LOW && ir_R == HIGH || ir_L == HIGH && ir_M == HIGH && ir_R == HIGH || ir_L == LOW && ir_M == LOW && ir_R == LOW) {
    vel_M(a);
  }
  else if (ir_L == HIGH && ir_M == LOW && ir_R == LOW || ir_L == HIGH && ir_M == HIGH && ir_R == LOW) {
    vel_SRR(a);
  }
}
//모터 제어--------------------------------------------------------------------------------------------------------------------------
void vel_L(int goal) { // 좌회전
  dxl_1.setGoalVelocity(DXL_ID_1, -goal);  // 양수 후진
  dxl_0.setGoalVelocity(DXL_ID_0, -goal); // 양수 전진
}
void vel_M(int goal) { // 직진
  dxl_1.setGoalVelocity(DXL_ID_1, -goal);
  dxl_0.setGoalVelocity(DXL_ID_0, goal);
}
void vel_R(int goal) { // 우회전
  dxl_1.setGoalVelocity(DXL_ID_1, goal);
  dxl_0.setGoalVelocity(DXL_ID_0, goal);
}

void vel_S() {  //정지
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
}

void vel_SL(int goal) { // soft 좌회전
  dxl_1.setGoalVelocity(DXL_ID_1, -goal);
  dxl_0.setGoalVelocity(DXL_ID_0, 0);
}

void vel_SLL(int goal) { // soft 좌회전
  dxl_1.setGoalVelocity(DXL_ID_1, -goal);
  dxl_0.setGoalVelocity(DXL_ID_0, -20);
}

void vel_SR(int goal) { // soft 우회전
  dxl_1.setGoalVelocity(DXL_ID_1, 0);
  dxl_0.setGoalVelocity(DXL_ID_0, goal);
}

void vel_SRR(int goal) { // soft 우회전
  dxl_1.setGoalVelocity(DXL_ID_1, 20);
  dxl_0.setGoalVelocity(DXL_ID_0, goal);
}

void vel_MM(int time_close) { // 직진 wiith delay
  dxl_1.setGoalVelocity(DXL_ID_1, -100);
  dxl_0.setGoalVelocity(DXL_ID_0, 100);
  delay(time_close);
}
