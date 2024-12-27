#include "AvatarTranstexhter_wire_core.h"

// AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, MASTER);

// void setup() {
//   // put your setup code here, to run once:
//   myAvatar.init();
//   Serial.begin(115200);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   //ステッピングモーターの回転速度を設定
//   myAvatar.setStepperSpeed(100);
//   //ステッピングモーターの回転角度を設定
//   myAvatar.setStepperStep(-251);
//   //設定したステッピングモーターの回転速度を取得
//   Serial.print("speed : ");
//   Serial.println(myAvatar.getStepperSpeed(1000));
//   //設定したステッピングモーターの回転角度を取得
//   Serial.print("step : ");
//   Serial.println(myAvatar.getStepperStep(1000));
//   delay(1000);
// }

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, STEPPER_MODULE);
void receive(int howByte);
void request();

long speed;
long step;
long sentValue;

void setup() {
  // put your setup code here, to run once:
  myAvatar.init();
  Serial.begin(115200);
  Wire.onReceive(receive);
  Wire.onRequest(request);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("speed = ");
  Serial.println(speed);
  Serial.print("step = ");
  Serial.println(step);
  delay(1000);
}

void receive(int howByte){
  String cmd;
  long value;
  //Serial.println("receive");
  boolean result = myAvatar.receiveStepperModule(&cmd, &value);
  if(cmd.equals(SET_STEPPER_SPEED)){
    speed = value;
  }else if(cmd.equals(SET_STEPPER_STEP)){
    step = value;
  }else if(cmd.equals(GET_STEPPER_SPEED)){
    sentValue = speed;
  }else if(cmd.equals(GET_STEPPER_STEP)){
    sentValue = step;
  }
}

void request(){
  myAvatar.sentInfoStpper(sentValue);
}