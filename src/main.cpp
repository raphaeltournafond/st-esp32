#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define MMA8451_DEFAULT_ADDRESS (0x1D) // MMA8451 default I2C address

#define SERVICE_UUID        "dc23fd97-88af-47ff-98a6-4fc529b66d7b"
#define CHARACTERISTIC_UUID "6a4496fd-51b3-419e-ac53-956fdbf0658a"

Adafruit_MMA8451 mma = Adafruit_MMA8451();
BLECharacteristic *pCharacteristic;
BLEServer *pServer;
bool isDeviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device connected");
      isDeviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Device disconnected");
      isDeviceConnected = false;
      BLEDevice::startAdvertising(); // Restart advertising
    }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial monitor
  }

  // Bluetooth Setup

  Serial.println("Starting BLE setup!");

  BLEDevice::init("Smart Tracker BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->setValue("Hello World!");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE setup done");

  // I2C Accelerometer setup

  Serial.println("Starting accelerometer connection");
  
  Wire.begin();

  if (!mma.begin()) {
    Serial.println("Could not find a valid MMA8451 sensor, check wiring!");
    while (1);
  }

  mma.setRange(MMA8451_RANGE_4_G);

  Serial.println("Accelerometer connection established");
}

void loop() {

  if (isDeviceConnected) {
    sensors_event_t event;
    mma.getEvent(&event);

    char buffer[30]; // Buffer to hold the string representation
    sprintf(buffer, "%.3f,%.3f,%.3f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    pCharacteristic->setValue(buffer);
    pCharacteristic->notify();

    Serial.printf("X: %.3f Y: %.3f Z: %.3f\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
  }

  delay(100);
}
