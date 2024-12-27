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
## ステッピングモーター関連
### 設定値
- ステッピングモータ動作子機のアドレス
```
#define STEPPER_ADDRESS 0x38
```
- ステッピングモーターの回転速度を設定するコマンド
```
String SET_STEPPER_SPEED = "SET_STEPPER_SPEED";
```
- ステッピングモーターの回転角度を設定するコマンド
```
String SET_STEPPER_STEP =  "SET_STEPPER_STEP";
```
- 現在のステッピングモーターの回転速度を取得するコマンド
```
String GET_STEPPER_SPEED =  "GET_STEPPER_SPEED";
```
- 現在のステッピングモーターの回転角度を取得するコマンド
```
String GET_STEPPER_STEP =  "GET_STEPPER_STEP";
```
### メソッド
- ステッピングモーター速度調整用メソッド(マスター側)
    - speed : ステッピングモーターの速度(単位 : rpms)
```
boolean setStepperSpeed(long speed);
```
- ステッピングモーター回転用メソッド(マスター側)
    - step : ステッピングモーターの回転角度(単位 : ステップ)
```
boolean setStepperStep(long step); 
```
- ステッピングモーターの設定速度を取得するメソッド(マスター側)
    - timeout : 受信を待つ時間(単位 : ms) (デフォルト値：0 (1回チェックして受信していなければ0を返す))
```
long getStepperSpeed(int timeout = 0);
```
- ステッピングモーターの回転角を取得するメソッド(マスター側)
    - timeout : 受信を待つ時間(単位 : ms) (デフォルト値：0 (1回チェックして受信していなければ0を返す))
```
long getStepperStep(int timeout = 0);
```
- ステッピングモーター制御用デバイス受信メソッド(スレーブ側)
    - cmd : 受信したコマンドを格納する変数のポインタ
    - value : 受信した数値を格納する変数のポインタ
```
boolean receiveStepperModule(String* cmd, long* value); 
```
- スレーブ側からステッピングモーターの情報を送信メソッド(スレーブ側)
    - value : マスターへ送信する値
```
boolean sentInfoStpper(long value);
```