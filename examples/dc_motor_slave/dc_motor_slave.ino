#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhterDcMotorSlave myDcMotor = AvatarTranstexhterDcMotorSlave(&Wire, 115200);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myDcMotor.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myDcMotor.updateDcMotor() == true){
    Serial.printf("【設定値】speed_L : %d, speed_R : %d \n", myDcMotor.getSpeedL(), myDcMotor.getSpeedR());
  }else{
    Serial.println("未更新");
    delay(500);
  }
}
