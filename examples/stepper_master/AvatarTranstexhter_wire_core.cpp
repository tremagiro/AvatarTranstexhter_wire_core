#include "AvatarTranstexhter_wire_core.h"

AvatarTranstexhterStepperSlave* AvatarTranstexhterStepperSlave::wireCoreInstance = nullptr;

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
    case DC_MOTOR_MODULE:
      core_wire->begin(DC_MOTOR_ADDRESS);
    break;
  }
  if(core_role != MASTER){
    core_wire->onRequest(slaveSentEvent);
    // core_wire->onReceive(slaveReceiveEvent);
  }
}

// I2C経由でコマンドを送信するメソッド 
//cmdとvalueを定めたルールに則って文字列にし、送信する
bool AvatarTranstexhter_wire_core::sent_wire(int address, int command, int value){
  static int16_t maxValue = INT16_MAX - INT8_MAX;
  if(value > maxValue){
    value = maxValue;
  }
  // 送信処理
  int tryTimes = 0;
  while(tryTimes < LIMIT_TRY_TIMES){
    core_wire->beginTransmission(address);
    sent(command, value);
    core_wire->endTransmission(true);
    break;
    // 返信要求（１バイト）
    core_wire->requestFrom(address, 1);
    // 受信結果を受け取る
    byte result = core_wire->read();
    if(result == (byte)SUCCESS){
      // SUCCESSが返ってきたら処理を終了する
      return true;
    }else{
      // 失敗した場合は再送
      tryTimes++;
    }
  }
  return false;
}
//スレーブ送信用イベント
void AvatarTranstexhter_wire_core::slaveSentEvent(){
  switch (resultStatus) {
    // 代入した受信結果の送信
    case RESULT_SENT:
      core_wire->write((byte)wire_result);
      wire_result = FAILURE;
      break;
    // 要求があった値の送信
    case VALUE_SENT:
      sent(sentCmd, sentValue);
      // 受信結果の送信状態へ戻す
      wire_result = SUCCESS;
      resultStatus = RESULT_SENT;
      break;
  }
}

// スレーブ受信用イベント
bool AvatarTranstexhter_wire_core::slaveReceiveEvent(int* command, int* value){
  if(receive(command, value)){
    wire_result = SUCCESS;
    return true;
  }else{
    wire_result = FAILURE;
    return false;
  }
}

//受信したcmdとvalueを格納する
bool AvatarTranstexhter_wire_core::receive_read(int* command, int* value){
  int tryTimes = 0;
  while (tryTimes < LIMIT_TRY_TIMES || !receive(command, value)) {
    delay(LIMIT_TRY_TIMES);
    tryTimes++;
  }
}

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
    }
  }
}

// DCモーター
// DCモーター設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setDcMotorInfo(int speed_l, int speed_r){
  sent_wire(DC_MOTOR_ADDRESS, SET_DC_MOTOR_SPEED_L, speed_l);
  sent_wire(DC_MOTOR_ADDRESS, SET_DC_MOTOR_SPEED_R, speed_r);
}
// DCモーターの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getDcMotorInfo(int* speed_l, int* speed_r){
  // 左回転速度の取得
  sent_wire(DC_MOTOR_ADDRESS, GET_DC_MOTOR_SPEED_L, 0);
  int command;
  int count = 0;
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(DC_MOTOR_ADDRESS, INFO_SIZE);
    receive_read(&command, speed_l);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  // 右回転速度の取得
  sent_wire(DC_MOTOR_ADDRESS, GET_DC_MOTOR_SPEED_R, 0);
  count = 0;
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(DC_MOTOR_ADDRESS, INFO_SIZE);
    receive_read(&command, speed_r);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterDcMotorSlave::receiveDcMotorInfo(int receiveByte){
  int command;
  int value;
  // I2C受信
  // updateInfo = wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value);
  // 受信後の処理
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

// マウスモジュール
// マウスモジュール設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setFrontDisplayInfo(int imageType, int loopTime){
  sent_wire(MOUTH_ADDRESS, SET_MOUTH_IMAGE_TYPE, imageType);
  sent_wire(MOUTH_ADDRESS, SET_MOUTH_LOOP_TIME, loopTime);
}
// DCモーターの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getFrontDisplayInfo(int *imageType, int *loopTime){
  // 画像種類の取得
  sent_wire(MOUTH_ADDRESS, GET_MOUTH_IMAGE_TYPE, 0);
  int command;
  int count = 0;
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(MOUTH_ADDRESS, INFO_SIZE);
    receive_read(&command, imageType);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  // 周期時間の取得
  sent_wire(MOUTH_ADDRESS, GET_MOUTH_LOOP_TIME, 0);
  count = 0;
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(MOUTH_ADDRESS, INFO_SIZE);
    receive_read(&command, loopTime);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterMouthSlave::receiveMouthInfo(int receiveByte){
  int command;
  int value;
  // I2C受信
  // updateInfo = wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value);
  // 受信後の処理
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