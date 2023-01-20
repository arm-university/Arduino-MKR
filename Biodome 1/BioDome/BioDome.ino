#include <Arduino_MKRENV.h>
#include <Time.h>

int light1 = 1;
int light2 = 2;
int light3 = 3;
int light4 = 5;
int fanA = 8;
int fanB = 9;

const float maxTemp = 25.0;
const float minIllum = 80;


void setup() {
  pinMode(light1,OUTPUT);
  pinMode(light2,OUTPUT);
  pinMode(light3,OUTPUT);
  pinMode(light4,OUTPUT);
  pinMode(fanA,OUTPUT);
  pinMode(fanB,OUTPUT);
  setTime(12,0,0,1,1,2000);
  Serial.begin(9600);
  while (!Serial);

  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
}

void turnOnTheLights() {
  digitalWrite(light1,HIGH);
  digitalWrite(light2,HIGH);
  digitalWrite(light3,HIGH);
  digitalWrite(light4,HIGH);
}

void turnOffTheLights() {
  digitalWrite(light1,LOW);
  digitalWrite(light2,LOW);
  digitalWrite(light3,LOW);
  digitalWrite(light4,LOW);
}

void turnOnFan() {
  digitalWrite(fanA,HIGH);
  digitalWrite(fanB,LOW);
}

void turnOffFan() {
  digitalWrite(fanA,LOW);
  digitalWrite(fanB,LOW);
}

void loop() {
  // read all the sensor values
  float temperature = ENV.readTemperature();
  float humidity    = ENV.readHumidity();
  float pressure    = ENV.readPressure();
  float illuminance = ENV.readIlluminance();
  float uva         = ENV.readUVA();
  float uvb         = ENV.readUVB();
  float uvIndex     = ENV.readUVIndex();
  
  //Turn the lights on or off
  time_t t = now();
  Serial.println(hour(t));
  Serial.println(minute(t));
  if (hour(t)>=22 or hour(t)<7 or illuminance < minIllum) {
    turnOnTheLights();
  } else {
    turnOffTheLights();
  }
  if (minute(t)<0 or temperature > maxTemp) {
    turnOnFan();
  } else {
    turnOffFan();
  }


  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Pressure    = ");
  Serial.print(pressure);
  Serial.println(" kPa");

  Serial.print("Illuminance = ");
  Serial.print(illuminance);
  Serial.println(" lx");

  Serial.print("UVA         = ");
  Serial.println(uva);

  Serial.print("UVB         = ");
  Serial.println(uvb);

  Serial.print("UV Index    = ");
  Serial.println(uvIndex);

  // print an empty line
  Serial.println();

  // wait 1 second to print again
  delay(1000);
}
