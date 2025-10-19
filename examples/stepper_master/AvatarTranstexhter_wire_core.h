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
    // inline static TwoWire* core_wire = nullptr;
    role_module core_role;
    inline static int core_serial_speed;
    bool enable_serial = false;
    inline static wire_result_enum wire_result = FAILURE;
    inline static result_status resultStatus = RESULT_SENT;
    inline static int sentCmd = 0;
    inline static int sentValue = 0;
    inline static uint8_t readDatas[INFO_SIZE];
    inline static bool updateReceive = false;
    inline static int8_t receiveCmd;
    inline static int16_t receiveVal; 
    inline static int16_t receiveSum;
    // スレーブ送信用イベント
    static void slaveSentEvent();
    static void sent(int command, int value);
    static bool receive(int* command, int* value);
  public:
    // I2Cインスタンス
    inline static TwoWire* core_wire = nullptr;
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
    bool sent_wire(int address, int command, int value);
    //受信したcmdとvalueを格納する
    bool receive_read(int* command, int* value);
    // ステッピングモーター
    // ステッピングモーター設定用メソッド(マスター側)
    void setStepperInfo(int speed, int step);
    // ステッピングモーターの設定を取得するメソッド(スレーブ側)
    bool getStepperInfo(int* sp, int* st);
    // DCモーター
    // DCモーターの速度調整用メソッド(マスター側)
    void setDcMotorInfo(int speed_l, int speed_r);
    // DCモーターの設定速度を取得するメソッド(マスター側)
    bool getDcMotorInfo(int* speed_l, int* speed_r);
    // フロントディスプレイ
    // フロントディスプレイ設定用メソッド（マスター側）
    void setFrontDisplayInfo(int imageType, int loopTime);
    // フロントディスプレイの設定を取得するメソッド（マスター側）
    bool getFrontDisplayInfo(int* imageType, int* loopTime);
    // メンバ変数の値代入用メソッド
    void setSentCmd(byte cmd){sentCmd = cmd;}
    void setSentValue(int val){sentValue = val;}
    void setResultStatus(result_status status){resultStatus = status;}
    // スレーブ受信用イベント
    bool slaveReceiveEvent(int* command, int* value);
};

class AvatarTranstexhterStepperSlave{
  private:
    inline static int speed;
    inline static int step;
    inline static int tmpSpeed;
    inline static int tmpStep;
    inline static bool updateInfo;
    AvatarTranstexhter_wire_core wireCore;
    static AvatarTranstexhterStepperSlave* wireCoreInstance;
    // マスター側から通信を受け取るメソッド(スレーブ側)
    static void receiveStepperInfo(int receiveByte);
  public:
    AvatarTranstexhterStepperSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, STEPPER_MODULE){
      wireCoreInstance = this;
    }
    // メンバ変数取得・設定用メソッド
    void setTmpSpeed(int sp){
      tmpSpeed = sp;
    }
    int getTmpSpeed(){
      return tmpSpeed;
    }
    void setTmpStep(int st){
      tmpStep = st;
    }
    int getTmpStep(){
      return tmpStep;
    }
    void setSpeed(int sp){
      speed = sp;
    }
    int getSpeed(){
      updateInfo = false;
      return speed;
    }
    void setStep(int st){
      step = st;
    }
    int getStep(){
      updateInfo = false;
      return step;
    }
    // 初期化変数
    void init(){
      wireCore.init();
      wireCore.core_wire->onReceive(receiveStepperInfo);
    }
    // I2Cから信号を受け取って一度も読み込まれていないか
    bool updateStepper(){
      return updateInfo;
    }
};

class AvatarTranstexhterDcMotorSlave{
  private:
    inline static int speed_L;
    inline static int speed_R;
    inline static int tmpSpeed_L;
    inline static int tmpSpeed_R;
    inline static bool updateInfo;
    AvatarTranstexhter_wire_core wireCore;
    static AvatarTranstexhterDcMotorSlave* wireCoreInstance;
    // マスター側から通信を受け取るメソッド(スレーブ側)
    static void receiveDcMotorInfo(int receiveByte);
  public:
    AvatarTranstexhterDcMotorSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, DC_MOTOR_MODULE){
      wireCoreInstance = this;
    }
    // メンバ変数取得・設定用メソッド
    void setTmpSpeedL(int spl){
      tmpSpeed_L = spl;
    }
    int getTmpSpeedL(){
      return tmpSpeed_L;
    }
    void setTmpSpeedR(int spr){
      tmpSpeed_R = spr;
    }
    int getTmpSpeedR(){
      return tmpSpeed_R;
    }
    void setSpeedL(int spl){
      speed_L = spl;
    }
    int getSpeedL(){
      updateInfo = false;
      return speed_L;
    }
    void setSpeedR(int spr){
      speed_R = spr;
    }
    int getSpeedR(){
      updateInfo = false;
      return speed_R;
    }
    // 初期化変数
    void init(){
      wireCore.init();
      wireCore.core_wire->onReceive(receiveDcMotorInfo);
    }
    // I2Cから信号を受け取って一度も読み込まれていないか
    bool updateStepper(){
      return updateInfo;
    }
};

class AvatarTranstexhterMouthSlave{
  private:
    inline static int imageType;
    inline static int loopTime;
    inline static int tmpImageType;
    inline static int tmpLoopTime;
    inline static bool updateInfo;
    AvatarTranstexhter_wire_core wireCore;
    static AvatarTranstexhterMouthSlave* wireCoreInstance;
    // マスター側から通信を受け取るメソッド(スレーブ側)
    static void receiveMouthInfo(int receiveByte);
  public:
    AvatarTranstexhterMouthSlave(TwoWire* wire, int serial_speed = 0) : wireCore(wire, serial_speed, MOUTH_MODULE){
      wireCoreInstance = this;
    }
    // メンバ変数取得・設定用メソッド
    void setTmpImageType(int it){
      tmpImageType = it;
    }
    int getTmpImageType(){
      return tmpImageType;
    }
    void setTmpLoopTime(int lt){
      tmpLoopTime = lt;
    }
    int getTmpLoopTime(){
      return tmpLoopTime;
    }
    void setImageType(int it){
      imageType = it;
    }
    int getImageType(){
      updateInfo = false;
      return imageType;
    }
    void setLoopTime(int lt){
      loopTime = lt;
    }
    int getLoopTime(){
      updateInfo = false;
      return loopTime;
    }
    // 初期化変数
    void init(){
      wireCore.init();
      wireCore.core_wire->onReceive(receiveMouthInfo);
    }
    // I2Cから信号を受け取って一度も読み込まれていないか
    bool updateStepper(){
      return updateInfo;
    }
};