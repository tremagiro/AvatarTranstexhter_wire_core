#include "Arduino.h"
#include <Wire.h>
#include <cstddef>
#include <cstdint>
#include <sys/_stdint.h>

//デバイスのI2Cアドレス
#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39
#define EYE_ADDRESS 0x3a

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
#define SET_MOUTH_LOOP_TIME 0x0a
#define GET_MOUTH_IMAGE_TYPE 0x0b
#define GET_MOUTH_LOOP_TIME 0x0c
#define SET_STEPPER_MODE 0x0d
#define GET_STEPPER_MODE 0x0e
#define SET_EYE_SERVO_L 0x0f
#define SET_EYE_SERVO_R 0x10
#define GET_EYE_SERVO_L 0x11
#define GET_EYE_SERVO_R 0x12
#define SET_EYE_PUPIL_L 0x13
#define SET_EYE_PUPIL_R 0x14
#define GET_EYE_PUPIL_L 0x15
#define GET_EYE_PUPIL_R 0x16
#define SET_EYE_EYELID_L 0x17
#define SET_EYE_EYELID_R 0x18
#define GET_EYE_EYELID_L 0x19
#define GET_EYE_EYELID_R 0x1a
#define SET_EYE_BLINK_TIME 0x1b
#define GET_EYE_BLINK_TIME 0x1c
#define SET_EYE_BRIGHTNESS 0x1d
#define GET_EYE_BRIGHTNESS 0x1e

#define INFO_SIZE 5
#define LIMIT_TRY_TIMES 5
#define LIMIT_WAIT_TIME 1000
#define SWITCH_RECEVE 5

