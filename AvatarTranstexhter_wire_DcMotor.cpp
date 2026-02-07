#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterDcMotorSlave* AvatarTranstexhterDcMotorSlave::wireCoreInstance = nullptr;

// DCモーター
// DCモーター設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setDcMotorInfo(int speed_l, int speed_r){
  sent_wire(DC_MOTOR_ADDRESS, SET_DC_MOTOR_SPEED_L, speed_l);
  sent_wire(DC_MOTOR_ADDRESS, SET_DC_MOTOR_SPEED_R, speed_r);
}
// ステッピングモーターの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getDcMotorInfo(int* speed_l, int* speed_r){
  // 左回転速度の取得
  sent_wire(DC_MOTOR_ADDRESS, GET_DC_MOTOR_SPEED_L, 0);
  int command;
  int count = 0;
  bool result = false;
  delay(SWITCH_RECEVE);
  // 回転速度を取得
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(DC_MOTOR_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, speed_l);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  // 回転角度の取得
  count = 0;
  sent_wire(DC_MOTOR_ADDRESS, GET_DC_MOTOR_SPEED_R, 0);
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(DC_MOTOR_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, speed_r);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterDcMotorSlave::receiveDcMotorInfo(int receiveByte){
  int command;
  int value;
  // I2C受信・受信後の処理
  if(wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value)){
    switch (command) {
      case SET_DC_MOTOR_SPEED_L:
        speed_L = value;
        updateInfo = true;
      break;
      case SET_DC_MOTOR_SPEED_R:
        speed_R = value;
        updateInfo = true;
      break;
      case GET_DC_MOTOR_SPEED_L:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_DC_MOTOR_SPEED_L);
        wireCoreInstance->wireCore.setSentValue(speed_L);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
      case GET_DC_MOTOR_SPEED_R:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_DC_MOTOR_SPEED_R);
        wireCoreInstance->wireCore.setSentValue(speed_R);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
    }
  }
}