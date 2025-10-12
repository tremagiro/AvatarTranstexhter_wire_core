# AvatarTranstexhter_wire_core
アバタートランステクターの各子機との通信で使用する処理をライブラリとしてまとめた。具体的には「マウスモジュール」「ステッピングモーターモジュール」「DCモーターモジュール」の３モジュールの通信に関わる設定値や処理方法を定めている。
## ３モジュール共通
### コンストラクタ
- wire : 子機・親機と通信する際に使用するWireのポインタ
- role : 該当ボードの役割を以下から選択する
    - MASTER : 親機
    - MOUTH_MODULE : マウスモジュール
    - STEPPER_MODULE : ステッピングモーターモジュール
    - DC_MOTOR_MODULE : DCモーターモジュール
- serial_speed : 各メソッドのでバックメッセージを表示するシリアル通信の速度を入力(単位 : baud) (デフォルト値 : 0 (0の場合はデバッグメッセージを表示しなくなる))
```
AvatarTranstexhter_wire_core(TwoWire* wire, role_module role = MASTER, int serial_speed = 0)
```
### メソッド
- 初期化を行うメソッド
```
void init();
```
- I2C経由でコマンドを送信するメソッド 
    - address：送信先のI2Cアドレス
    - command：メソッドを選択するためのコマンド
    - value：パラメーター
```
void sent_wire(uint64_t address, uint64_t command, uint64_t value);
```
- I2C経由でコマンドを受信し、値を渡すメソッド
    - *command：受信したコマンドを受け取るポインタ
    - *value：受信したパラメーターを受け取るポインタ
void receive_read(uint64_t *command, uint64_t *value);
```

## ステッピングモーター関連
### 設定値
- ステッピングモータ動作子機のアドレス
```
#define STEPPER_ADDRESS 0x38
```
- ステッピングモーターの回転速度を設定するコマンド
```
#define SET_STEPPER_SPEED 0x01
```
- ステッピングモーターの回転角度を設定するコマンド
```
#define SET_STEPPER_STEP  0x02
```
- 現在のステッピングモーターの回転速度を取得するコマンド
```
#define GET_STEPPER_SPEED  0x03
```
- 現在のステッピングモーターの回転角度を取得するコマンド
```
#define GET_STEPPER_STEP 0x04
```
- 現在のDCモーターの回転角度を取得するコマンド
```
#define SET_DC_MOTOR_SPEED 0x05
```
- 現在のDCモーターの回転角度を取得するコマンド
```
#define GET_DC_MOTOR_SPEED 0x06
```

### メソッド
- ステッピングモーター速度調整用メソッド(マスター側)
    - speed : ステッピングモーターの速度(単位 : rpms)
```
bool setStepperSpeed(uint64_t speed);
```
- ステッピングモーター回転用メソッド(マスター側)
    - step : ステッピングモーターの回転角度(単位 : ステップ)
```
bool setStepperStep(uint64_t step); 
```
- ステッピングモーターの設定速度を取得するメソッド(マスター側)
    - timeout : 受信を待つ時間(単位 : ms) (デフォルト値：0 (1回チェックして受信していなければ0を返す))
```
uint64_t getStepperSpeed(int timeout = 0);
```
- ステッピングモーターの回転角を取得するメソッド(マスター側)
    - timeout : 受信を待つ時間(単位 : ms) (デフォルト値：0 (1回チェックして受信していなければ0を返す))
```
uint64_t getStepperStep(int timeout = 0);
```
- ステッピングモーター制御用デバイス受信メソッド(スレーブ側)
    - cmd : 受信したコマンドを格納する変数のポインタ
    - value : 受信した数値を格納する変数のポインタ
```
bool receiveStepperModule(uint64_t* cmd, uint64_t* value); 
```
- スレーブ側からステッピングモーターの情報を送信メソッド(スレーブ側)
    - value : マスターへ送信する値
```
bool sentInfoStpper(uint64_t value);
```

## DCモーター関連
#define SET_DC_MOTOR_SPEED 0x05
#define GET_DC_MOTOR_SPEED 0x06
DCモーターの速度調整用コマンド(マスター側)
bool setDcMotorSpeed(int speed_l, int speed_r);

DCモーターの設定速度を取得するコマンド(マスター側)
bool getDcMotorSpeed(int* speed_l, int* speed_r, int timeout = 0);

DCモーター制御用デバイス受信コマンド(スレーブ側)
bool receiveDcMotorModule(String* cmd, int* value_l, int* value_r); 

スレーブ側からDCモーターの情報を送信(スレーブ側)
bool sentInfoDcMotor(long value);