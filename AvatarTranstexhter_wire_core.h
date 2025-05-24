#include "Arduino.h"
#include <Wire.h>

//デバイスのI2Cアドレス
#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39

//コマンド
extern String SET_STEPPER_SPEED;
extern String SET_STEPPER_STEP;
extern String GET_STEPPER_SPEED;
extern String GET_STEPPER_STEP;
extern String SET_DC_MOTOR_L_SPEED;
extern String GET_DC_MOTOR_L_SPEED;
extern String SET_DC_MOTOR_R_SPEED;
extern String GET_DC_MOTOR_R_SPEED;

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
    int core_serial_speed;
    boolean enable_serial = false;
    //cmdとvalueを定めたルールに則って文字列にし、送信する
    void sent_wire(String cmd, long value, byte address = 0);
    //文字列からcmdとvalueをそれぞれ分解する
    boolean split_cmd_value(String* cmd, long* value);
    

  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0){
      core_wire = wire;
      core_role = role;
      core_serial_speed = serial_speed;
    }

    //初期化
    void init();
    //ステッピングモーター速度調整用コマンド(マスター側)
    boolean setStepperSpeed(long speed);
    //ステッピングモーター回転用コマンド(マスター側)
    boolean setStepperStep(long step); 
    //ステッピングモーターの設定速度を取得するコマンド(マスター側)
    long getStepperSpeed(int timeout = 0);
    //ステッピングモーターの回転角を取得するコマンド(マスター側)
    long getStepperStep(int timeout = 0);
    //ステッピングモーター制御用デバイス受信コマンド(スレーブ側)
    boolean receiveStepperModule(String* cmd, long* value); 
    //スレーブ側からステッピングモーターの情報を送信(スレーブ側)
    boolean sentInfoStpper(long value);
    //左DCモーターの速度調整用コマンド(マスター側)
    boolean setDcMotorLSpeed(long speed);
    //左DCモーターの設定速度を取得するコマンド(マスター側)
    long getDcMotorLSpeed(int timeout = 0);
    //右DCモーターの速度調整用コマンド(マスター側)
    boolean setDcMotorRSpeed(long speed);
    //右DCモーターの設定速度を取得するコマンド(マスター側)
    long getDcMotorRSpeed(int timeout = 0);
    //DCモーター制御用デバイス受信コマンド(スレーブ側)
    boolean receiveDcMotorModule(String* cmd, long* value); 
    //スレーブ側からDCモーターの情報を送信(スレーブ側)
    boolean sentInfoDcMotor(long value);
};
