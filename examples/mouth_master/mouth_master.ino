#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, 115200);

#define IMAGE_TYPE 3
#define LOOP_TIME 1500

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myAvatar.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 値送信
  myAvatar.setFrontDisplay(IMAGE_TYPE, LOOP_TIME);
  Serial.printf("【送信値】imageType : %d, loopTime : %d \n", IMAGE_TYPE, LOOP_TIME);
  delay(1000);
  // 値受信
  int it;
  int lt;
  myAvatar.getFrontDisplay(&it, &lt);
  Serial.printf("【受信値】speed : %d, step : %d \n", it, lt);
  delay(1000);
}
