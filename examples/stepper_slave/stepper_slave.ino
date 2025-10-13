#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterStepperSlave myStepper = AvatarTranstexhterStepperSlave(&Wire, 115200);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myStepper.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myStepper.receiveStepperInfo() == true){
    Serial.printf("【設定値】speed : %d, step : %d \n", myStepper.getSpeed(), myStepper.getStep());
  }
}
