/*
 * pins.h
 *
 * Created: 23.04.2016 19:41:15
 *  Author: JochenAlt
 */


#ifndef PINS_H_
#define PINS_H_

#include <Arduino.h>
#include "i2c_t3.h"

// global variables used sensor bus (all is implemented in main.cpp)
extern i2c_t3* Wires[2];

// global variables used for interfacing and for logging
extern HardwareSerial* cmdSerial;
extern HardwareSerial* logger;
extern HardwareSerial* servoComm;

// first I2C bus is for 4 rotary encoder, second for one encoder and lights
#define I2C0 0
#define I2C1 1
#define PIN_SDA0 18
#define PIN_SCL0 19
#define PIN_SDA1 38
#define PIN_SCL1 37

// UART 5, used as serial interface
#define PIN_RX5 34
#define PIN_TX5 33
#define SERIAL_CMD_RX PIN_RX5
#define SERIAL_CMD_TX PIN_TX5

// UART 4, used for logging
#define PIN_RX4 31
#define PIN_TX4 32
#define SERIAL_LOG_RX PIN_RX4
#define SERIAL_LOG_TX PIN_TX2

// UART 1, used for Herkulex servos
#define PIN_RX1 0
#define PIN_TX1 1
#define HERKULEX_RX PIN_RX1
#define HERKULEX_TX PIN_TX1

// UART 6, used for thermal printer
#define PIN_RX6 47
#define PIN_TX6 48
#define THERMAL_PRINTER_RX PIN_RX6
#define THERMAL_PRINTER_TX PIN_TX6

// Sensor Bus 0 on I2C0
#define SENSOR0_SDA PIN_SDA0
#define SENSOR0_SCL PIN_SCL0

// Sensor Bus 1 on I2C1
#define SENSOR1_SDA PIN_SDA1
#define SENSOR1_SCL PIN_SCL1

// Stepper motor driver, all have one pin for enable/direction/clock
#define HIP_EN_PIN 			4
#define HIP_DIR_PIN 		3
#define HIP_CLK_PIN 		2

#define UPPERARM_EN_PIN 	7
#define UPPERARM_DIR_PIN 	6
#define UPPERARM_CLK_PIN 	5

#define FOREARM_EN_PIN 		11
#define FOREARM_DIR_PIN 	10
#define FOREARM_CLK_PIN 	9

#define ELBOW_EN_PIN 		25
#define ELBOW_DIR_PIN		24
#define ELBOW_CLK_PIN		12

#define WRIST_EN_PIN		28
#define WRIST_DIR_PIN		27
#define WRIST_CLK_PIN		26

#define MOTOR_KNOB_PIN		15			// potentiometer for manual actuator control
#define LED_PIN 			13			// blinking LED on Teensy

// connection to power supply pcb, where two relay switch on power for servos (9V/1A) and for the steppers (24V/10A)
#define POWER_SUPPLY_STEPPER_PIN 23		// line connected to a relay powering on steppers
#define POWER_SUPPLY_SERVO_PIN 22		// line connected to a relay powering on servos

#endif