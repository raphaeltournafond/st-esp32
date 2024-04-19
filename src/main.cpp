#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>

#define MMA8451_DEFAULT_ADDRESS (0x1D) /**< MMA8451 default I2C address */

Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial monitor
  }
  
  Wire.begin();

  if (!mma.begin()) {
    Serial.println("Could not find a valid MMA8451 sensor, check wiring!");
    while (1);
  }

  mma.setRange(MMA8451_RANGE_4_G);
}

void loop() {
  sensors_event_t event;
  mma.getEvent(&event);

  // Print the accelerometer data
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
  Serial.print("Z: "); Serial.println(event.acceleration.z);

  delay(500); // Delay between readings
}
