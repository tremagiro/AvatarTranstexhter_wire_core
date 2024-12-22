#include "AvatarTranstexhter_wire_core.h"

// AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, MASTER, 115200);

// void setup() {
//   // put your setup code here, to run once:
//   myAvatar.init();
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   myAvatar.setStepperSpeed(100);
//   // myAvatar.setStepperStep(250);
//   // Serial.print("Speed: ");
//   // Serial.println(myAvatar.getStepperSpeed());
//   // Serial.print("Step: ");
//   // Serial.println(myAvatar.getStepperStep());
//   delay(1000);
// }

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, STEPPER_MODULE, 115200);
void receive(int howByte);

int speed;
int step;

void setup() {
  // put your setup code here, to run once:
  myAvatar.init();
  Wire.onReceive(receive);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void receive(int howByte){
  String cmd;
  long value;
  //Serial.println("receive");
  boolean result = myAvatar.receiveStepperModule(&cmd, &value);
  Serial.print("cmd = ");
  Serial.println(cmd);
  Serial.print("value = ");
  Serial.println(value);
  // if(result == true){
  //   switch (cmd) {
  //     case SET_STEPPER_SPEED:
  //       speed = value;
  //     break;

  //     case SET_STEPPER_STEP:
  //       step = value;
  //     break;

  //     case GET_STEPPER_SPEED:
  //       myAvatar.sentInfoStpper(speed);
  //     break;

  //     case GET_STEPPER_STEP:
  //       myAvatar.sentInfoStpper(step);
  //     break;
  //   }
  // }
}

