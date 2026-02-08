#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire);

#define SPEED_L 67
#define SPEED_R -38

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myAvatar.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 値送信
  myAvatar.setDcMotorInfo(SPEED_L, SPEED_R);
  Serial.printf("【送信値】speed_L : %d, speed_R : %d \n", SPEED_L, SPEED_R);
  delay(1000);
  // 値受信
  int spL;
  int spR;
  myAvatar.getDcMotorInfo(&spL, &spR);
  Serial.printf("【受信値】speed_L : %d, speed_R : %d \n", spL, spR);
  delay(1000);
}
