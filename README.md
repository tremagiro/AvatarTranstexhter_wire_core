# AvatarTranstexhter_wire_core
アバタートランステクターの各子機との通信で使用する処理をライブラリとしてまとめた。具体的には「マウスモジュール」「ステッピングモーターモジュール」「DCモーターモジュール」の３モジュールの通信に関わる設定値や処理方法を定めている。
## 動作確認済みボード
- Raspberry Pi Pico/RP2040/RP2350 5.5.0(https://github.com/earlephilhower/arduino-pico)
    - Raspberry Pi Pico W
    - Raspberry Pi Pico 2 W 
    - Waveshare RP2350 Zero
- Seeeduino SAMD Boards 1.2.0(https://www.seeedstudio.com/)
    - Seeeduino XIAO
- Seeeduino nRF52 mbed-enabled boards 2.9.3(https://www.seeedstudio.com/)
    - XIAO nRF52840
## ３モジュール共通
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
- role : 該当ボードの役割を以下から選択する
    - MASTER : 親機
    - MOUTH_MODULE : マウスモジュール
    - STEPPER_MODULE : ステッピングモーターモジュール
    - DC_MOTOR_MODULE : DCモーターモジュール

```
AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER)
```
### メソッド（マスター側）
- 初期化を行うメソッド
```
void init();
```
- cmdとvalueを定めたルールに則って文字列にし、送信する
```
bool sent_wire(int address, int command, int value);
```
- 受信したcmdとvalueを格納する
```
bool receive_read(int* command, int* value);
```
### メソッド（スレーブ側）
- スレーブ受信用イベント
```
bool slaveReceiveEvent(int* command, int* value);
```
## マウスモジュール
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
```
AvatarTranstexhterMouthSlave(TwoWire* wire)
```
### メソッド（マスター側）
- フロントディスプレイ設定用メソッド（マスター側）
```
void setFrontDisplayInfo(int imageType, int loopTime);
```
- フロントディスプレイの設定を取得するメソッド（マスター側）
```
bool getFrontDisplayInfo(int* imageType, int* loopTime);
```
### メソッド（スレーブ側）
- メンバ変数取得・設定用メソッド
```
void setTmpImageType(int it);
int getTmpImageType();
void setTmpLoopTime(int lt);
int getTmpLoopTime();
void setImageType(int it);
int getImageType();
void setLoopTime(int lt);
int getLoopTime();
```
- 初期化変数
```
void init();
```
- I2Cから信号を受け取って一度も読み込まれていないか
```
bool updateMouth();
```
## ステッピングモジュール
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
```
AvatarTranstexhterStepperSlave(TwoWire* wire) 
```
### 列挙型
- ステッピングモーターの動作モードを示す列挙型
```
enum mode_stepper{
  DUAL,
  A,
  B
};
```
### メソッド（マスター側）
- ステッピングモーター設定用メソッド(マスター側)
```
void setStepperInfo(int speed, int step);
```
- ステッピングモーターの設定を取得するメソッド(マスター側)
```
bool getStepperInfo(int* sp, int* st);
```
- ステッピングモーターの動作モード設定用メソッド(マスター側)
```
void setStepperMode(mode_stepper mode);
```
- ステッピングモーターの動作モード取得用メソッド(マスター側)
```
bool getStepperMode(mode_stepper* mp);
```
### メソッド（スレーブ側）
- メンバ変数取得・設定用メソッド
```
void setTmpSpeed(int sp);
int getTmpSpeed();
void setTmpStep(int st);
int getTmpStep();
void setSpeed(int sp);
int getSpeed();
void setStep(int st);
int getStep();
```
- 初期化変数
```
void init();
```
- I2Cから信号を受け取って一度も読み込まれていないか
```
bool updateStepper();
```
## DCモーターモジュール
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
```
AvatarTranstexhterDcMotorSlave(TwoWire* wire) 
```
### メソッド（マスター側）
- DCモーターの速度調整用メソッド(マスター側)
```
void setDcMotorInfo(int speed_l, int speed_r);
```
- DCモーターの設定速度を取得するメソッド(マスター側)
```
bool getDcMotorInfo(int* speed_l, int* speed_r);
```
### メソッド（スレーブ側）
- メンバ変数取得・設定用メソッド
```
void setTmpSpeedL(int spl);
int getTmpSpeedL();
void setTmpSpeedR(int spr);
int getTmpSpeedR();
void setSpeedL(int spl);
int getSpeedL();
void setSpeedR(int spr);
int getSpeedR();
```
- 初期化変数
```
void init();
```
- I2Cから信号を受け取って一度も読み込まれていないか
```
bool updateDcMotor();
```

