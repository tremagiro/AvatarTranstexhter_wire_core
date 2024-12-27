#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, MASTER);

void setup() {
  // put your setup code here, to run once:
  myAvatar.init();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //ステッピングモーターの回転速度を設定
  myAvatar.setStepperSpeed(100);
  //ステッピングモーターの回転角度を設定
  myAvatar.setStepperStep(-251);
  //設定したステッピングモーターの回転速度を取得
  Serial.print("speed : ");
  Serial.println(myAvatar.getStepperSpeed());
  //設定したステッピングモーターの回転角度を取得
  Serial.print("step : ");
  Serial.println(myAvatar.getStepperStep());
  delay(1000);
}

