#include "delay.h"
#include <machine/endian.h>
#include <Wire.h>

#include <cstddef>
#include <cstdint>
#include <sys/_stdint.h>
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
  if(core_role != MASTER){
    core_wire->onRequest(resuletEvent);
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
    delay(10);
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
//受信結果の送信
void AvatarTranstexhter_wire_core::resuletEvent(){
  switch (resultStatus) {
    // 代入した受信結果の送信
    case RESULT_SENT:
      core_wire->write((byte)wire_result);
      wire_result = FAILURE;
      break;
    // 要求があった値の送信
    case VALUE_SENT:
      // コマンド送信(1バイト)
      core_wire->write((uint8_t*)&sentCmd, sizeof(sentCmd));
      // 値送信（２バイト）
      core_wire->write((uint8_t*)&sentValue, sizeof(sentValue));
      // テェックサム（コマンド＋値）送信（２バイト）
      int16_t sum = (int16_t)sentCmd + (int16_t)sentValue;
      core_wire->write((uint8_t*)&sum, sizeof(sum));
      // 受信結果の送信状態へ戻す
      resultStatus = RESULT_SENT;
      break;
    // default:
      // break;
  }
}

//受信したcmdとvalueを格納する
bool AvatarTranstexhter_wire_core::receive_read(int *command, int *value){
  int8_t cmd;
  int16_t val, sum;
  int8_t* data;
  int tryTimes = 0;
  while (tryTimes < LIMIT_TRY_TIMES) {
    if(core_wire->available() >= sizeof(cmd) + sizeof(val) + sizeof(sum)){
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
    }
  }
  return false;
}

// ステッピングモーター
// ステッピングモーター設定用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setStepperSpeed(int speed, int step){

}
// ステッピングモーターの設定を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getStepperSpeed(int* speed, int* step){
  return true;
}
// DCモーター
// DCモーターの速度調整用メソッド(マスター側)
void AvatarTranstexhter_wire_core::setDcMotor(int speed_l, int speed_r){

}
// DCモーターの設定速度を取得するメソッド(マスター側)
bool AvatarTranstexhter_wire_core::getDcMotor(int* speed_l, int* speed_r){
  return true;
}
// フロントディスプレイ
// フロントディスプレイ設定用メソッド（マスター側）
void AvatarTranstexhter_wire_core::setFrontDisplay(int typeImage, int loopTime){

}
// フロントディスプレイの設定を取得するメソッド（マスター側）
bool AvatarTranstexhter_wire_core::getFrontDisplay(int* typeImage, int* loopTime){
  return true;
}