#include <esp_now.h>
#include <WiFi.h>

uint8_t transmitterMacAddress[] = { 0xA0, 0xB7, 0x65, 0x27, 0x0A, 0x34 };  
typedef struct struct_message {
  char command[32];  
  int value;
} struct_message;

struct_message incomingData;
struct_message outgoingData;

int sensorPin = 34;
int en = 13;
int in1 = 12;
int in2 = 14;
int dryThreshold = 2500;
int wetThreshold = 1500;
int activereadfreq = 200;
int passivereadfreq = 5000;
bool motorRunning = false;
bool systemActive = false;

void sendData(const char *message) {
  strncpy(outgoingData.command, message, sizeof(outgoingData.command) - 1);
  outgoingData.command[sizeof(outgoingData.command) - 1] = '\0';

  esp_err_t result = esp_now_send(transmitterMacAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
  if (result == ESP_OK) {
    Serial.println("Data sent successfully to transmitter!");
  } else {
    Serial.println("Error sending data.");
  }
}

void startMotor() {
  digitalWrite(en, HIGH);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  motorRunning = true;
  sendData("Motor started");
}

void stopMotor() {
  digitalWrite(en, LOW);
  motorRunning = false;
  sendData("Motor stopped");
}

void monitorMoisture() {
  if (systemActive) {
    int moisture = analogRead(sensorPin);
    sendData(("Current moisture level: " + String(moisture)).c_str());

    if (moisture > dryThreshold && !motorRunning) {
      startMotor();
    }
    if (moisture < wetThreshold && motorRunning) {
      stopMotor();
    }

    delay(motorRunning ? activereadfreq : passivereadfreq);
  }
}

void onDataRecv(const esp_now_recv_info* recv_info, const uint8_t* data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  Serial.print("Received Command: ");
  Serial.println(incomingData.command);

  if (strcmp(incomingData.command, "on") == 0) {
    systemActive = true;
    startMotor();
  } else if (strcmp(incomingData.command, "off") == 0) {
    systemActive = false;
    stopMotor();
  } else if (strcmp(incomingData.command, "calibrate") == 0) {
    sendData("Calibration mode activated.");
  } else if (strcmp(incomingData.command, "threshold") == 0) {
    sendData("Threshold updated.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(en, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, transmitterMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Receiver setup complete, waiting for commands.");
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  monitorMoisture();
  delay(100);
}