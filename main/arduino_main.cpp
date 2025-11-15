// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "sdkconfig.h"
#include <Arduino.h>
#include "Arduino_APDS9960.h"
#include <ESP32Servo.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"
#include "MotorMovements.h"
#include<ESP32SharpIR.h>

#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
#define IN3  19 // Control pin 3
#define IN4  23 // Control pin 4

#define APDS9960_INT 0
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000
TwoWire I2C_0 = TwoWire(0);
APDS9960 sensor = APDS9960(I2C_0, APDS9960_INT);

ESP32SharpIR IRSensorRight(ESP32SharpIR::GP2Y0A21YK0F, 32);
ESP32SharpIR IRSensorFront(ESP32SharpIR::GP2Y0A21YK0F, 33);
ESP32SharpIR IRSensorLeft(ESP32SharpIR::GP2Y0A21YK0F, 25);

#include <QTRSensors.h>

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

void dumpGamepad(ControllerPtr ctl) {
    Console.printf(
        "DPAD: %d A: %d B: %d X: %d Y: %d LX: %d LY: %d RX: %d RY: %d L1: %d R1: %d L2: %d R2: %d\n",
        ctl->dpad(),        // D-pad
        ctl->a(),           // Letter buttons
        ctl->b(),
        ctl->x(),
        ctl->y(),
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->l1(),           // Bumpers
        ctl->r1(),
        ctl->l2(),
        ctl->r2()
    );
}

Servo myServo;

void RGBsetup() {
   I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
    sensor.setInterruptPin(APDS9960_INT);
    sensor.begin();
    Serial.begin(115200);
    if (!sensor.begin()) {
        Serial.println("Failed to initialize APDS9960!");
        while (1) {
            delay(100);
        }
    }
    Serial.println("APDS9960 initialized successfully!");
}

int r, g, b, c;
void RGBloop() {
    while(!sensor.colorAvailable()) {
        delay(5);
    }

    sensor.readColor(r, g, b, c);

    Serial.print(" r = "); Serial.print(r);
    Serial.print(" g = "); Serial.print(g);
    Serial.print(" b = "); Serial.print(b);
    Serial.print(" c = "); Serial.println(c);

    vTaskDelay(1);
}

QTRSensors qtr;
uint16_t sensors[2];

void setup() {

    Serial.begin(115200);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys(); 
    esp_log_level_set("gpio", ESP_LOG_ERROR); // Suppress info log spam from gpio_isr_service
    uni_bt_allowlist_set_enabled(true);
    myServo.attach(26);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    RGBsetup();
    
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
    sensor.setInterruptPin(APDS9960_INT);
    sensor.begin();
    Serial.begin(115200);

    Serial.begin(115200);
    IRSensorRight.setFilterRate(1.0f);
    IRSensorFront.setFilterRate(1.0f);
    IRSensorLeft.setFilterRate(1.0f);

    // set up Serial Communication and sensor pins
    Serial.begin(115200);
    qtr.setTypeAnalog(); // or setTypeAnalog()
    qtr.setSensorPins((const uint8_t[]) {26, 27}, 2); // pin numbers go in the curly brackets {}, and number of sensors in use goes after
    // calibration sequence
    for (uint8_t i = 0; i < 250; i++) { 
        Console.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
        qtr.calibrate(); 
        delay(20);
    }
}


void loop() {

    vTaskDelay(1); // Ensures WDT does not get triggered when no controller is connected

    qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
    Console.printf("S1: %d S2: %d\n", sensors[0], sensors[1]);
    delay(250); // line sensor stuff

    myServo.write(1750); // Rotate to 0 degrees
    RGBloop();

    Console.printf("Right sensor: %f \n",IRSensorRight.getDistanceFloat());
    Console.printf("Front sensor: %f \n",IRSensorFront.getDistanceFloat());
    Console.printf("Left sensor: %f \n",IRSensorLeft.getDistanceFloat());

    
    



    BP32.update(); 
    for (auto myController : myControllers) { // Only execute code when controller is connected
        if (myController && myController->isConnected() && myController->hasData()) {
            
            if (myController->x())  {
                while(myController->b() == 0){
                    if (IRSensorFront.getDistanceFloat() > 40 && IRSensorLeft.getDistanceFloat() > 40 && IRSensorRight.getDistanceFloat() > 40) 
                    GoForward(255);

                    else if (IRSensorLeft.getDistanceFloat() <40)
                    GoRight(255);

                    else if (IRSensorRight.getDistanceFloat() <40)
                    GoLeft(255); 

                    BP32.update(); }
    }


            if (myController->y())  {
                while(myController->b() == 0) {

                    if (r > 40){
                        GoForward(255);
                        
                        delay(250);
                    
                    do {
                        GoForward(255);
                    } while (r <= 40);
                    Stop();
                }

                    else if (g > 40){
                        GoForward(255);
                        
                        delay(250);
                    
                    do {
                        GoForward(255);
                    } while (g <= 40);
                    Stop();
                }

                    else if (b > 40){
                        GoForward(255);
                        
                        delay(250);
                    
                    do {
                        GoForward(255);
                    } while (b <= 40);
                    Stop();
                }
       BP32.update(); }   

    }
        
          
            if(myController->axisY() < -200){           // go forward

                GoForward(255);

                //delay(250); // Run for .25 second
                
            } 
            else if(myController->axisY() > 200){       // go backward
               
                GoBackward(255);

                //delay(250); // Run for .25 second
                
        
            }  
            else if(myController->axisX() < -200){      // turn left
                
                GoLeft(255);

                //delay(250); // Run for .25 second
                
                
            }
            else if(myController->axisX() > 200){       // turn right
                
                GoRight(255);

               // delay(250); // Run for .25 second
                
            }
            
            else{
            
                Stop();
            
                //delay(250);
            
            }
            dumpGamepad(myController); // Prints the gamepad state, delete or comment if don't need
        }
    }
}
 