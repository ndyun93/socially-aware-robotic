#include <MyCobotBasic.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();// (26/1024)*20ms ≒ 0.5 ms  (-90°)
#define SERVOMIN 102
// 下で設定するPWM設定周波数を4096で分割したタイミングで制御する最小パルス幅　：4096/20×0.5＝102.4　（0.5ms：SG90、0°の時のパルス幅）
#define SERVOMAX 492
// 下で設定するPWM設定周波数を4096で分割したタイミングで制御する最大パルス幅　：4096/20×2.4＝491.5　（2.4ms：SG90、180°の時のパルス幅）
int sk = 0;


MyCobotBasic myCobot;
//------------j-1,j2, j3,j4,j5,j6
Angles angles = {0, 0, 0, 0, 0, 0};
Angles bow = { 0, -50, -50, -45, 0, 0};
Angles poi = {  0, 0,50,  50, 0, -90};
// ----------------------------------------------------
Angles pick ={15, -45, -90,-45, -150,-180}; // pick -> p1
//Angles up_alt ={15, -32.5, -115,-32.5, -160,-180};
Angles pick1 ={15, -25, -130,-25, -150,-180};
Angles back ={-45, -25, -130,-25, -150,-180};
Angles up1 ={15, 0, -90,-90, -150,-180};

Angles down ={-45, -45, -90,-45, -150,-180};
// -
Angles pick2 ={15, -90,   0, -90, -150,-180};
Angles up2 =  {15,   0, -90, -90, -150,-180};
Angles down2 ={-15, -90, 0,-90, -150,-180};
Angles up2_2 ={-15, 0, -90,-90, -150,-180};


//--servo motor setup for pwm--

#define SERVO_PIN_0 19 //m5stack gpio19
#define SERVO_CH_0 0
int deg = 0;
int flag = 0;


//int minimum = 26;
//// (123/1024)*20ms ≒ 2.4 ms (+90°)
//int maximum = 123;

//pwm servo
//void servo(int degree) {
//  ledcWrite(SERVO_CH_0, map(degree, -90, 90, minimum, maximum));
//  delay(200);
//  ledcWrite(SERVO_CH_0, 0);
//  drawScreen();
//}

void drawScreen() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("%d", deg);
  M5.Lcd.setCursor(0, 240 - 18);
  M5.Lcd.print("    +        0       -");
}

void setup()
{
  M5.begin();

  //M5.begin(true, false, true);
  dacWrite(25, 0); //　ノイズ対策

  ledcSetup(SERVO_CH_0, 50, 10);
  ledcAttachPin(SERVO_PIN_0, SERVO_CH_0);
  Wire.begin(); //i2cのpin宣言が重要 SDA, SCL
  pwm.begin();
  pwm.setPWMFreq(50); // SG90は 50 Hz で動く（PWM周波数設定）

  M5.Lcd.setTextSize(2);
  M5.Lcd.setBrightness(40);
  drawScreen();




  //---------------------------------------------------------
  myCobot.setup(); //This api is required
  delay(100);
  myCobot.powerOn();//robot poweron
  delay(100);

  myCobot.writeAngles(angles, 30); //All joints to zero, speed 50
  delay(5000);
  //----------------------------------------------------------

  myCobot.setLEDRGB(0XFF, 0, 0);
  delay(500);
  myCobot.setLEDRGB(0, 0XFF, 0);
  delay(500);
  myCobot.setLEDRGB(0, 0, 0XFF);
  delay(500);
  myCobot.writeAngle((Joint)1, 100, 30); //Single joint control，J1 to 100°,speed 30
  delay(5000);
  // ----done ------
  myCobot.writeAngle((Joint)1, 0, 30);
  delay(5000);
  grip(2);
  delay(1000);
   grip(1);
}

void loop()
{
  M5.update();
  if ( M5.BtnA.wasPressed() ) {
//    sorry(); not
//    delay(1000);
//    s1();
//   init_ang_all();

default_state();

  }

  if ( M5.BtnB.wasPressed() ) {
    delay(100);
  d();
  }
  if ( M5.BtnC.wasPressed()) {
    delay(200);
//    sorry();
    p2();
    delay(1000);
//    init_ang_all();
    }
  delay(1);

}


void servo_write(int ch, int ang) { //動かすサーボチャンネルと角度を指定 for pca
  ang = map(ang, -90, 90, SERVOMIN, SERVOMAX); //角度（0～180）をPWMのパルス幅（150～500）に変換
  pwm.setPWM(ch, 0, ang);
}

//type 1 for robot hand 
//type 2 for human hand
void grip_test(int type) {
  if ( type == 1) {
    servo_write(0, 90);
    delay(1000);
    servo_write(0, 0);
    delay(1000);
  }
  if (type == 2) {
    // 11 -15
    servo_write(11, 90);
    servo_write(12, 10);
    servo_write(13, 10);
    servo_write(14, 10);
    servo_write(15, 10);
    delay(1000);
//    off
    servo_write(11, 10);
    servo_write(12, 90);
    servo_write(13, 90);
    servo_write(14, 90);
    servo_write(15, 90);
    delay(1000);
  }
}

// 1 = free
// 2= grip
void grip(int m){
    if(m == 1){
        servo_write(0, 90);
        servo_write(11, 90);
        servo_write(12, -30);
        servo_write(13, -30);
        servo_write(14, -30);
        servo_write(15, -30);
        delay(1000);
      }
    if(m == 2){
        servo_write(0,  10);
        servo_write(11, 10);
        servo_write(12, 90);
        servo_write(13, 90);
        servo_write(14, 90);
        servo_write(15, 90);
        delay(1000);
      }
}
  
