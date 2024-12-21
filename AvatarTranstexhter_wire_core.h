#include "Print.h"
#include "Arduino.h"
#include <Wire.h>

//デバイスのI2Cアドレス
#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39

//コマンド
const byte SET_STEPPER_SPEED =  0x10;
const byte SET_STEPPER_STEP =  0x11;
const byte GET_STEPPER_SPEED =  0x12;
const byte GET_STEPPER_STEP =  0x13;

enum role_module{
  MASTER,
  MOUTH_MODULE,
  STEPPER_MODULE,
  DC_MOTOR_MODULE,
  dummy_role_module
};

class AvatarTranstexhter_wire_core{
  private:
    TwoWire* core_wire;
    role_module core_role;
    boolean enable_serial = false;

  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0){
      core_wire = wire;
      core_role = role;
      if(serial_speed != 0){
        Serial.begin(serial_speed);
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
    }
    //初期化
    void init();
    //ステッピングモーター速度調整用コマンド(マスター側)
    boolean setStepperSpeed(int speed);
    //ステッピングモーター回転用コマンド(マスター側)
    boolean setStepperStep(int step); 
    //ステッピングモーターの設定速度を取得するコマンド(マスター側)
    int getStepperSpeed();
    //ステッピングモーターの回転角を取得するコマンド(マスター側)
    int getStepperStep();
    //ステッピングモーター制御用デバイス受信コマンド(スレーブ側)
    boolean receiveStepperModule(byte* cmd, int* value); 
    //スレーブ側からステッピングモーターの情報を送信(スレーブ側)
    boolean sentInfoStpper(int value);
};
