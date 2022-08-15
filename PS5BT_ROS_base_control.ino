/*
 *     0         0
 *     |         |
 *  0-----255 0-----255
 *     |         |
 *    255       255
 */
#include <PS5BT.h>
#include <usbhub.h>
#include <SPI.h>
USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the PS5BT class in two ways */
// This will start an inquiry and then pair with the PS5 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS5 controller will then start to blink rapidly indicating that it is in pairing mode
//PS5BT PS5(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
PS5BT PS5(&Btd);
uint32_t ps_timer;
uint16_t lastMessageCounter = -1;
double Vx = 0.0, Vy = 0.0, W = 0.0;
#define max_linear_speed  0.2 // m/s
#define max_angular_speed 0.8 // rad/s
float linear_speed_threshold = max_linear_speed * 10 / 127.5;
float angular_speed_threshold = max_angular_speed * 10 / 127.5;
/*-----ROS & rosserial-----------------------------------------------------------------------------*/ 
#include "ros.h"
#include <geometry_msgs/Twist.h>
class NewHardware : public ArduinoHardware
{
  public:
  NewHardware():ArduinoHardware(&Serial1, 500000){};
};
ros::NodeHandle_<NewHardware>  nh;
geometry_msgs::Twist base_speed;
ros::Publisher PS5_controller("/cmd_vel", &base_speed);
/*-----arduino function----------------------------------------------------------------------------*/
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float zero_speed_threshold(float x, float threshold){
  if(x < -threshold or x > threshold) return x;
  else return 0;
}

void print_Hat(){
  Serial.print(F("\r\nLeftHatX: "));
  Serial.print(PS5.getAnalogHat(LeftHatX));
  Serial.print(F("\tLeftHatY: "));
  Serial.print(PS5.getAnalogHat(LeftHatY));
  Serial.print(F("\tRightHatX: "));
  Serial.print(PS5.getAnalogHat(RightHatX));
  Serial.print(F("\tRightHatY: "));
  Serial.print(PS5.getAnalogHat(RightHatY));
}

void print_Hat_and_speed(){
  Serial.print(F("\r\nLeftHatY: "));Serial.print(PS5.getAnalogHat(LeftHatY));
  Serial.print(F("\tVx: "));Serial.print(Vx, 4);
  Serial.print(F("\tLeftHatX: "));  Serial.print(PS5.getAnalogHat(LeftHatX));
  Serial.print(F("\tVy: "));Serial.print(Vy, 4);
  Serial.print(F("\tRightHatX: ")); Serial.print(PS5.getAnalogHat(RightHatX));
  Serial.print(F("\tW: ")); Serial.print(W, 4);
}

void PS_button_disconnect(){
  // Hold the PS button for 1 second to disconnect the controller
  // This prevents the controller from disconnecting when it is reconnected,
  // as the PS button is sent when it reconnects
  if (PS5.getButtonPress(PS)){
    if (millis() - ps_timer > 1000)
    PS5.disconnect();
  } 
  else
    ps_timer = millis();

  if (PS5.getButtonClick(PS))
    Serial.print(F("\r\nPS"));
}
/*----------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS5 Bluetooth Library Started"));
  /*-----ROS & rosserial-----*/
  nh.initNode();
  nh.advertise(PS5_controller);
}

void loop() {
  Usb.Task();
  PS5.setLed(Green);
  if (PS5.connected() && lastMessageCounter != PS5.getMessageCounter()) {
    lastMessageCounter = PS5.getMessageCounter();
//    print_Hat();
    Vx = map_float(PS5.getAnalogHat(LeftHatY), 0, 255, max_linear_speed, -max_linear_speed);
    Vx = zero_speed_threshold(Vx, linear_speed_threshold);
    Vy = map_float(PS5.getAnalogHat(LeftHatX), 0, 255, max_linear_speed, -max_linear_speed);
    Vy = zero_speed_threshold(Vy, linear_speed_threshold);
    W  = map_float(PS5.getAnalogHat(RightHatX), 0, 255, max_angular_speed, -max_angular_speed);
    W  = zero_speed_threshold(W , angular_speed_threshold);
    print_Hat_and_speed();
  }
  /*-----ROS & rosserial-----*/
  base_speed.linear.x  = Vx;
  base_speed.linear.y  = Vy;
  base_speed.angular.z = W;
  PS5_controller.publish( &base_speed );
  nh.spinOnce();
  delay(10);
}
