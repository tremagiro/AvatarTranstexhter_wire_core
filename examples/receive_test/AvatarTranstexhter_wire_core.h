#include "Arduino.h"
#include <Wire.h>

//デバイスのI2Cアドレス
#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39

//コマンド
#define SET_STEPPER_SPEED 0x01
#define SET_STEPPER_STEP 0x02
#define GET_STEPPER_SPEED 0x03
#define GET_STEPPER_STEP 0x04
#define SET_DC_MOTOR_SPEED 0x05
#define GET_DC_MOTOR_SPEED 0x06

#define LIMIT_TRY_TIMES 5

enum role_module{
  MASTER,
  MOUTH_MODULE,
  STEPPER_MODULE,
  DC_MOTOR_MODULE,
  dummy_role_module
};

enum wire_result_enum{
  FAILURE,
  SUCCESS
};
#define Resend_Time 10 

class AvatarTranstexhter_wire_core{
  private:
    inline static TwoWire* core_wire = nullptr;
    role_module core_role;
    int core_serial_speed;
    bool enable_serial = false;
    // wire_result_enum wire_result_global;
    inline static wire_result_enum wire_result = FAILURE;
    static void resuletEvent();
    
  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0){
      core_wire = wire;
      core_role = role;
      core_serial_speed = serial_speed;
    }
    //cmdとvalueを定めたルールに則って文字列にし、送信する
    void sent_wire(int address, int command, int value);
    //文字列からcmdとvalueをそれぞれ分解する
    bool receive_read(int *command, int *value);

    //初期化
    void init();
    // ステッピングモーター
    // ステッピングモーター速度調整用メソッド(マスター側)
    bool setStepperSpeed(int speed);
    // ステッピングモーター回転用メソッド(マスター側)
    bool setStepperStep(int step); 
    // ステッピングモーターの設定速度を取得するメソッド(マスター側)
    int getStepperSpeed(int timeout = 0);
    // ステッピングモーターの回転角を取得するメソッド(マスター側)
    int getStepperStep(int timeout = 0);
    // ステッピングモーター制御用デバイス受信メソッド(スレーブ側)
    bool receiveStepperModule(int* cmd, int* value); 
    // スレーブ側からステッピングモーターの情報を送信メソッド(スレーブ側)
    bool sentInfoStpper(int value);
    // DCモーター
    // DCモーターの速度調整用コマンド(マスター側)
    bool setDcMotorSpeed(int speed_l, int speed_r);
    // DCモーターの設定速度を取得するコマンド(マスター側)
    bool getDcMotorSpeed(int* speed_l, int* speed_r, int timeout = 0);
    // DCモーター制御用デバイス受信コマンド(スレーブ側)
    bool receiveDcMotorModule(String* cmd, int* value_l, int* value_r); 
    // スレーブ側からDCモーターの情報を送信(スレーブ側)
    bool sentInfoDcMotor(int value);
};
