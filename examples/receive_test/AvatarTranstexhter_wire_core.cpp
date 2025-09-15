#include "delay.h"
#include <machine/endian.h>
#include <Wire.h>

#include <cstddef>
#include <cstdint>
#include <sys/_stdint.h>
#include "Arduino.h"
#include "AvatarTranstexhter_wire_core.h"

//受信結果の送信
void AvatarTranstexhter_wire_core::resuletEvent(){
  switch (resultStatus) {
    case RESULT_SENT:
      core_wire->write((byte)wire_result);
      wire_result = FAILURE;
      break;
    case VALUE_SENT:
      resultStatus = RESULT_SENT;
      break;
    default:
      break;
  }
}

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
  while (tryTimes < LIMIT_TRY_TIMES) {
    core_wire->beginTransmission(address);
    core_wire->write((uint8_t*)&cmd, sizeof(cmd));
    core_wire->write((uint8_t*)&val, sizeof(val));
    core_wire->write((uint8_t*)&sum, sizeof(sum));
    core_wire->endTransmission();
    delay(10);
    core_wire->requestFrom(address, 1);
    unsigned long nowTime = millis();
    while(core_wire->available() <= 0 && (millis() - nowTime) < LIMIT_WAIT_TIME){
      delay(RESEND_TIME);
    }
    if((millis() - nowTime) >= LIMIT_WAIT_TIME){
      return;
    }
    byte result = core_wire->read();
    if(result == (byte)SUCCESS){
      return;
    }else{
      tryTimes++;
    }
  }
}
//文字列からcmdとvalueをそれぞれ分解する
bool AvatarTranstexhter_wire_core::receive_read(int *command, int *value){
  int8_t cmd;
  int16_t val, sum;
  int8_t* data;
  int tryTimes = 0;
  while (tryTimes < LIMIT_TRY_TIMES) {
    if(core_wire->available() >= sizeof(cmd) + sizeof(val) + sizeof(sum)){

      cmd = core_wire->read();

      data = (int8_t*)&val;
      for(int i = 0;i < sizeof(val) && core_wire->available();i++){
        data[i] = core_wire->read();
      }

      data = NULL;
      data = (int8_t*)&sum;
      for(int i = 0;i < sizeof(sum) && core_wire->available();i++){
        data[i] = core_wire->read();
      }

      *command = (int)cmd;
      *value = (int)val;
      if(core_serial_speed <= 0){
        Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
      }
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