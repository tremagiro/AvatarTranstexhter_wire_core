#include "AvatarTranstexhter_wire_core.h"

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
    // if(core_serial_speed <= 0){
    Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
    // }
    // 受信結果代入
    // if(sum != cmd + val){
    //   wire_result = FAILURE;
    // }else{
    //   wire_result = SUCCESS;
    // }
    return true; 
  }else{
    return false;
  }
}