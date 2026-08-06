#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterStepperSlave myStepper = AvatarTranstexhterStepperSlave(&Wire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myStepper.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myStepper.updateStepper() == true){
    Serial.printf("【設定値】speed : %d, step : %d \n", myStepper.getSpeed(), myStepper.getStep());
  }
  else{
    Serial.printf("信号未受理、受信バイト数：%d \n", Wire.available());
    delay(1000);
  }
}
