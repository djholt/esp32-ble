#include <BLEDevice.h>
#include "heltec.h"

#define DEVICE_NAME         "DJ BLE"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
String message = "";

void printToScreen(String s) {
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, s);
  Heltec.display->display();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      printToScreen("BLE client connected.");
    };

    void onDisconnect(BLEServer* pServer) {
      printToScreen("BLE client disconnected.");
      BLEDevice::startAdvertising();
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    message = String(characteristic->getValue().c_str());
    printToScreen("Received:\n" + message);

    if (message == "ledoff") {
      digitalWrite(25, LOW);
    }
    if (message == "ledon") {
      digitalWrite(25, HIGH);
    }
  }
};

void setup() {
  Heltec.begin(true /*display*/, false /*LoRa*/, true /*Serial*/);
  printToScreen("Starting BLE!");

  BLEDevice::init(DEVICE_NAME);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pCharacteristic->setValue("Init");

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
}

void loop() {
  if (digitalRead(0) == LOW) {
    while (digitalRead(0) == LOW);
    message.toUpperCase();
    pCharacteristic->setValue(message.c_str());
    printToScreen("Got em.");
    delay(250);
  }
}
