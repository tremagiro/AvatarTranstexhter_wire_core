#include "USB/USBAPI.h"
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
boolean AvatarTranstexhter_wire_core::setStepperSpeed(long speed){
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  sent_wire(SET_STEPPER_SPEED, speed, STEPPER_ADDRESS);
  return true;
}

//ステッピングモーター回転用コマンド(マスター側)
boolean AvatarTranstexhter_wire_core::setStepperStep(long step){
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  sent_wire(SET_STEPPER_STEP, step, STEPPER_ADDRESS);
  return true;
}

//ステッピングモーターの設定速度を取得するコマンド(マスター側)
int AvatarTranstexhter_wire_core::getStepperSpeed(int timeout){
  int value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return 0;
  }
  sent_wire(GET_STEPPER_SPEED, 0, STEPPER_ADDRESS);
  if(timeout != 0){
    long start = millis();
    while(timeout >= (millis() - start) || core_wire->available() == 0){}
  }
  if(core_wire->available() == 0){
    return 0;
  }else{
    
  }
  return 0;
}

//ステッピングモーターの回転角を取得するコマンド(マスター側)
int AvatarTranstexhter_wire_core::getStepperStep(int timeout){
  int value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  sent_wire(GET_STEPPER_STEP, 0, STEPPER_ADDRESS);
  if(core_wire->requestFrom(STEPPER_ADDRESS, 2) == 2){
    byte high = core_wire->read();
    byte low = core_wire->read();
    value = (high << 8) | low;
    return value;
  }
  return 0;
}

//ステッピングモーター制御用デバイス受信コマンド(スレーブ側)
boolean AvatarTranstexhter_wire_core::receiveStepperModule(String* cmd, long* value){ 
  if(core_role != STEPPER_MODULE){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  if(core_wire->available() != 0){
    String sentCmd = "";
    while (core_wire->available() > 0) {
      char c = core_wire->read();
      sentCmd += c;
    }
    if(enable_serial == true){
      Serial.println(sentCmd);
    }
    int colon[3];
    int colonIndex = 0;
    if(sentCmd.indexOf("cmd:") >= 0){
      char charCmd[sentCmd.length()];
      sentCmd.toCharArray(charCmd, sentCmd.length());
      for(int i = 0;i<sentCmd.length();i++){
        if(charCmd[i] == ':'){
          colon[colonIndex] = i;
          colonIndex++; 
        }
      }
      *cmd = sentCmd.substring(colon[0] + 1, colon[1]);
      *value = sentCmd.substring(colon[2] + 1, sentCmd.length()).toInt();
      return true;
    }
    else{
      return false;
    }
  }else{
    return false;
  }
}

//スレーブ側からステッピングモーターの情報を送信(スレーブ側)
boolean AvatarTranstexhter_wire_core::sentInfoStpper(long value){
  sent_wire("", value);
  if(enable_serial == true){
    Serial.println("Send wire");
    Serial.print("value: ");
    Serial.println(value);
  }
  return true;
}

////cmdとvalueを定めたルールに則って文字列にし、送信する
void AvatarTranstexhter_wire_core::sent_wire(String cmd, long value, byte address){
  String sentCmd = "cmd:" + cmd + ":value:" + String(value); 
  if(enable_serial == true){
    Serial.println(address);
    Serial.println(sentCmd);
  }
  if(address == 0){
    core_wire->write(sentCmd.c_str());
    core_wire->endTransmission();
  }else{
    core_wire->beginTransmission(address);
    core_wire->write(sentCmd.c_str());
    core_wire->endTransmission();
  }
}