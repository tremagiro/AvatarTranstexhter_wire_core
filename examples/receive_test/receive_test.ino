#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhter_wire_core myAvatar = AvatarTranstexhter_wire_core(&Wire, STEPPER_MODULE, 115200);

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
  static int value;
  static int command;
  systemSetup();
  Serial.begin(115200);
  Serial.println("start");
  while(1){
    if(myAvatar.receive_read(&command, &value) == true){
      Serial.printf("command = %d, value = %d\n", command, value);
    }
  }
}

void systemSetup(){
  myAvatar.init();
}


