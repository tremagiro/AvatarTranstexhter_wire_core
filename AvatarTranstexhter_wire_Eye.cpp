#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterEyeSlave* AvatarTranstexhterEyeSlave::wireCoreInstance = nullptr;

// 瞳モジュールから1つの値を取得する(マスター側)
// 取得コマンドを送り、正しい値が来るまで要求を繰り返す(ほかのモジュールの取得処理と同じ手順)
static bool getEyeValue(AvatarTranstexhter_wire_core* core, int getCommand, int* value){
  core->sent_wire(EYE_ADDRESS, getCommand, 0);
  int command;
  int count = 0;
  bool result = false;
  delay(SWITCH_RECEVE);
  do{// 正しい値が来るまで繰り返す
    core->core_wire->requestFrom(EYE_ADDRESS, INFO_SIZE);
    delay(SWITCH_RECEVE);
    result = core->receive_read(&command, value);
    delay(SWITCH_RECEVE);
    count++;
  }while(result == false && count < LIMIT_TRY_TIMES);
  return result;
}

// 瞳モジュール
// 両目サーボの角度設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setEyeServoInfo(int angle_l, int angle_r){
  sent_wire(EYE_ADDRESS, SET_EYE_SERVO_L, angle_l);
  sent_wire(EYE_ADDRESS, SET_EYE_SERVO_R, angle_r);
}
// 両目サーボの角度を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getEyeServoInfo(int* angle_l, int* angle_r){
  if(getEyeValue(this, GET_EYE_SERVO_L, angle_l) == false){
    return false;
  }
  return getEyeValue(this, GET_EYE_SERVO_R, angle_r);
}
// 瞳孔画像の種類設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setEyePupilInfo(int pupil_l, int pupil_r){
  sent_wire(EYE_ADDRESS, SET_EYE_PUPIL_L, pupil_l);
  sent_wire(EYE_ADDRESS, SET_EYE_PUPIL_R, pupil_r);
}
// 瞳孔画像の種類を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getEyePupilInfo(int* pupil_l, int* pupil_r){
  if(getEyeValue(this, GET_EYE_PUPIL_L, pupil_l) == false){
    return false;
  }
  return getEyeValue(this, GET_EYE_PUPIL_R, pupil_r);
}
// 瞼画像の種類設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setEyelidInfo(int eyelid_l, int eyelid_r){
  sent_wire(EYE_ADDRESS, SET_EYE_EYELID_L, eyelid_l);
  sent_wire(EYE_ADDRESS, SET_EYE_EYELID_R, eyelid_r);
}
// 瞼画像の種類を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getEyelidInfo(int* eyelid_l, int* eyelid_r){
  if(getEyeValue(this, GET_EYE_EYELID_L, eyelid_l) == false){
    return false;
  }
  return getEyeValue(this, GET_EYE_EYELID_R, eyelid_r);
}
// 瞬きを発生するまでの時間(ms)設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setEyeBlinkTime(int blinkTime){
  sent_wire(EYE_ADDRESS, SET_EYE_BLINK_TIME, blinkTime);
}
// 瞬きを発生するまでの時間(ms)を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getEyeBlinkTime(int* blinkTime){
  return getEyeValue(this, GET_EYE_BLINK_TIME, blinkTime);
}
// ディスプレイの明るさ設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setEyeBrightness(int brightness){
  sent_wire(EYE_ADDRESS, SET_EYE_BRIGHTNESS, brightness);
}
// ディスプレイの明るさを取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getEyeBrightness(int* brightness){
  return getEyeValue(this, GET_EYE_BRIGHTNESS, brightness);
}

// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterEyeSlave::receiveEyeInfo(int receiveByte){
  int command;
  int value;
  // 取得要求に対して返す値
  int sentValue;
  // I2C受信・受信後の処理
  if(wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value)){
    switch (command) {
      case SET_EYE_SERVO_L:
        servo_L = value;
        updateInfo = true;
      return;
      case SET_EYE_SERVO_R:
        servo_R = value;
        updateInfo = true;
      return;
      case SET_EYE_PUPIL_L:
        pupil_L = value;
        updateInfo = true;
      return;
      case SET_EYE_PUPIL_R:
        pupil_R = value;
        updateInfo = true;
      return;
      case SET_EYE_EYELID_L:
        eyelid_L = value;
        updateInfo = true;
      return;
      case SET_EYE_EYELID_R:
        eyelid_R = value;
        updateInfo = true;
      return;
      case SET_EYE_BLINK_TIME:
        blinkTime = value;
        updateInfo = true;
      return;
      case SET_EYE_BRIGHTNESS:
        brightness = value;
        updateInfo = true;
      return;
      case GET_EYE_SERVO_L:
        sentValue = servo_L;
      break;
      case GET_EYE_SERVO_R:
        sentValue = servo_R;
      break;
      case GET_EYE_PUPIL_L:
        sentValue = pupil_L;
      break;
      case GET_EYE_PUPIL_R:
        sentValue = pupil_R;
      break;
      case GET_EYE_EYELID_L:
        sentValue = eyelid_L;
      break;
      case GET_EYE_EYELID_R:
        sentValue = eyelid_R;
      break;
      case GET_EYE_BLINK_TIME:
        sentValue = blinkTime;
      break;
      case GET_EYE_BRIGHTNESS:
        sentValue = brightness;
      break;
      default:
      return;
    }
    // 取得要求の場合は、次の要求(requestFrom)で値を返す
    delay(SWITCH_RECEVE);
    wireCoreInstance->wireCore.setSentCmd(command);
    wireCoreInstance->wireCore.setSentValue(sentValue);
    wireCoreInstance->wireCore.setResultStatus(VALUE_SENT);
  }
}
