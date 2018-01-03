
///////////////////////////////////////////////////////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <EEPROM.h>


#define STEPPIN1  D1
#define DIRPIN1   D0

#define SENSORPIN1 D2
#define SENSORPIN2 D3

#define IDPIN0 D5
#define IDPIN1 D6
#define IDPIN2 D7

#define IDNR 0

WiFiClient espClient;
PubSubClient client(espClient);
AccelStepper axis1(1, STEPPIN1, DIRPIN1);

// wifi
const char* ssid = "ssidssid";
const char* password = "passwordpassword";
const char* mqtt_server = "192.168.2.100"; // raspberry

// mqtt
byte id;
char msg[50];

String prefix = "/";
String posString;
String statusString;

String resetString;
String speedString;
String targetString;
String startString;

unsigned long lastMsg;
unsigned int posUpdateFreq = 500;

// eeprom
int eepromAdress = 0;

// motor
unsigned int normalAccel = 4000;
unsigned long maxDist;
int motorSpeed = 500;

int statusByte = 0;
boolean calibrated = false;

boolean doPublishPos = false;

///////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(SENSORPIN1, INPUT_PULLUP);
  pinMode(SENSORPIN2, INPUT_PULLUP);

  pinMode(IDPIN0, INPUT);
  pinMode(IDPIN1, INPUT);
  pinMode(IDPIN2, INPUT);

  Serial.begin(19200);
  delay(10);
  Serial.println("test");
  delay(10);

  id = IDNR;
  //id = getID();

  if (id < 100) {
    prefix += "0";
  }
  if (id < 10) {
    prefix += "0";
  }
  prefix += String(id);
  prefix += "/";


  Serial.println("ID: " + String(id));

  setup_wifi();
  Serial.println("CONNECTED");
  axis1.setMaxSpeed(motorSpeed);
  axis1.setSpeed(motorSpeed);
  axis1.setAcceleration(1000);

  calibrate();

  //maxDist = EEPROMReadlong(eepromAdress);

  //runToZero();
  lastMsg = millis();

}

///////////////////////////////////////////////////////////////////////////////////////////////

void loop() {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  postPos();

  switch (statusByte) {
    case 0: {

        if (digitalRead(SENSORPIN1)) {
          axis1.runSpeed();
        }
        else {
          axis1.setCurrentPosition(0);
          statusByte++;
          sendStatus();
          axis1.setSpeed(motorSpeed);
        }
        break;
      }
    case 1: {
        if (digitalRead(SENSORPIN2)) {
          axis1.runSpeed();
        }
        else {
          maxDist = axis1.currentPosition();
          axis1.moveTo(maxDist);
          statusByte++;
          sendStatus();
          calibrated = true;
        }
        break;
      }
    case 2: {
        axis1.run();
        break;
      }
  }
  if (doPublishPos) postPos();

   Serial.print("D1: ");
  Serial.println(digitalRead(SENSORPIN1));
  Serial.print("D2: ");
  Serial.println(digitalRead(SENSORPIN2));

}

///////////////////////////////////////////////////////////////////////////////////////////////

void postPos() {
  if (millis() > lastMsg + posUpdateFreq) {
    sendPosition();
    //Serial.println(String(statusByte));
    lastMsg = millis();
  }
}

