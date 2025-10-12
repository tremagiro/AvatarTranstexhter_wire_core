#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhterMouthSlave myMouth= AvatarTranstexhterMouthSlave(&Wire, 115200);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myMouth.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myMouth.receiveMouthInfo() == true){
    Serial.printf("【設定値】imageType : %d, loopTime : %d \n", myMouth.getImageType(), myMouth.getLoopTime());
  }
}
