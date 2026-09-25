#include <AvatarTranstexhter_wire_core.h>

AvatarTranstexhterEyeSlave myEye = AvatarTranstexhterEyeSlave(&Wire);

void setup() {
  // put your setup code here, to run once:
  Wire.setSDA(16);
  Wire.setSCL(17);
  Serial.begin(115200);
  myEye.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myEye.updateEye() == true){
    // ボードによってはprintfで一度に出せる文字数が少ないため、行を分けて表示する
    Serial.printf("【設定値】servo : %d / %d, pupil : %d / %d\n",
      myEye.getServoL(), myEye.getServoR(), myEye.getPupilL(), myEye.getPupilR());
    Serial.printf("　　　　　eyelid : %d / %d, blinkTime : %d, brightness : %d\n",
      myEye.getEyelidL(), myEye.getEyelidR(), myEye.getBlinkTime(), myEye.getBrightness());
  }else{
    Serial.println("未検出");
    delay(500);
  }
}