void init_ang_all(){
    servo_write(0, 0); //robot
    servo_write(11, 10); //human
    servo_write(12, 90);
    servo_write(13, 90);
    servo_write(14, 90);
    servo_write(15, 90);
    myCobot.writeAngles(angles, 20);
    
}

void throw_robot_hand(){
  //throw 
    delay(10);
    //            servo(50);
    servo_write(0, -5);
    
    delay(2000);
    myCobot.writeAngle((Joint)2, 50, 30);
    delay(1000);
    myCobot.writeAngle((Joint)3, 50, 30);
    delay(2000);
    servo_write(0, 40);
    delay(2000);
    myCobot.writeAngles(angles, 20);
    delay(5000);
    servo_write(0, 0);
    delay(50);
}

void human_hand_sorry(){    // throw and sorry
//      delay(1);
//      grip(1);
//      delay(2000);
//      grip(2);
//      delay(10);
        delay(10);
    //            servo(50);
    
//     delay(2000);
    myCobot.writeAngle((Joint)6, -90, 30);  
    delay(5000);
    grip(1);
    delay(2000);
   myCobot.writeAngles(poi,30);
   delay(5000);
    grip(2);
    delay(5000);
    myCobot.writeAngles(angles, 20);
    delay(5000);
    grip(1);
    delay(6000);
    myCobot.writeAngle((Joint)3, -40, 30);
    delay(10);
    }


void sorry(){
  for(int j =2; j<5; j++){
    myCobot.writeAngle((Joint)j, -45, 30);
    delay(1000);
  }
}

void s1(){
  for(int j =2; j<5; j++){
    myCobot.writeAngle((Joint)j, -45, 30);
    delay(5000);
    myCobot.writeAngles(angles, 20);
    delay(5000);
  }
}
void default_state(){
  delay(500);
  myCobot.writeAngles(pick1, 50); //20
  delay(5000);
  grip(1); //open
}
//picking and drop
//              j1,  j2,  j3, j4 , j5, j6       
//Angles pick ={15, -45, -90,-45, -160,-180};
//Angles up1={15, 0, -90,-90, -160,-180};
//Angles down ={-45, -45, -90,-45, -160,-180};

// Angles back ={-45, -25, -130,-25, -160,-180};


//Angles pick2 ={15, -90, 0,-90, -160,-180};
//Angles up2 ={15, -90, 0,-90, -160,-180};
// Angles down2 ={-15, -45, -90,-45, -160,-180};
// Angles up2_2 ={-15, 0, -90,-90, -160,-180};
void p1(){
  delay(500);
//  default position
  myCobot.writeAngles(pick, 20);
//  delay(8000);
  delay(2000);
  grip(2); //grip
  delay(2000);
  myCobot.writeAngles(up1, 20);
  delay(2000);
  myCobot.writeAngle((Joint)1, -45, 20);
  delay(2000);
  myCobot.writeAngles(down, 20);
  delay(5000);
  grip(1);
  delay(2000);
  myCobot.writeAngles(back, 20);
  delay(5000);
  myCobot.writeAngle((Joint)1, 15, 20);
  delay(2000);
//  ------2nd bottle
myCobot.writeAngles(pick2, 20);
delay(5000);
grip(2);
delay(2000);
myCobot.writeAngles(up2, 20);
delay(2000);
myCobot.writeAngle((Joint)1, -15, 20);
delay(2000);
myCobot.writeAngles(down2, 20);
delay(5000);
grip(1);
delay(2000); 
myCobot.writeAngles(up2_2, 20);
delay(2000); 
myCobot.writeAngle((Joint)1, 0, 20);
delay(2000); 
myCobot.writeAngles(pick, 20);
delay(5000);
grip(2);
}
//  -------------------------


void p2(){
  delay(500);
//  default position
  myCobot.writeAngles(pick1, 20);
//  delay(8000);
  delay(2000);
//  grip(2); //grip
  servo_write(0, 10);
  delay(2000);
  myCobot.writeAngles(up1, 70);
  delay(2000);
  myCobot.writeAngle((Joint)1, -45, 20);
  delay(2000);
  myCobot.writeAngles(down, 40);
  delay(5000);
  grip(1);
  delay(2000);
  myCobot.writeAngles(up1, 70);
  delay(2000);
  myCobot.writeAngle((Joint)1, 15, 20);
  delay(2000);
//  ------2nd bottle
myCobot.writeAngles(pick2, 20);
delay(5000);
grip(2);
delay(2000);
myCobot.writeAngles(up2, 70);
delay(2000);
myCobot.writeAngle((Joint)1, -15, 20);
delay(2000);
myCobot.writeAngles(down2, 40);
delay(5000);
grip(1);
delay(2000); 
myCobot.writeAngles(up2_2, 70);
delay(2000); 
myCobot.writeAngle((Joint)1, 0, 20);
delay(2000); 
myCobot.writeAngles(pick1, 20);
delay(5000);
grip(2);
  }

void d(){
myCobot.writeAngles(pick1, 20);
  delay(2000);
  grip(2); //grip
  delay(2000);
  myCobot.writeAngles(up1, 20);
  delay(2000);
  myCobot.writeAngle((Joint)1, -45, 20);
  delay(1000);
  grip(1);

}
