#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//==================== WiFi ====================

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

ESP8266WebServer server(80);

//================== Motor Pins =================

// Left Motor
#define ENA D1  // GPIO5
#define IN1 D8  // GPIO15
#define IN2 D7  // GPIO13

// Right Motor
#define ENB D2  // GPIO4
#define IN3 D6  // GPIO12
#define IN4 D5  // GPIO14

// Horn
#define HORN D3  // GPIO16

//==============================================

// Values received from MIT App Inventor
// Speed     : -100 to +100
// Direction : -100 to +100

int speedValue = 0;
int directionValue = 0;

// unsigned long hornStartTime = 0;
// bool hornActive = false;

bool hornActive = false;
unsigned long hornTimer = 0;
int hornStep = 0;

// Built-in LED status
#define STATUS_LED LED_BUILTIN

const unsigned long CONTROLLER_TIMEOUT = 3000; // 3 seconds
unsigned long lastControllerActivity = 0;
bool controllerRecentlyActive = false;

//=====================================================
// LEFT MOTOR
//=====================================================

void setLeftMotor(int speed) {
  speed = constrain(speed, -1023, 1023);

  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    speed = -speed;
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  analogWrite(ENA, speed);
}

//=====================================================
// RIGHT MOTOR
//=====================================================

void setRightMotor(int speed) {
  speed = constrain(speed, -1023, 1023);

  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    speed = -speed;
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  analogWrite(ENB, speed);
}



void driveRobot() {
  // Speed: -100 to +100
  // Direction: -100 to +100

  int leftSpeed;
  int rightSpeed;

  // Differential steering
  leftSpeed = speedValue + directionValue;
  rightSpeed = speedValue - directionValue;

  // Limit values
  leftSpeed = constrain(leftSpeed, -100, 100);
  rightSpeed = constrain(rightSpeed, -100, 100);

  // Convert -100..100 to PWM -1023..1023
  leftSpeed = map(abs(leftSpeed), 0, 100, 0, 1023) * (leftSpeed >= 0 ? 1 : -1);

  rightSpeed = map(abs(rightSpeed), 0, 100, 0, 1023) * (rightSpeed >= 0 ? 1 : -1);

  // Dead zone
  if (abs(leftSpeed) < 40)
    leftSpeed = 0;

  if (abs(rightSpeed) < 40)
    rightSpeed = 0;

  setLeftMotor(leftSpeed);
  setRightMotor(rightSpeed);
}

void startHorn() {
  hornActive = true;
  hornStep = 0;
  hornTimer = millis();

  digitalWrite(HORN, HIGH);
}

void stopHorn() {
  hornActive = false;
  digitalWrite(HORN, LOW);
}

void updateHorn() {

  if (!hornActive)
    return;

  unsigned long now = millis();

  switch (hornStep) {

    case 0:
      // BEEP 1
      digitalWrite(HORN, HIGH);

      if (now - hornTimer >= 300) {
        hornStep = 1;
        hornTimer = now;
        digitalWrite(HORN, LOW);
      }
      break;

    case 1:
      // PAUSE
      if (now - hornTimer >= 250) {
        hornStep = 2;
        hornTimer = now;
        digitalWrite(HORN, HIGH);
      }
      break;

    case 2:
      // BEEP 2
      if (now - hornTimer >= 150) {
        hornStep = 3;
        hornTimer = now;
        digitalWrite(HORN, LOW);
      }
      break;

    case 3:
      // PAUSE
      if (now - hornTimer >= 200) {
        hornStep = 4;
        hornTimer = now;
        digitalWrite(HORN, HIGH);
      }
      break;

    case 4:
      // BEEP 3
      if (now - hornTimer >= 400) {
        hornStep = 5;
        hornTimer = now;
        digitalWrite(HORN, LOW);
      }
      break;

    case 5:
      // LONG PAUSE
      if (now - hornTimer >= 500) {
        hornStep = 0;
        hornTimer = now;
        digitalWrite(HORN, HIGH);
      }
      break;
  }
}


//==================================================
// HOME PAGE
//==================================================

void handleRoot() {
  server.send(200, "text/plain", "ESP8266 Smart Car Controller");
}

//==================================================
// RECEIVE SLIDER VALUES
//==================================================

void handleControl() {

    // Record controller activity
  lastControllerActivity = millis();
  controllerRecentlyActive = true;

  if (server.hasArg("command")) {
    String command = server.arg("command");

    Serial.print("Received: ");
    Serial.println(command);


    // Updated horn command
    if (command == "H:1") {
      // digitalWrite(HORN, HIGH);
      if (!hornActive) {
        startHorn();
      }
    }

    if (command == "H:0") {
      // digitalWrite(HORN, LOW);
      stopHorn();
    }


    int sIndex = command.indexOf("S:");
    int dIndex = command.indexOf(",D:");

    if (sIndex >= 0 && dIndex >= 0) {
      // Get speed
      String speedString =
        command.substring(sIndex + 2, dIndex);

      // Get direction
      String directionString =
        command.substring(dIndex + 3);

      speedValue = speedString.toInt();
      directionValue = directionString.toInt();

      // Safety limits
      speedValue = constrain(speedValue, -100, 100);
      directionValue = constrain(directionValue, -100, 100);

      // Drive motors
      driveRobot();

      // Serial Monitor
      Serial.print("Speed: ");
      Serial.print(speedValue);

      Serial.print("   Direction: ");
      Serial.println(directionValue);
    }
  }

  server.send(200, "text/plain", "OK");
}

//==================================================
// SETUP
//==================================================

void setup() {
  Serial.begin(115200);

    // Initialize built-in LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH); // LED OFF initially

  // Motor Pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  // Stop Motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Horn
  pinMode(HORN, OUTPUT);
  digitalWrite(HORN, LOW);

  analogWriteRange(1023);
  analogWriteFreq(1000);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  Serial.println();
  Serial.println("==============================");
  Serial.println("Connecting to WiFi...");
  Serial.println("==============================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("==============================");
  Serial.println("WiFi Connected");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println("==============================");

  // Web Server
  server.on("/", handleRoot);
  server.on("/control", handleControl);

  server.begin();

  Serial.println("HTTP Server Started");
}

//==================================================
// LOOP
//==================================================

void loop() {
  server.handleClient();

  // Check Wi-Fi connection
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);

  // Check recent controller activity
  bool controllerActive =
      controllerRecentlyActive &&
      (millis() - lastControllerActivity <= CONTROLLER_TIMEOUT);

  // Control built-in LED
  if (wifiConnected && controllerActive) {
    digitalWrite(STATUS_LED, LOW);  // LED ON
  } else {
    digitalWrite(STATUS_LED, HIGH); // LED OFF
    controllerRecentlyActive = false;
  }

  updateHorn();

  // if (hornActive && millis() - hornStartTime >= 500) {
  //   digitalWrite(HORN, LOW);
  //   hornActive = false;
  // }
}
