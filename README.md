# AvatarTranstexhter_wire_core
アバタートランステクターの各子機との通信で使用する処理をライブラリとしてまとめた。具体的には「マウスモジュール」「ステッピングモーターモジュール」「DCモーターモジュール」「瞳モジュール」の４モジュールの通信に関わる設定値や処理方法を定めている。
## 動作確認済みボード
- Raspberry Pi Pico/RP2040/RP2350 5.5.0(https://github.com/earlephilhower/arduino-pico)
    - Raspberry Pi Pico W
    - Raspberry Pi Pico 2 W 
    - Waveshare RP2350 Zero
- Seeeduino SAMD Boards 1.2.0(https://www.seeedstudio.com/)
    - Seeeduino XIAO
- Seeeduino nRF52 mbed-enabled boards 2.9.3(https://www.seeedstudio.com/)
    - XIAO nRF52840
## ４モジュール共通
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
- role : 該当ボードの役割を以下から選択する
    - MASTER : 親機
    - MOUTH_MODULE : マウスモジュール
    - STEPPER_MODULE : ステッピングモーターモジュール
    - DC_MOTOR_MODULE : DCモーターモジュール
    - EYE_MODULE : 瞳モジュール

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
- 受信したcmdとvalueを格納する。最大 `LIMIT_TRY_TIMES` 回まで受信を試し、チェックサムの合った値を受け取れたら true、受け取れなかった場合（スレーブが接続されていない場合など）は false を返す
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
void setMode(mode_stepper m);
mode_stepper getMode();
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
## 瞳モジュール
両目のサーボ、瞳孔画像・瞼画像（左右別々）、瞬き、ディスプレイの明るさを制御する子機（RP2350）との通信。I2Cアドレスは `0x3a`。

値はほかのモジュールと同じく1コマンドにつき1つ（int16）で送る。値の意味（角度の範囲、画像の番号、瞬きの時間が0のときの扱いなど）はスレーブ側のスケッチで判断する。

| 値 | 左右 | 内容 |
| --- | --- | --- |
| サーボ | 別々 | 両目サーボの角度 |
| 瞳孔 | 別々 | 瞳孔画像の種類 |
| 瞼 | 別々 | 瞼画像の種類 |
| 瞬き | 共通 | 瞬きを発生するまでの時間(ms)。送れる最大値は32640 |
| 明るさ | 共通 | ディスプレイの明るさ |
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
```
AvatarTranstexhterEyeSlave(TwoWire* wire)
```
### メソッド（マスター側）
- 両目サーボの角度設定・取得用メソッド(マスター側)
```
void setEyeServoInfo(int angle_l, int angle_r);
bool getEyeServoInfo(int* angle_l, int* angle_r);
```
- 瞳孔画像の種類設定・取得用メソッド(マスター側)
```
void setEyePupilInfo(int pupil_l, int pupil_r);
bool getEyePupilInfo(int* pupil_l, int* pupil_r);
```
- 瞼画像の種類設定・取得用メソッド(マスター側)
```
void setEyelidInfo(int eyelid_l, int eyelid_r);
bool getEyelidInfo(int* eyelid_l, int* eyelid_r);
```
- 瞬きを発生するまでの時間(ms)設定・取得用メソッド(マスター側)
```
void setEyeBlinkTime(int blinkTime);
bool getEyeBlinkTime(int* blinkTime);
```
- ディスプレイの明るさ設定・取得用メソッド(マスター側)
```
void setEyeBrightness(int brightness);
bool getEyeBrightness(int* brightness);
```
### メソッド（スレーブ側）
- メンバ変数取得・設定用メソッド
```
void setTmpServoL(int sl);
int getTmpServoL();
void setTmpServoR(int sr);
int getTmpServoR();
void setTmpPupilL(int pl);
int getTmpPupilL();
void setTmpPupilR(int pr);
int getTmpPupilR();
void setTmpEyelidL(int el);
int getTmpEyelidL();
void setTmpEyelidR(int er);
int getTmpEyelidR();
void setTmpBlinkTime(int bt);
int getTmpBlinkTime();
void setTmpBrightness(int br);
int getTmpBrightness();
void setServoL(int sl);
int getServoL();
void setServoR(int sr);
int getServoR();
void setPupilL(int pl);
int getPupilL();
void setPupilR(int pr);
int getPupilR();
void setEyelidL(int el);
int getEyelidL();
void setEyelidR(int er);
int getEyelidR();
void setBlinkTime(int bt);
int getBlinkTime();
void setBrightness(int br);
int getBrightness();
```
- 初期化変数
```
void init();
```
- I2Cから信号を受け取って一度も読み込まれていないか
```
bool updateEye();
```
### コマンド
| コマンド | 値 | 内容 |
| --- | --- | --- |
| `SET_EYE_SERVO_L` / `SET_EYE_SERVO_R` | 0x0f / 0x10 | 左右サーボの角度を設定 |
| `GET_EYE_SERVO_L` / `GET_EYE_SERVO_R` | 0x11 / 0x12 | 左右サーボの角度を取得 |
| `SET_EYE_PUPIL_L` / `SET_EYE_PUPIL_R` | 0x13 / 0x14 | 左右の瞳孔画像を設定 |
| `GET_EYE_PUPIL_L` / `GET_EYE_PUPIL_R` | 0x15 / 0x16 | 左右の瞳孔画像を取得 |
| `SET_EYE_EYELID_L` / `SET_EYE_EYELID_R` | 0x17 / 0x18 | 左右の瞼画像を設定 |
| `GET_EYE_EYELID_L` / `GET_EYE_EYELID_R` | 0x19 / 0x1a | 左右の瞼画像を取得 |
| `SET_EYE_BLINK_TIME` / `GET_EYE_BLINK_TIME` | 0x1b / 0x1c | 瞬きを発生するまでの時間を設定・取得 |
| `SET_EYE_BRIGHTNESS` / `GET_EYE_BRIGHTNESS` | 0x1d / 0x1e | ディスプレイの明るさを設定・取得 |
### サンプル
- `examples/eye_master` : 全項目を設定し、取得した値を表示する（マスター側）
- `examples/eye_slave` : 受け取った値を表示する（スレーブ側）
