#include "sdkconfig.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"
#include "MotorMovements.h"

#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
#define IN3  19 // Control pin 3
#define IN4  23 // Control pin 4

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

void GoForward(int duty){
    
    
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);

}

void GoBackward(int duty){
  
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
    
}

void GoLeft(int duty){
    
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);


}

void GoRight(int duty){
    
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
}

void Stop(){
    
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    

}




