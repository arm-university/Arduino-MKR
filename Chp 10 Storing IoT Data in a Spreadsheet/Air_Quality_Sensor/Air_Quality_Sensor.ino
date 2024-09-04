#include "Air_Quality_Sensor.h"

AirQualitySensor sensor(A0);

void setup() {
  Serial.begin(9600); //Initiliase the serial monitor
  delay(10000); //Wait for the serial monitor to open
  Serial.println("Waiting for the sensor to warm up. Just 20 seconds...");
  delay(20000); //Wait for the sensor to warm up
  if (sensor.init()) { //Try and initialise the sensor.
    Serial.println("Sensor ready.");
  } else {
    Serial.println("Sensor did not initialise, sorry!");
  }
}

void loop() {
  int pollutionValue = sensor.slope(); //Update variation
  int quality = sensor.getValue(); //Get the sensor reading
  Serial.print("Sensor value: ");
  Serial.println(quality); //Output the reading
  if (pollutionValue == AirQualitySensor::FORCE_SIGNAL) {
    Serial.println("Force signal - get out now!");
  } else if (pollutionValue == AirQualitySensor::HIGH_POLLUTION) {
    Serial.println("High pollution - this could be bad!");
  } else if (pollutionValue == AirQualitySensor::LOW_POLLUTION) {
    Serial.println("Low pollution - this could be worse but it's not great.");
  } else if (pollutionValue == AirQualitySensor::FRESH_AIR) {
    Serial.println("Fresh air - hurrah!");
  }
  delay(10000); //10s delay
}
