#include "Arduino.h"
#include "AvatarTranstexhter_wire_core.h"

//初期化
void AvatarTranstexhter_wire_core::init(){
  if(core_serial_speed > 0){
    Serial.begin(core_serial_speed);
    enable_serial = true;
  }
  switch(core_role){
    case MASTER:
      core_wire->begin();
    break;
    case MOUTH_MODULE:
      core_wire->begin(MOUTH_ADDRESS);
    break;
    case  STEPPER_MODULE:
      core_wire->begin(STEPPER_ADDRESS);
    break;
    case DC_MOTOR_ADDRESS:
      core_wire->begin(DC_MOTOR_ADDRESS);
    break;
  }
}

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

//ステッピングモーターの設定速度を取得するコマンド(マスター側)
int AvatarTranstexhter_wire_core::getStepperSpeed(){
  int value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  core_wire->beginTransmission(STEPPER_ADDRESS);
  core_wire->write(GET_STEPPER_SPEED);
  core_wire->write(highByte(0));
  core_wire->write(lowByte(0));
  core_wire->endTransmission();
  if(core_wire->requestFrom(STEPPER_ADDRESS, 2) == 2){
    byte high = core_wire->read();
    byte low = core_wire->read();
    value = (high << 8) | low;
    return value;
  }
  return 0;
}

//ステッピングモーターの回転角を取得するコマンド(マスター側)
int AvatarTranstexhter_wire_core::getStepperStep(){
  int value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  core_wire->beginTransmission(STEPPER_ADDRESS);
  core_wire->write(GET_STEPPER_STEP);
  core_wire->write(highByte(0));
  core_wire->write(lowByte(0));
  core_wire->endTransmission();
  if(core_wire->requestFrom(STEPPER_ADDRESS, 2) == 2){
    byte high = core_wire->read();
    byte low = core_wire->read();
    value = (high << 8) | low;
    return value;
  }
  return 0;
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

//スレーブ側からステッピングモーターの情報を送信(スレーブ側)
boolean AvatarTranstexhter_wire_core::sentInfoStpper(int value){
  core_wire->write(highByte(value));
  core_wire->write(lowByte(value));
  return true;
}