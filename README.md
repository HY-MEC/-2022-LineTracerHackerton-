![KakaoTalk_20220521_172354414_13](https://user-images.githubusercontent.com/96982279/172133085-859a43b9-c2b6-4bc2-945f-2a20405d2947.jpg)
# -2022년 라트톤-
### 2022년도 라트톤에서 사용된 맵과 점수표 테스트에서 사용한 테스트 코드와 영상, 1위 팀의 코드와 영상, 그리고 2위, 3위 팀의 코드 입니다.

* * *

## <2022년 라트톤 맵&점수표>
![KakaoTalk_20220521_172354414_13](https://user-images.githubusercontent.com/96982279/172140711-874e14fe-bd33-4a65-9770-497e26a4fb44.jpg)
![점수표_1](https://user-images.githubusercontent.com/96982279/171007370-79298aa6-de3d-4f94-a18a-433ee284f8fa.png)
![점수표_2](https://user-images.githubusercontent.com/96982279/171007390-6ab71bb8-0fff-40bd-808d-907cd385c8d1.png)

 구간 공통 조건: 모든 구간의 기본 주행 속도는 다이나믹셀의 PWM제어 기준 최소 110 입니다. (어느 한 바퀴만 110 이상이면 인정)

 1. 라인트레이싱 구간: 기본적인 라인트레이싱 구간 입니다. 교차로가 여럿 존재하기 때문에 일반적인 라인트레이싱 주행 만으로는 해당 구간을 빠져나가기 어려울수도 있습니다, 때문에 교차점을 인식하여 원하는 교차점에서 원하는 방향으로 회전할 수 있도록 하는 등의 스킬이 필요합니다.

 2. 터널 구간: 터널 진입시 LED를 On하고, 터널 빠져나오면 LED를 OFF하는 구간입니다. 조도 센서를 활용하여 센서 값에 따라 LED를 ON/OFF할 수 있도록 합니다. 대회 시연시 공간의 조도 상황에 따라 값이 변할 수 있기 때문에 항상 시연전 조도 센서 값 확인이 중요합니다.

 3. 차선 변경 구간: 바닥에 붙은 컬러 테이프를 인식하여 색에 맞는 방향으로 차선 변경을 하는 구간입니다. RGB 센서를 활용하여 빨강과 파랑을 구분하고 파란색에서는 오른쪽으로, 빨간색에서는 왼쪽으로 차선 변뎡이 이루어지도록 합니다.

 4. 라인트레이싱 및 속도 변화 구간: 기본적인 라인트레이싱과 더불어 바닥에 붙은 컬러 테이프를 인식하여 라인트레이서의 주행 속도를 조절해야 하는 구간입니다. 빨간색 사이에서는 기본 속도보다 20 이상 느린 속도로 주행해야 하며, 초록색 사이에서는 기본 속도보다 20 이상 빠른 속도로 주행해야 합니다. 해당 구간은 라인트레이싱 가능 유부와 주행 속도 변화를 각각 따로 점수측정하기 때문에 속도 변화에 자신 없다면 라인트레이싱 만이라도 잘 수행할 수 있게끔 하는것도 좋은 방법 입니다.

 5. 주차 구간: 라인 끝에 붙은 컬러 테이프의 색에 맞는 구간으로 라인트레이서를 주차시키는 구간 입니다. 라인 끝에 붙여질 컬러는 빨강, 파랑, 초록 셋중 하나이며, 대회 시연 10분 직전에 각 조별로 랜덤으로 부여됩니다. 해당 미션은 RGB 센서를 통한 빨강, 파랑, 초록의 컬러 인식이 필요하며, 또한 각 주차 구간까지의 길에는 라인이 존재하지 않기 때문에 delay 등의 방법을 통해 로봇이 해당 주차구간으로 진입할 수 있는 경로를 코딩해주어야 합니다.

 6. 보너스 구간(* 구간): 기본 점수 100점 외에 추가적인 점수를 획득할 수 있는 보너스 구간 입니다. 보너스 구간인 만큼, 격자로 이루어진 오른쪽 길이 아닌 왼쪽의 일직선 길을 주행해도 감점은 존재하지 않습니다. 격자로 구성된 길에는 추가 점수 획득을 위한 다양한 미션이 존재하며, -점수 또한 존재하니 주의해야 합니다.
+ ▲: 해당 영역 통과시 -2.5점(최초 1회만 인정)
+ ●: 해당 영역 통과시 +2.5점(최초 1회만 인정)
+ 초록색＋: 초록색 인식 후 1~3초 동안 LED ON, 이후 LED OFF -> +5점(최초 1회만 인정)
+ 빨간색＋: 빨간색 인식 후 3초동안 정지 -> +5점(최초 1회만 인정)


* * *

## <2022년 라트톤 사전 테스트 영상>
https://user-images.githubusercontent.com/96982279/172136958-212015d4-4b5a-41d1-8536-de0d9e719d89.mp4
#### 해당 영상은 2022 라트톤 진행 전, 미션 구간의 난이도 및 세부 조건 설정을 위해 소프트웨어 팀원 분들께서 테스트 주행을 진행했던 영상 입니다.

* * *

## <2022년 라트톤 1위팀 영상>
https://user-images.githubusercontent.com/96982279/172143944-cc1461ab-9181-444e-9dea-45ef224435ba.mp4
#### 해당 영상은 2022 라트톤 1위 팀이었던 1조의 주행 영상 입니다.

* * *

## <2022년 라트톤 주행 코드들>
### 해당 코드들은 라트톤에 참가해주신 분들의 노력의 결과물 입니다. 코드를 기꺼이 공유해주신 모든 분들께 감사의 인사 드립니다.
|파일명|설명|
|------|---|
|[proto.ino](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/05e925f687472f6b2ad97878da25915718447db3/proto.ino)|사전 테스트에서 사용된 코드 입니다.|
|[team1.ino](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/05e925f687472f6b2ad97878da25915718447db3/team1.ino)|2022 라트톤 1위팀인 1조의 코드 입니다.|
|[team7.zip](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/cd0ebf64d790fe56d2e4a1b81445fa3f211bdbb6/team7.zip)|2022 라트톤 2위팀인 7조의 코드 입니다.|
|[team11.zip](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/cd0ebf64d790fe56d2e4a1b81445fa3f211bdbb6/team11.zip)|2022 라트톤 2위팀인 11조의 코드 입니다.|
|[team13.ino](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/05e925f687472f6b2ad97878da25915718447db3/team13.ino)|2022 라트톤 3위팀인 13조의 코드 입니다.|
|[team14.ino](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/05e925f687472f6b2ad97878da25915718447db3/team14.ino)|2022 라트톤 3위팀인 14조의 코드 입니다.|
|[team5.ino](https://github.com/HY-MEC/-2022-LineTracerHackerton-/blob/05e925f687472f6b2ad97878da25915718447db3/team5.ino)|2022 라트톤 3위팀인 5조의 코드 입니다.|

