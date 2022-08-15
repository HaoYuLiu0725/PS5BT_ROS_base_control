# 1 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
/*

 *     0         0

 *     |         |

 *  0-----255 0-----255

 *     |         |

 *    255       255

 */
# 8 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
# 9 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 2
# 10 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 2
# 11 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 2
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


float linear_speed_threshold = 0.2 /* m/s*/ * 10 / 127.5;
float angular_speed_threshold = 0.8 /* rad/s*/ * 10 / 127.5;
/*-----ROS & rosserial-----------------------------------------------------------------------------*/
# 29 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 2
# 30 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 2
ros::NodeHandle nh;
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
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 45 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 45 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\r\nLeftHatX: "
# 45 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 45 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));
  Serial.print(PS5.getAnalogHat(LeftHatX));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 47 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 47 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tLeftHatY: "
# 47 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 47 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));
  Serial.print(PS5.getAnalogHat(LeftHatY));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 49 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 49 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tRightHatX: "
# 49 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 49 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));
  Serial.print(PS5.getAnalogHat(RightHatX));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 51 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 51 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tRightHatY: "
# 51 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 51 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));
  Serial.print(PS5.getAnalogHat(RightHatY));
}

void print_Hat_and_speed(){
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 56 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 56 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\r\nLeftHatY: "
# 56 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 56 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));Serial.print(PS5.getAnalogHat(LeftHatY));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 57 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 57 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tVx: "
# 57 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 57 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));Serial.print(Vx, 4);
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 58 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 58 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tLeftHatX: "
# 58 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 58 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              ))); Serial.print(PS5.getAnalogHat(LeftHatX));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 59 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 59 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tVy: "
# 59 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 59 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));Serial.print(Vy, 4);
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 60 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 60 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tRightHatX: "
# 60 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 60 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              ))); Serial.print(PS5.getAnalogHat(RightHatX));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 61 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 61 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\tW: "
# 61 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 61 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              ))); Serial.print(W, 4);
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
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 76 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 76 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
                "\r\nPS"
# 76 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
                ); &__c[0];}))
# 76 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
                )));
}
/*----------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 82 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 82 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
                "\r\nOSC did not start"
# 82 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
                ); &__c[0];}))
# 82 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
                )));
    while (1); // Halt
  }
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 85 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 85 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              "\r\nPS5 Bluetooth Library Started"
# 85 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino" 3
              ); &__c[0];}))
# 85 "c:\\Users\\lhjh0\\OneDrive\\桌面\\PS5BT_ROS_base_control\\PS5BT_ROS_base_control.ino"
              )));
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
    Vx = map_float(PS5.getAnalogHat(LeftHatY), 0, 255, 0.2 /* m/s*/, -0.2 /* m/s*/);
    Vx = zero_speed_threshold(Vx, linear_speed_threshold);
    Vy = map_float(PS5.getAnalogHat(LeftHatX), 0, 255, 0.2 /* m/s*/, -0.2 /* m/s*/);
    Vy = zero_speed_threshold(Vy, linear_speed_threshold);
    W = map_float(PS5.getAnalogHat(RightHatX), 0, 255, 0.8 /* rad/s*/, -0.8 /* rad/s*/);
    W = zero_speed_threshold(W , angular_speed_threshold);
    print_Hat_and_speed();
  }
  /*-----ROS & rosserial-----*/
  PS5_controller.publish( &base_speed );
  nh.spinOnce();
}
