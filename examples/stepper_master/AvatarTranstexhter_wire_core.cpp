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
void AvatarTranstexhter_wire_core::sent_wire(int address, int command, int value){
  static int16_t maxValue = INT16_MAX - INT8_MAX;
  if(value > maxValue){
    value = maxValue;
  }
  int8_t cmd = (int8_t)command;
  int16_t val = (int16_t)value;
  int16_t sum = (int16_t)cmd + (int16_t)val;

  int tryTimes = 0;
  // LIMIT＿TRY＿TIMES回送信を施行する
  while (tryTimes < LIMIT_TRY_TIMES) {
    core_wire->beginTransmission(address);
    // コマンド送信(1バイト)
    core_wire->write((uint8_t*)&cmd, sizeof(cmd));
    // 値送信（２バイト）
    core_wire->write((uint8_t*)&val, sizeof(val));
    // テェックサム（コマンド＋値）送信（２バイト）
    core_wire->write((uint8_t*)&sum, sizeof(sum));
    core_wire->endTransmission();
    // 返信要求（１バイト）
    core_wire->requestFrom(address, 1);
    unsigned long nowTime = millis();
    // LIMIT_WAIT_TIME未満待機する
    while(core_wire->available() <= 0 && (millis() - nowTime) < LIMIT_WAIT_TIME){
      delay(RESEND_TIME);
    }
    if((millis() - nowTime) >= LIMIT_WAIT_TIME){
      // LIMIT_WAIT_TIME以上返信が返らない場合は処理を終了する
      return;
    }
    byte result = core_wire->read();
    if(result == (byte)SUCCESS){
      // SUCCESSが返ってきたら処理を終了する
      return;
    }else{
      tryTimes++;
    }
  }
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
      int8_t cmd = (int8_t)sentCmd;
      int16_t val = (int16_t)sentValue;
      int16_t sum = (int16_t)cmd + (int16_t)val;
      // コマンド送信(1バイト)
      core_wire->write((uint8_t*)&cmd, sizeof(cmd));
      // 値送信（２バイト）
      core_wire->write((uint8_t*)&val, sizeof(val));
      // テェックサム（コマンド＋値）送信（２バイト）
      core_wire->write((uint8_t*)&sum, sizeof(sum));
      // 受信結果の送信状態へ戻す
      wire_result = SUCCESS;
      resultStatus = RESULT_SENT;
      break;
    // default:
      // break;
  }
}

// スレーブ受信用イベント
bool AvatarTranstexhter_wire_core::slaveReceiveEvent(int* command, int* value){
  int8_t cmd;
  int16_t val, sum;
  int8_t* data;
  if(core_wire->available() >= INFO_SIZE){
    // コマンド受信（１バイト）
    cmd = core_wire->read();
    // 値受信（2バイト）
    data = (int8_t*)&val;
    for(int i = 0;i < sizeof(val) && core_wire->available();i++){
      data[i] = core_wire->read();
    }
    // チェックサム受信(2バイト)
    data = NULL;
    data = (int8_t*)&sum;
    for(int i = 0;i < sizeof(sum) && core_wire->available();i++){
      data[i] = core_wire->read();
    }
    // 代入
    *command = (int)cmd;
    *value = (int)val;
    // if(core_serial_speed <= 0){
    //   Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
    // }
    // 受信結果代入
    if(sum != cmd + val){
      wire_result = FAILURE;
    }else{
      wire_result = SUCCESS;
    }
    return true; 
  }else{
    return false;
  }
}

//受信したcmdとvalueを格納する
bool AvatarTranstexhter_wire_core::receive_read(int* command, int* value){
  int8_t cmd;
  int16_t val, sum;
  int8_t* data;
  int tryTimes = 0;
  while (tryTimes < LIMIT_TRY_TIMES) {
    if(updateReceive == true){
      // コマンド受信（１バイト）
      cmd = receiveCmd;
      // 値受信（2バイト）
      val = receiveVal;
      // チェックサム受信(2バイト)
      sum = receiveSum;
      // 読取済み
      updateReceive = false;
      // 代入
      *command = (int)cmd;
      *value = (int)val;
      if(core_serial_speed <= 0){
        Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
      }
      // 受信結果代入
      if(sum != cmd + val){
        wire_result = FAILURE;
      }else{
        wire_result = SUCCESS;
        return true;
      }
      tryTimes++;
    }else{
      delay(10);
      tryTimes++;
    }
  }
  return false;
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
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(STEPPER_ADDRESS, INFO_SIZE);
    receive_read(&command, speed);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  // 回転角度の取得
  sent_wire(STEPPER_ADDRESS, GET_STEPPER_STEP, 0);
  count = 0;
  do{// 正しい値が来るまで繰り返す
    delay(SWITCH_RECEVE);
    core_wire->requestFrom(STEPPER_ADDRESS, INFO_SIZE);
    receive_read(&command, step);
    count++;
  }while(wire_result == FAILURE && count < LIMIT_TRY_TIMES);
  if(wire_result == FAILURE){
    return false;
  }
  return true;
}
// マスター側から通信を受け取るメソッド(スレーブ側)
void AvatarTranstexhterStepperSlave::receiveStepperInfo(int receiveByte){
  int command;
  int value;
  // I2C受信
  // updateInfo = wireCoreInstance->wireCore.slaveReceiveEvent(&command, &value);
  // 受信後の処理
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