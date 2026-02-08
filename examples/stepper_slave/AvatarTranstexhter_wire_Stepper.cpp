#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterStepperSlave* AvatarTranstexhterStepperSlave::wireCoreInstance = nullptr;

// ステッピングモーター
// ステッピングモーター設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setStepperInfo(int speed, int step){
  sent_wire(STEPPER_ADDRESS, SET_STEPPER_SPEED, speed);
  sent_wire(STEPPER_ADDRESS, SET_STEPPER_STEP, step);
}
// ステッピングモーターの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getStepperInfo(int* speed, int* step){
  // 回転速度の取得
  sent_wire(STEPPER_ADDRESS, GET_STEPPER_SPEED, 0);
  int command;
  int count = 0;
  bool result = false;
  delay(SWITCH_RECEVE);
  // 回転速度を取得
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(STEPPER_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, speed);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  // 回転角度の取得
  count = 0;
  sent_wire(STEPPER_ADDRESS, GET_STEPPER_STEP, 0);
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(STEPPER_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, step);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  return true;
}
// ステッピングモーターの動作モード設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setStepperMode(mode_stepper mode){
  sent_wire(STEPPER_ADDRESS, SET_STEPPER_MODE, (int)mode);
}
// ステッピングモーターの動作モード取得用メソッド(マスター側)
bool AvatarTranstexhter_wire_core::getStepperMode(mode_stepper* mp){
  // 動作モードの取得
  sent_wire(STEPPER_ADDRESS, GET_STEPPER_MODE, 0);
  int command;
  int count = 0;
  bool result = false;
  int tmpMode;
  delay(SWITCH_RECEVE);
  // 動作モードを取得
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(STEPPER_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, &tmpMode);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  switch(tmpMode){
    case 0:
      *mp = DUAL;
    break;
    case 1:
      *mp = A;
    break;
    case 2:
      *mp = B;
    break;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterStepperSlave::receiveStepperInfo(int receiveByte){
  int command;
  int value;
  // I2C受信・受信後の処理
  if(wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value)){
    switch (command) {
      case SET_STEPPER_SPEED:
        speed = value;
        updateInfo = true;
      break;
      case SET_STEPPER_STEP:
        step = value;
        updateInfo = true;
      break;
      case SET_STEPPER_MODE:
        switch(value){
          case 0:
            mode = DUAL;
          break;
          case 1:
            mode = A;
          break;
          case 2:
            mode = B;
          break;
        }
        updateInfo = true;
      break;
      case GET_STEPPER_SPEED:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_STEPPER_SPEED);
        wireCoreInstance->wireCore.setSentValue(speed);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
      case GET_STEPPER_STEP:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_STEPPER_STEP);
        wireCoreInstance->wireCore.setSentValue(step);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
      case GET_STEPPER_MODE:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_STEPPER_MODE);
        wireCoreInstance->wireCore.setSentValue(mode);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
    }
  }
}