enum role_module{
  MASTER,
  MOUTH_MODULE,
  STEPPER_MODULE,
  DC_MOTOR_MODULE,
  EYE_MODULE,
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

enum mode_stepper{
  DUAL,
  A,
  B
};

class AvatarTranstexhter_wire_core{
  private:
    // inline static TwoWire* core_wire = nullptr;
    role_module core_role;
    // inline static int core_serial_speed;
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
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER){
      core_wire = wire;
      core_role = role;
      // core_serial_speed = serial_speed;
      // if(core_serial_speed != 0){
      //   enable_serial = true;
      // }
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
    // ステッピングモーターの設定を取得するメソッド(マスター側)
    bool getStepperInfo(int* sp, int* st);
    // ステッピングモーターの動作モード設定用メソッド(マスター側)
    void setStepperMode(mode_stepper mode);
    // ステッピングモーターの動作モード取得用メソッド(マスター側)
    bool getStepperMode(mode_stepper* mp);
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
    // 瞳モジュール
    // 両目サーボの角度設定用メソッド(マスター側)
    void setEyeServoInfo(int angle_l, int angle_r);
    // 両目サーボの角度を取得するメソッド(マスター側)
    bool getEyeServoInfo(int* angle_l, int* angle_r);
    // 瞳孔画像の種類設定用メソッド(マスター側)
    void setEyePupilInfo(int pupil_l, int pupil_r);
    // 瞳孔画像の種類を取得するメソッド(マスター側)
    bool getEyePupilInfo(int* pupil_l, int* pupil_r);
    // 瞼画像の種類設定用メソッド(マスター側)
    void setEyelidInfo(int eyelid_l, int eyelid_r);
    // 瞼画像の種類を取得するメソッド(マスター側)
    bool getEyelidInfo(int* eyelid_l, int* eyelid_r);
    // 瞬きを発生するまでの時間(ms)設定用メソッド(マスター側)
    void setEyeBlinkTime(int blinkTime);
    // 瞬きを発生するまでの時間(ms)を取得するメソッド(マスター側)
    bool getEyeBlinkTime(int* blinkTime);
    // ディスプレイの明るさ設定用メソッド(マスター側)
    void setEyeBrightness(int brightness);
    // ディスプレイの明るさを取得するメソッド(マスター側)
    bool getEyeBrightness(int* brightness);
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
    inline static mode_stepper mode;
    inline static bool updateInfo;
    AvatarTranstexhter_wire_core wireCore;
    static AvatarTranstexhterStepperSlave* wireCoreInstance;
    // マスター側から通信を受け取るメソッド(スレーブ側)
    static void receiveStepperInfo(int receiveByte);
  public:
    AvatarTranstexhterStepperSlave(TwoWire* wire) : wireCore(wire, STEPPER_MODULE){
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
    void setMode(mode_stepper m){
      mode = m;
    }
    mode_stepper getMode(){
      return mode;
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
    AvatarTranstexhterDcMotorSlave(TwoWire* wire) : wireCore(wire, DC_MOTOR_MODULE){
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
    bool updateDcMotor(){
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
    AvatarTranstexhterMouthSlave(TwoWire* wire) : wireCore(wire, MOUTH_MODULE){
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
    bool updateMouth(){
      return updateInfo;
    }
};

class AvatarTranstexhterEyeSlave{
  private:
    inline static int servo_L;
    inline static int servo_R;
    inline static int pupil_L;
    inline static int pupil_R;
    inline static int eyelid_L;
    inline static int eyelid_R;
    inline static int blinkTime;
    inline static int brightness;
    inline static int tmpServo_L;
    inline static int tmpServo_R;
    inline static int tmpPupil_L;
    inline static int tmpPupil_R;
    inline static int tmpEyelid_L;
    inline static int tmpEyelid_R;
    inline static int tmpBlinkTime;
    inline static int tmpBrightness;
    inline static bool updateInfo;
    AvatarTranstexhter_wire_core wireCore;
    static AvatarTranstexhterEyeSlave* wireCoreInstance;
    // マスター側から通信を受け取るメソッド(スレーブ側)
    static void receiveEyeInfo(int receiveByte);
  public:
    AvatarTranstexhterEyeSlave(TwoWire* wire) : wireCore(wire, EYE_MODULE){
      wireCoreInstance = this;
    }
    // メンバ変数取得・設定用メソッド
    void setTmpServoL(int sl){
      tmpServo_L = sl;
    }
    int getTmpServoL(){
      return tmpServo_L;
    }
    void setTmpServoR(int sr){
      tmpServo_R = sr;
    }
    int getTmpServoR(){
      return tmpServo_R;
    }
    void setTmpPupilL(int pl){
      tmpPupil_L = pl;
    }
    int getTmpPupilL(){
      return tmpPupil_L;
    }
    void setTmpPupilR(int pr){
      tmpPupil_R = pr;
    }
    int getTmpPupilR(){
      return tmpPupil_R;
    }
    void setTmpEyelidL(int el){
      tmpEyelid_L = el;
    }
    int getTmpEyelidL(){
      return tmpEyelid_L;
    }
    void setTmpEyelidR(int er){
      tmpEyelid_R = er;
    }
    int getTmpEyelidR(){
      return tmpEyelid_R;
    }
    void setTmpBlinkTime(int bt){
      tmpBlinkTime = bt;
    }
    int getTmpBlinkTime(){
      return tmpBlinkTime;
    }
    void setTmpBrightness(int br){
      tmpBrightness = br;
    }
    int getTmpBrightness(){
      return tmpBrightness;
    }
    void setServoL(int sl){
      servo_L = sl;
    }
    int getServoL(){
      updateInfo = false;
      return servo_L;
    }
    void setServoR(int sr){
      servo_R = sr;
    }
    int getServoR(){
      updateInfo = false;
      return servo_R;
    }
    void setPupilL(int pl){
      pupil_L = pl;
    }
    int getPupilL(){
      updateInfo = false;
      return pupil_L;
    }
    void setPupilR(int pr){
      pupil_R = pr;
    }
    int getPupilR(){
      updateInfo = false;
      return pupil_R;
    }
    void setEyelidL(int el){
      eyelid_L = el;
    }
    int getEyelidL(){
      updateInfo = false;
      return eyelid_L;
    }
    void setEyelidR(int er){
      eyelid_R = er;
    }
    int getEyelidR(){
      updateInfo = false;
      return eyelid_R;
    }
    void setBlinkTime(int bt){
      blinkTime = bt;
    }
    int getBlinkTime(){
      updateInfo = false;
      return blinkTime;
    }
    void setBrightness(int br){
      brightness = br;
    }
    int getBrightness(){
      updateInfo = false;
      return brightness;
    }
    // 初期化変数
    void init(){
      wireCore.init();
      wireCore.core_wire->onReceive(receiveEyeInfo);
    }
    // I2Cから信号を受け取って一度も読み込まれていないか
    bool updateEye(){
      return updateInfo;
    }
};