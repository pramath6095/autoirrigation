#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverMacAddress[] = { 0xD0, 0xEF, 0x76, 0x57, 0xF4, 0x3C };

typedef struct struct_message {
  char command[32]; 
  int value;
} struct_message;

struct_message outgoingData;
struct_message incomingData;

void sendData() {
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
  if (result == ESP_OK) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.println("Error sending data.");
  }
}

void onDataRecv(const esp_now_recv_info* recv_info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  Serial.print("Received Confirmation: ");
  Serial.println(incomingData.command);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Transmitter setup complete, ready to send commands.");
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    
    if (input == "on") {
      strncpy(outgoingData.command, "on", sizeof(outgoingData.command) - 1);
      outgoingData.command[sizeof(outgoingData.command) - 1] = '\0';
      sendData();
    } else if (input == "off") {
      strncpy(outgoingData.command, "off", sizeof(outgoingData.command) - 1);
      outgoingData.command[sizeof(outgoingData.command) - 1] = '\0';
      sendData();
    } else if (input == "calibrate") {
      strncpy(outgoingData.command, "calibrate", sizeof(outgoingData.command) - 1);
      outgoingData.command[sizeof(outgoingData.command) - 1] = '\0';
      sendData();
    } else if (input.startsWith("threshold")) {
      int thresholdValue = input.substring(10).toInt();
      strncpy(outgoingData.command, "threshold", sizeof(outgoingData.command) - 1);
      outgoingData.command[sizeof(outgoingData.command) - 1] = '\0';
      outgoingData.value = thresholdValue;
      sendData();
    }
  }
}