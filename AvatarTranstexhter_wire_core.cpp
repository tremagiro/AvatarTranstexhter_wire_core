#include "Arduino.h"
#include "AvatarTranstexhter_wire_core.h"

String SET_STEPPER_SPEED = "SET_STEPPER_SPEED";
String SET_STEPPER_STEP =  "SET_STEPPER_STEP";
String GET_STEPPER_SPEED =  "GET_STEPPER_SPEED";
String GET_STEPPER_STEP =  "GET_STEPPER_STEP";
String SET_DC_MOTOR_L_SPEED = "SET_DC_MOTOR_L_SPEED";
String GET_DC_MOTOR_L_SPEED = "GET_DC_MOTOR_L_SPEED";
String SET_DC_MOTOR_R_SPEED = "SET_DC_MOTOR_R_SPEED";
String GET_DC_MOTOR_R_SPEED = "GET_DC_MOTOR_R_SPEED";

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
long AvatarTranstexhter_wire_core::getStepperSpeed(int timeout){
  long value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return 0;
  }
  sent_wire(GET_STEPPER_SPEED, 0, STEPPER_ADDRESS);
  delay(10);
  core_wire->requestFrom(STEPPER_ADDRESS, 32);
  if(timeout != 0){
    long start = millis();
    while(timeout >= (millis() - start) && core_wire->available() == 0);
  }
  if(core_wire->available() == 0){
    return 0;
  }else{
    String cmd;
    split_cmd_value(&cmd, &value);
    return value;
  }
  return 0;
}

//ステッピングモーターの回転角を取得するコマンド(マスター側)
long AvatarTranstexhter_wire_core::getStepperStep(int timeout){
  long value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return 0;
  }
  sent_wire(GET_STEPPER_STEP, 0, STEPPER_ADDRESS);
  delay(10);
  core_wire->requestFrom(STEPPER_ADDRESS, 32);
  if(timeout != 0){
    long start = millis();
    while(timeout >= (millis() - start) && core_wire->available() == 0);
  }
  if(core_wire->available() == 0){
    return 0;
  }else{
    String cmd;
    split_cmd_value(&cmd, &value);
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
  return split_cmd_value(cmd, value);
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

//DCモーター速度調整用コマンド(マスター側)
boolean AvatarTranstexhter_wire_core::setDcMotorSpeed(long speed){
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  sent_wire(SET_DC_MOTOR_SPEED, speed, DC_MOTOR_ADDRESS);
  return true;
}

//DCモーターの設定速度を取得するコマンド(マスター側)
long AvatarTranstexhter_wire_core::getDcMotorSpeed(int timeout){
  long value = 0;
  if(core_role != MASTER){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return 0;
  }
  sent_wire(GET_DC_MOTOR_SPEED, 0, DC_MOTOR_ADDRESS);
  delay(10);
  core_wire->requestFrom(DC_MOTOR_ADDRESS, 32);
  if(timeout != 0){
    long start = millis();
    while(timeout >= (millis() - start) && core_wire->available() == 0);
  }
  if(core_wire->available() == 0){
    return 0;
  }else{
    String cmd;
    split_cmd_value(&cmd, &value);
    return value;
  }
  return 0;
}

//DCモーター制御用デバイス受信コマンド(スレーブ側)
boolean AvatarTranstexhter_wire_core::receiveDcMotorModule(String* cmd, long* value){ 
  if(core_role != DC_MOTOR_MODULE){
    if(enable_serial == true){
      Serial.println("This function is not available for this device role.");
    }
    return false;
  }
  return split_cmd_value(cmd, value);
}

//スレーブ側からDCモーターの情報を送信(スレーブ側)
boolean AvatarTranstexhter_wire_core::sentInfoDcMotor(long value){
  return AvatarTranstexhter_wire_core::sentInfoStpper(value);
}

//cmdとvalueを定めたルールに則って文字列にし、送信する
void AvatarTranstexhter_wire_core::sent_wire(String cmd, long value, byte address){
  String sentCmd = "cmd:" + cmd + ":value:" + String(value) + ";"; 
  if(enable_serial == true){
    Serial.println(address);
    Serial.println(sentCmd);
  }
  if(address == 0){
    core_wire->write(sentCmd.c_str());
    //core_wire->endTransmission();
  }else{
    core_wire->beginTransmission(address);
    core_wire->write(sentCmd.c_str());
    core_wire->endTransmission();
  }
}

//文字列からcmdとvalueをそれぞれ分解する
boolean AvatarTranstexhter_wire_core::split_cmd_value(String* cmd, long* value){
  if(core_wire->available() != 0){
    String sentCmd = "";
    boolean over = false;
    while (core_wire->available() > 0) {
      char c = core_wire->read();
      if(c == ';'){
        over = true;
      }
      if(over == false){
        sentCmd += c;
      }
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
      if(enable_serial == true){
        Serial.println(sentCmd);
      }
      if(colon[2] != 0){
        *cmd = sentCmd.substring(colon[0] + 1, colon[1]);
        *value = sentCmd.substring(colon[2] + 1, sentCmd.length()).toInt();
        return true;
      }else{
        return false;
      }
    }
    else{
      return false;
    }
  }else{
    return false;
  }
}