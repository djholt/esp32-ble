#include <BLEDevice.h>
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define DEVICE_NAME         "DJ BLE"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst

BLECharacteristic *pCharacteristic;
String message = "";

void printToScreen(String s) {
  display.clear();
  display.drawString(0, 0, s);
  display.display();
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
  pinMode(0, INPUT_PULLUP);
  Serial.begin(115200);

  display.init();

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
    delay(50);
    Serial.println("Button press detected!");
    message.toUpperCase();
    pCharacteristic->setValue(message.c_str());
    printToScreen("Got em.");
  }
}