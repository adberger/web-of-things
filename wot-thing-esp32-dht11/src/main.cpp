#include <Arduino.h> // needed for feather m0

#include <App.hpp>
#include <LoRaWAN.hpp>
#include <DHT11Sensor.hpp>
#include <EventLog.hpp>

void setup() {
  Serial.begin(115200);

  myEventLog.setup();
  myLoRaWAN.setup(&myPinMap);
  mySensor.setup(); // TODO remove parameter later, only for testing
}

void loop() {
  myLoRaWAN.loop();
  mySensor.loop();
  myEventLog.loop();
}