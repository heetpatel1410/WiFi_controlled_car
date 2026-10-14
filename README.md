# WiFi_controlled_car
# ESP8266 Wi-Fi Controlled Car

A smartphone-controlled robotic car developed using the **ESP8266 NodeMCU, DC motors, a motor driver, and MIT App Inventor**. The system uses Wi-Fi communication and an Android application to control the car's movement, speed, steering, and horn.

## Project Overview

This project demonstrates the integration of embedded systems, wireless communication, motor control, and mobile application development. The ESP8266 receives HTTP commands from an Android application and controls two DC motors through a dual-channel motor driver.

The mobile application, **Smart Car Controller**, is designed using MIT App Inventor and provides a simple interface for remotely operating the vehicle.

## Features

- Wi-Fi-based wireless car control
- Forward and reverse movement
- Left and right steering
- Adjustable motor speed using PWM
- Smartphone-based stop and horn controls
- Differential steering for turning
- HTTP communication between the mobile app and ESP8266

## Hardware Requirements

| Component | Purpose |
|---|---|
| ESP8266 NodeMCU | Main controller and Wi-Fi communication |
| Two DC geared motors | Drive the vehicle |
| Dual-channel motor driver | Controls motor direction and drive current |
| Robot chassis and wheels | Mechanical structure |
| Battery / motor power supply | Powers the motors |
| Buzzer | Horn functionality |
| Jumper wires and connectors | Electrical connections |
| Android smartphone | Wireless control interface |

## Software Requirements

- Arduino IDE
- ESP8266 Arduino Core
- `ESP8266WiFi` library
- `ESP8266WebServer` library
- MIT App Inventor
- Android smartphone

## System Architecture

```text
Android Smartphone
        |
        v
Smart Car Controller App
     (MIT App Inventor)
        |
        | HTTP over Wi-Fi
        v
 ESP8266 NodeMCU
        |
        | GPIO + PWM
        v
 Dual-Channel Motor Driver
       /           \
      v             v
 Left DC Motor   Right DC Motor

 ESP8266 GPIO ---> Buzzer / Horn
```

## Pin Configuration

| Function | NodeMCU Pin | ESP8266 GPIO |
|---|---|---|
| Left motor enable (ENA) | D1 | GPIO5 |
| Left motor input 1 (IN1) | D8 | GPIO15 |
| Left motor input 2 (IN2) | D7 | GPIO13 |
| Right motor enable (ENB) | D2 | GPIO4 |
| Right motor input 3 (IN3) | D6 | GPIO12 |
| Right motor input 4 (IN4) | D5 | GPIO14 |
| Buzzer control | D0 | GPIO16 |

**Hardware note:** GPIO15 (D8) is an ESP8266 boot-strapping pin. Verify that the motor-driver circuit does not interfere with the board's startup requirements. Connect the controller and motor-driver grounds together, and never power motors directly from GPIO pins.

## Working Principle

1. The ESP8266 connects to the configured Wi-Fi network.
2. The Android application sends control commands through HTTP requests.
3. The ESP8266 web server receives and processes the commands.
4. The firmware calculates the required motor direction and speed.
5. The motor driver controls the two DC motors using direction signals and PWM.
6. The Stop and Horn controls operate their respective outputs.

### Differential Steering

The controller combines speed and steering values to determine the individual motor commands:

```cpp
leftSpeed  = speed + direction;
rightSpeed = speed - direction;
```

Both values are limited to the supported range of -100 to +100.

- Positive speed: forward movement
- Negative speed: reverse movement
- Zero speed: stop
- Different left and right motor commands: turning

PWM controls the effective motor drive level. The actual turning direction depends on motor wiring and the firmware's direction convention.

## Getting Started

1. Install the Arduino IDE and ESP8266 board support package.
2. Open the ESP8266 firmware file in the `firmware/` directory.
3. Configure the Wi-Fi credentials in the code.
4. Select the correct ESP8266 board and serial port.
5. Upload the firmware to the NodeMCU.
6. Open Serial
