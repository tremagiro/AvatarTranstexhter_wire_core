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

#define INFO_SIZE 5
#define LIMIT_TRY_TIMES 5
#define LIMIT_WAIT_TIME 1000
#define SWITCH_RECEVE 5

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
    inline static wire_result_enum wire_result = FAILURE;
    inline static result_status resultStatus = RESULT_SENT;
    inline static byte sentCmd = 0;
    inline static int sentValue = 0;
    // スレーブ送信用イベント
    static void slaveSentEvent();
    
  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0){
      core_wire = wire;
      core_role = role;
      core_serial_speed = serial_speed;
      if(core_serial_speed != 0){
        enable_serial = true;
      }
    }
    //初期化
    void init();
    //cmdとvalueを定めたルールに則って文字列にし、送信する
    void sent_wire(int address, int command, int value);
    //受信したcmdとvalueを格納する
    bool receive_read(int *command, int *value);
    // ステッピングモーター
    // ステッピングモーター設定用メソッド(マスター側)
    void setStepperInfo(int speed, int step);
    // ステッピングモーターの設定を取得するメソッド(スレーブ側)
    bool getStepperInfo(int* sp, int* st);
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
    // メンバ変数の値代入用メソッド
    void setSentCmd(byte cmd){sentCmd = cmd;}
    void setSentValue(int val){sentValue = val;}
    void setResultStatus(result_status status){resultStatus = status;}
};

class AvatarTranstexhterStepperSlave{
  private:
    int speed;
    int step;
    int tmpSpeed;
    int tmpStep;
    AvatarTranstexhter_wire_core wireCore;
    // 受信した結果から次の送信イベントを制御するメソッド
    void controlReceiveInfo();
  public:
    AvatarTranstexhterStepperSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, STEPPER_MODULE ,serial_speed){
    }
    // メンバ変数取得・設定用メソッド
    void setTmpSpeed(int sp){tmpSpeed = sp;}
    int getTmpSpeed(){return tmpSpeed;}
    void setTmpStep(int st){tmpStep = st;}
    int getTmpStep(){return tmpStep;}
    void setSpeed(int sp){speed = sp;}
    int getSpeed(){return speed;}
    void setStep(int st){step = st;}
    int getStep(){return step;}
    // 初期化変数
    void init(){wireCore.init();}
    // マスター側から受信した値をspeedとstepに格納する
    bool receiveStepInfo();
};
