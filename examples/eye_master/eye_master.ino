#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire);

#define SERVO_L 80
#define SERVO_R 100
#define PUPIL_L 1
#define PUPIL_R 2
#define EYELID_L 0
#define EYELID_R 1
#define BLINK_TIME 3000
#define BRIGHTNESS 128

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myAvatar.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 値送信
  myAvatar.setEyeServoInfo(SERVO_L, SERVO_R);
  myAvatar.setEyePupilInfo(PUPIL_L, PUPIL_R);
  myAvatar.setEyelidInfo(EYELID_L, EYELID_R);
  myAvatar.setEyeBlinkTime(BLINK_TIME);
  myAvatar.setEyeBrightness(BRIGHTNESS);
  // ボードによってはprintfで一度に出せる文字数が少ないため、行を分けて表示する
  Serial.printf("【送信値】servo : %d / %d, pupil : %d / %d\n", SERVO_L, SERVO_R, PUPIL_L, PUPIL_R);
  Serial.printf("        eyelid : %d / %d, blinkTime : %d, brightness : %d\n", EYELID_L, EYELID_R, BLINK_TIME, BRIGHTNESS);
  delay(1000);
  // 値受信
  int sl, sr, pl, pr, el, er, bt, br;
  myAvatar.getEyeServoInfo(&sl, &sr);
  myAvatar.getEyePupilInfo(&pl, &pr);
  myAvatar.getEyelidInfo(&el, &er);
  myAvatar.getEyeBlinkTime(&bt);
  myAvatar.getEyeBrightness(&br);
  Serial.printf("【受信値】servo : %d / %d, pupil : %d / %d\n", sl, sr, pl, pr);
  Serial.printf("       eyelid : %d / %d, blinkTime : %d, brightness : %d\n", el, er, bt, br);
  delay(1000);
}
