#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterMouthSlave* AvatarTranstexhterMouthSlave::wireCoreInstance = nullptr;

// マウスモジュール
// マウスモジュール設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setFrontDisplayInfo(int imageType, int loopTime){
  sent_wire(MOUTH_ADDRESS, SET_MOUTH_IMAGE_TYPE, imageType);
  sent_wire(MOUTH_ADDRESS, SET_MOUTH_LOOP_TIME, loopTime);
}
// マウスモジュールの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getFrontDisplayInfo(int* imageType, int* loopTime){
  // 左回転速度の取得
  sent_wire(MOUTH_ADDRESS, GET_MOUTH_IMAGE_TYPE, 0);
  int command;
  int count = 0;
  bool result = false;
  delay(SWITCH_RECEVE);
  // 回転速度を取得
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(MOUTH_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, imageType);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  // 回転角度の取得
  count = 0;
  sent_wire(MOUTH_ADDRESS, GET_MOUTH_LOOP_TIME, 0);
  do{// 正しい値が来るまで繰り返す
    core_wire->requestFrom(MOUTH_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = receive_read(&command, loopTime);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  if(result == false){
    return false;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterMouthSlave::receiveMouthInfo(int receiveByte){
  int command;
  int value;
  // I2C受信・受信後の処理
  if(wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value)){
    switch (command) {
      case SET_MOUTH_IMAGE_TYPE:
        imageType = value;
        updateInfo = true;
      break;
      case SET_MOUTH_LOOP_TIME:
        loopTime = value;
        updateInfo = true;
      break;
      case GET_MOUTH_IMAGE_TYPE:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_MOUTH_IMAGE_TYPE);
        wireCoreInstance->wireCore.setSentValue(imageType);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
      case GET_MOUTH_LOOP_TIME:
        delay(SWITCH_RECEVE);
        wireCoreInstance->wireCore.setSentCmd(GET_MOUTH_LOOP_TIME);
        wireCoreInstance->wireCore.setSentValue(loopTime);
        wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
      break;
    }
  }
}