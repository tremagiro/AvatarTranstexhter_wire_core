#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire);

#define SPEED 230
#define STEP 123

void setup() {
  // put your setup code here, to run once:
  Wire.setSDA(16);
  Wire.setSCL(17);
  Serial.begin(115200);
  myAvatar.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 値送信
  myAvatar.setStepperInfo(SPEED, STEP);
  Serial.printf("【送信値】speed : %d, step : %d \n", SPEED, STEP);
  delay(1000);
  // 値受信
  int sp;
  int st;
  myAvatar.getStepperInfo(&sp, &st);
  Serial.printf("【受信値】speed : %d, step : %d \n", sp, st);
  delay(1000);
  // 動作モード
  mode_stepper mode;
  myAvatar.setStepperMode(DUAL);
  delay(500);
  myAvatar.getStepperMode(&mode);
  Serial.printf("【送信値】mode stepper : %d\n", mode);
  delay(500);
  myAvatar.setStepperMode(A);
  delay(500);
  myAvatar.getStepperMode(&mode);
  Serial.printf("【送信値】mode stepper : %d\n", mode);
  delay(500);
  myAvatar.setStepperMode(B);
  delay(500);
  myAvatar.getStepperMode(&mode);
  Serial.printf("【送信値】mode stepper : %d\n", mode);
  delay(500);
}
