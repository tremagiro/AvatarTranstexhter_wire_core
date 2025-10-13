# AvatarTranstexhter_wire_core
アバタートランステクターの各子機との通信で使用する処理をライブラリとしてまとめた。具体的には「マウスモジュール」「ステッピングモーターモジュール」「DCモーターモジュール」の３モジュールの通信に関わる設定値や処理方法を定めている。
## インストールが必要なライブラリ
* ArrayQueueStack(https://github.com/robotty1234/ArrayQueueStack)
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
