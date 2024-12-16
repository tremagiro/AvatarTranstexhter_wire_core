#include "Arduino.h"
#include "AvatarTranstexhter_wire_core.h"

//ステッピングモーター速度調整用コマンド(マスター側)
boolean AvatarTranstexhter_wire_core::setStepperSpeed(int speed){
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  core_wire->beginTransmission(STEPPER_ADDRESS);
  core_wire->write(SET_STEPPER_SPEED);
  core_wire->write(highByte(speed));
  core_wire->write(lowByte(speed));
  core_wire->endTransmission();
  return true;
}

//ステッピングモーター回転用コマンド(マスター側)
boolean AvatarTranstexhter_wire_core::setStepperStep(int step){
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  core_wire->beginTransmission(STEPPER_ADDRESS);
  core_wire->write(SET_STEPPER_STEP);
  core_wire->write(highByte(step));
  core_wire->write(lowByte(step));
  core_wire->endTransmission();
  return true;
}

//ステッピングモーター制御用デバイス受信コマンド(スレーブ側)
boolean AvatarTranstexhter_wire_core::receiveStepperModule(byte* cmd, int* value){ 
  if(core_role != STEPPER_MODULE){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  else if(core_wire->available() == 3){
    *cmd = core_wire->read();
    byte high = core_wire->read();
    byte low = core_wire->read();
    *value = (high << 8) | low;
    return true;
  }else{
    return false;
  }
}