#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, MASTER, 115200);

void systemSetup();
void receive(int howByte);
void request();

long speed;
long step;
long sentValue;

void setup() {
}  

void loop() {
  // put your main code here, to run repeatedly:
  static uint64_t value;
  static uint64_t command;
  systemSetup();
  while(1){
    value = 325;
    command = SET_DC_MOTOR_SPEED;
    myAvatar.sent_wire(STEPPER_ADDRESS, command, value);
    delay(1000);
  }
}

void systemSetup(){
  myAvatar.init();
}

