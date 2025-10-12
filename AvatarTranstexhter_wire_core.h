#include "Arduino.h"
#include <Wire.h>
#include <cstddef>
#include <cstdint>
#include <sys/_stdint.h>

//デバイスのI2Cアドレス
#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39

//コマンド
#define SET_STEPPER_SPEED 0x01
#define SET_STEPPER_STEP 0x02
#define GET_STEPPER_SPEED 0x03
#define GET_STEPPER_STEP 0x04
#define SET_DC_MOTOR_SPEED_L 0x05
#define SET_DC_MOTOR_SPEED_R 0x06
#define GET_DC_MOTOR_SPEED_L 0x07
#define GET_DC_MOTOR_SPEED_R 0x08
#define SET_MOUTH_IMAGE_TYPE 0x09
#define SET_MOUTH_LOOP_TIME 0x10
#define GET_MOUTH_IMAGE_TYPE 0x11
#define GET_MOUTH_LOOP_TIME 0x12

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
    AvatarTranstexhter_wire_core(TwoWire* wire, int serial_speed = 0, role_module role = MASTER){
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
    bool receive_read(int* command, int* value);
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
    void setFrontDisplay(int imageType, int loopTime);
    // フロントディスプレイの設定を取得するメソッド（マスター側）
    bool getFrontDisplay(int* imageType, int* loopTime);
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
  public:
    AvatarTranstexhterStepperSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, STEPPER_MODULE){
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
    // マスター側から通信を受け取るメソッド(スレーブ側)
    bool receiveStepperInfo();
};

class AvatarTranstexhterDcMotorSlave{
  private:
    int speed_L;
    int speed_R;
    int tmpSpeed_L;
    int tmpSpeed_R;
    AvatarTranstexhter_wire_core wireCore;
  public:
    AvatarTranstexhterDcMotorSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, DC_MOTOR_MODULE){
    }
    // メンバ変数取得・設定用メソッド
    void setTmpSpeedL(int spL){tmpSpeed_L = spL;}
    int getTmpSpeedL(){return tmpSpeed_L;}
    void setTmpSpeedR(int spR){tmpSpeed_R = spR;}
    int getTmpSpeedR(){return tmpSpeed_R;}
    void setSpeedL(int spL){speed_L = spL;}
    int getSpeedL(){return speed_L;}
    void setSpeedR(int spR){speed_R = spR;}
    int getSpeedR(){return speed_R;}
    // 初期化変数
    void init(){wireCore.init();}
    // マスター側から通信を受け取るメソッド(スレーブ側)
    bool receiveDcMotorInfo();
};

class AvatarTranstexhterMouthSlave{
  private:
    int imageType;
    int loopTime;
    int tmpImageType;
    int tmpLoopTime;
    AvatarTranstexhter_wire_core wireCore;
  public:
    AvatarTranstexhterMouthSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, MOUTH_MODULE){
    }
    // メンバ変数取得・設定用メソッド
    void setTmpImageType(int it){tmpImageType = it;}
    int getTmpImageType(){return tmpImageType;}
    void setTmpLoopTime(int lt){tmpLoopTime = lt;}
    int getTmpLoopTime(){return tmpLoopTime;}
    void setImageType(int it){imageType = it;}
    int getImageType(){return imageType;}
    void setLoopTime(int lt){loopTime = lt;}
    int getLoopTime(){return loopTime;}
    // 初期化変数
    void init(){wireCore.init();}
    // マスター側から通信を受け取るメソッド(スレーブ側)
    bool receiveMouthInfo();
};