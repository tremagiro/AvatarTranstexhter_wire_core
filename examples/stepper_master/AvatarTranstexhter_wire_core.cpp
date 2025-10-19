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
    case DC_MOTOR_MODULE:
      core_wire->begin(DC_MOTOR_ADDRESS);
    break;
  }
  if(core_role != MASTER){
    core_wire->onRequest(slaveSentEvent);
    // core_wire->onReceive(slaveReceiveEvent);
  }
}
// 情報送信用
void AvatarTranstexhter_wire_core::sent(int command, int value){
  int8_t cmd = (int8_t)command;
  int16_t val = (int16_t)value;
  int16_t sum = (int16_t)cmd + (int16_t)val;
  // コマンド送信(1バイト)
  core_wire->write((uint8_t*)&cmd, sizeof(cmd));
  // 値送信（２バイト）
  core_wire->write((uint8_t*)&val, sizeof(val));
  // テェックサム（コマンド＋値）送信（２バイト）
  core_wire->write((uint8_t*)&sum, sizeof(sum));
}
// 情報受け取り用
bool AvatarTranstexhter_wire_core::receive(int* command, int* value){
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
    if(core_serial_speed > 0){
      Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
    }
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