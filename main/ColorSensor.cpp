// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "sdkconfig.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"

// ===========================
// Bluepad32 Controller Setup
// ===========================

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; 
// BP32 allows up to 4 concurrent controller connections

void dumpGamepad(ControllerPtr ctl) {
    Console.printf(
        "DPAD: %d A: %d B: %d X: %d Y: %d LX: %d LY: %d RX: %d RY: %d L1: %d R1: %d L2: %d R2: %d\n",
        ctl->dpad(),
        ctl->a(), ctl->b(), ctl->x(), ctl->y(),
        ctl->axisX(), ctl->axisY(),
        ctl->axisRX(), ctl->axisRY(),
        ctl->l1(), ctl->r1(),
        ctl->l2(), ctl->r2()
    );
}

// ===========================
// Color Sensor (APDS9960)
// ===========================

// I2C pins & config
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

Adafruit_APDS9960 sensor;

void RGBsetup() {
    Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
    Serial.begin(115200);

    if (!sensor.begin()) {
        Serial.println("Failed to initialize APDS9960!");
        while (1) {
            delay(100);
        }
    }
    Serial.println("APDS9960 initialized successfully!");
}

void RGBloop() {
    uint16_t r, g, b, c;
    sensor.getColorData(&r, &g, &b, &c);

    Serial.print("r = "); Serial.print(r);
    Serial.print(" g = "); Serial.print(g);
    Serial.print(" b = "); Serial.print(b);
    Serial.print(" c = "); Serial.println(c);

    vTaskDelay(1);
}

// ===========================
// Main setup() and loop()
// ===========================

void setup() {
    // --- Gamepad setup ---
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    esp_log_level_set("gpio", ESP_LOG_ERROR);  // suppress GPIO ISR spam
    uni_bt_allowlist_set_enabled(true);

    // --- Color sensor setup ---
    RGBsetup();
}

void loop() {
    vTaskDelay(1);  // Prevent watchdog triggers when idle
    BP32.update();

    // --- Color sensor reading ---
    RGBloop();   // comment out if not needed continuously

    // --- Gamepad handling ---
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            dumpGamepad(myController);
        }
    }
}
