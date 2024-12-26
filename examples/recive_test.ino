#include "AvatarTranstexhter_wire_core.h"

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
}

