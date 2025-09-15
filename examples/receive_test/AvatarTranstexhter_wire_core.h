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
#define LIMIT_WAIT_TIME 1000

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
#define RESEND_TIME 10 

enum result_status{
  RESULT_SENT,
  VALUE_SENT
};

class AvatarTranstexhter_wire_core{
  private:
    inline static TwoWire* core_wire = nullptr;
    role_module core_role;
    int core_serial_speed;
    bool enable_serial = false;
    // wire_result_enum wire_result_global;
    inline static wire_result_enum wire_result = FAILURE;
    inline static result_status resultStatus = RESULT_SENT;
    inline static int sentValue0 = 0;
    inline static int sentValue1 = 0;
    static void resuletEvent();
    
  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0){
      core_wire = wire;
      core_role = role;
      core_serial_speed = serial_speed;
    }
    //初期化
    void init();
    //cmdとvalueを定めたルールに則って文字列にし、送信する
    void sent_wire(int address, int command, int value);
    //文字列からcmdとvalueをそれぞれ分解する
    bool receive_read(int *command, int *value);
    // ステッピングモーター
    // ステッピングモーター設定用メソッド(マスター側)
    void setStepperSpeed(int speed, int step);
    // ステッピングモーターの設定を取得するメソッド(マスター側)
    bool getStepperSpeed(int* speed, int* step);
    // DCモーター
    // DCモーターの速度調整用メソッド(マスター側)
    void setDcMotor(int speed_l, int speed_r);
    // DCモーターの設定速度を取得するメソッド(マスター側)
    bool getDcMotor(int* speed_l, int* speed_r);
    // フロントディスプレイ
    // フロントディスプレイ設定用メソッド（マスター側）
    void setFrontDisplay(int typeImage, int loopTime);
    // フロントディスプレイの設定を取得するメソッド（マスター側）
    bool getFrontDisplay(int* typeImage, int* loopTime);
};
