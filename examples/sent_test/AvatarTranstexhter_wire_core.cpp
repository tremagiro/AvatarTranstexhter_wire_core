#include <Wire.h>

#include <cstddef>
#include <cstdint>
#include <sys/_stdint.h>
#include "Arduino.h"
#include "AvatarTranstexhter_wire_core.h"

// String SET_STEPPER_SPEED = "SET_STEPPER_SPEED";
// String SET_STEPPER_STEP =  "SET_STEPPER_STEP";
// String GET_STEPPER_SPEED =  "GET_STEPPER_SPEED";
// String GET_STEPPER_STEP =  "GET_STEPPER_STEP";
// String SET_DC_MOTOR_L_SPEED = "SET_DC_MOTOR_L_SPEED";
// String GET_DC_MOTOR_L_SPEED =  "GET_DC_MOTOR_L_SPEED";
// String SET_DC_MOTOR_R_SPEED = "SET_DC_MOTOR_R_SPEED";
// String GET_DC_MOTOR_R_SPEED =  "GET_DC_MOTOR_R_SPEED";

#define UP_BIT 32
#define QUOTIENT_BIT 0xFF
#define DWON_BIT 0xFFFFFFFF



//受信結果の送信
void AvatarTranstexhter_wire_core::resuletEvent(){
  // wire_result_enum wire_result = wire_result_global;
  while(wire_result == NONE_RESULT){
    delay(Resend_Time);
  }
  core_wire->write((byte)wire_result);
  wire_result = NONE_RESULT;
}

//初期化
void AvatarTranstexhter_wire_core::init(){
   wire_result_enum wire_result = NONE_RESULT;
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
  // wire_result = NONE_RESULT;
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
  Serial.println(sum);

  // int successes = 0;
  // while (successes < 5) {
    core_wire->beginTransmission(address);
    core_wire->write((uint8_t*)&cmd, sizeof(cmd));
    core_wire->write((uint8_t*)&val, sizeof(val));
    core_wire->write((uint8_t*)&sum, sizeof(sum));
    core_wire->endTransmission();

  //   // core_wire->requestFrom(address, 1);

  //   // if(core_wire->available() > 0){
  //   //   byte result = core_wire->read();
  //   //   if(result == (byte)SUCCESS){
  //   //     return;
  //   //   }else{
  //   //     successes++;
  //   //   }
  //   // }
  // }
}
//文字列からcmdとvalueをそれぞれ分解する
bool AvatarTranstexhter_wire_core::receive_read(int *command, int *value){
  int8_t cmd;
  int16_t val, sum;
  int8_t* data;
  int successes = 0;
  while (successes < 5) {
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
      Serial.printf("sum:%d, cmd:%d, val:%d\n",(int)sum, (int)cmd, (int)val);
      Serial.println(successes);
      if(sum != cmd + val){
        wire_result = FAILURE;
      }else{
        wire_result = SUCCESS;
        return true;
      }
      successes++;
    }
  }
  return false;
}

// ステッピングモーター
// // ステッピングモーター速度調整用メソッド(マスター側)
// bool setStepperSpeed(int speed);
// // ステッピングモーター回転用メソッド(マスター側)
// bool setStepperStep(int step); 
// // ステッピングモーターの設定速度を取得するメソッド(マスター側)
// int getStepperSpeed(int timeout = 0);
// // ステッピングモーターの回転角を取得するメソッド(マスター側)
// int getStepperStep(int timeout = 0);
// // ステッピングモーター制御用デバイス受信メソッド(スレーブ側)
// bool receiveStepperModule(int* cmd, int* value); 
// // スレーブ側からステッピングモーターの情報を送信メソッド(スレーブ側)
// bool sentInfoStpper(int value);
// // DCモーター
// // DCモーターの速度調整用コマンド(マスター側)
// bool setDcMotorSpeed(int speed_l, int speed_r);
// // DCモーターの設定速度を取得するコマンド(マスター側)
// bool getDcMotorSpeed(int* speed_l, int* speed_r, int timeout = 0);
// // DCモーター制御用デバイス受信コマンド(スレーブ側)
// bool receiveDcMotorModule(String* cmd, int* value_l, int* value_r); 
// // スレーブ側からDCモーターの情報を送信(スレーブ側)
// bool sentInfoDcMotor(int value);