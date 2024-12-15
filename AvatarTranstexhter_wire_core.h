#include <Wire.h>

#define MOUTH_ADDRESS 0x37
#define STEPPER_ADDRESS 0x38
#define DC_MOTOR_ADDRESS 0x39

enum role_module{
  MASTER,
  MOUTH_MODULE,
  STEPPER_MODULE,
  DC_MOTOR_MODULE,
  dummy_role_module
};

class AvatarTranstexhter_wire_core{
  private:
    TwoWire* core_wire;
    role_module core_role;

  public:
    AvatarTranstexhter_wire_core(TwoWire* wire, role_module role){
      core_wire = wire;
      core_role = role;
      switch(core_role){
        case MASTER:
          core_wire->begin();
        break;
        case MOUTH_MODULE:
          core_wire->begin(MOUTH_ADDRESS);
        break;
        case  STEPPER_MODULE:
          core_wire->begin(STEPPER_ADDRESS);
        break;
        case DC_MOTOR_ADDRESS:
          core_wire->begin(DC_MOTOR_ADDRESS);
        break;
      }
    }

    //ステッピングモーター速度調整用コマンド(マスター側)

    
};
