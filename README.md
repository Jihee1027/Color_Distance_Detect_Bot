# Color and Distance Sensor Robot

## Overview
The Color and Distance Sensor Robot is an autonomous robot that scans a 180-degree area to detect a target color, turns toward that target, moves in its direction, and stops once it reaches a set distance. The system combines multiple embedded interfaces and peripherals, including I2C, SPI, PWM, and ADC, to control sensors, motors, and display output.

## Project Objectives
- Use **I2C** to receive data from the color/light sensor and distance sensor
- Use **SPI** to display detected color and distance information on the TFT LCD
- Use **PWM** to control motor speed and turning behavior
- Use **ADC** to monitor light intensity and battery voltage
- Implement a reset button to restart the search behavior

## Features
- Scans the surrounding **180-degree field**
- Detects a specified **target color**
- Rotates toward the detected target
- Drives toward the object
- Stops when the robot is approximately **2 inches away**
- Displays:
  - detected color
  - measured distance
  - battery voltage
- Resets and restarts search when the button is pressed

## System Description
The robot uses a servo-mounted color/light sensor to scan different angles in front of it. During the scan, it checks for the desired color intensity and determines the best direction to move. Once the target direction is found, the robot uses PWM-controlled motors to rotate and drive toward the object. An ultrasonic distance sensor continuously measures the distance to the target, and the robot stops when it reaches the required stopping point. After stopping, the LCD/TFT displays the detected color, final distance, and battery level.

## Hardware Components
- Adafruit APDS9999 Proximity, Lux, Light, and Color Sensor
- Ultrasonic Distance Sensor (HC-SR04 compatible)
- TowerPro SG92R Micro Servo
- L9110H H-Bridge Motor Driver
- Mecanum Wheels
- Lithium Ion Battery Pack
- TFT LCD display
- Push button for reset

## Interfaces Used
- **I2C**: sensor communication
- **SPI**: LCD/TFT display communication
- **PWM**: motor speed control and steering
- **ADC**: light intensity and battery voltage readings
- **GPIO**: reset button and control signals

## Expected Operation
1. The robot starts in scanning mode.
2. The servo rotates the sensor across 180 degrees.
3. The robot analyzes color data at each angle.
4. Once the target color is found, the robot turns toward it.
5. The robot drives forward while checking distance.
6. When it is about 2 inches away, it stops.
7. The display shows the detected color, distance, and battery voltage.
8. Pressing the reset button clears the state and begins scanning again.

## Team Members
- havlinc
- zamerman
- kim4465
- bpatke