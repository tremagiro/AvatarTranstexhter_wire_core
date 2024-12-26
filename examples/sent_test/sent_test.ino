#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, MASTER, 115200);

void setup() {
  // put your setup code here, to run once:
  myAvatar.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  myAvatar.setStepperSpeed(100);
  delay(1000);
}

