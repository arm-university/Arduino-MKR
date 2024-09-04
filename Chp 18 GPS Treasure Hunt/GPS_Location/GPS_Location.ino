#include <Arduino_MKRGPS.h>
#include <LiquidCrystal.h>

//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int rs = 10, en = 9, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  //Initialise serial monitor
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Trying serial");
  // initialise serial communications and wait for port to open:
  Serial.begin(9600);
  delay(10000); //Wait 10s for serial monitor if connected
  lcd.setCursor(0,0);
  if (!Serial) {
    lcd.print("No serial.   ");
  } else {
    lcd.print("Serial ready ");
  }

  lcd.setCursor(0,1);
  lcd.print("Trying GPS");
  Serial.println("Trying GPS");
  delay(3000);
  lcd.setCursor(0,1);
  if (GPS.begin()) {
    lcd.print("GPS ready.");
    Serial.println("GPS ready");
  } else {
    lcd.print("GPS failed.");
    Serial.println("GPS failed");
  }
}

void loop() {
  if (GPS.available()) {
    lcd.clear();
    // read GPS values
    float latitude   = GPS.latitude();
    float longitude  = GPS.longitude();
    float altitude   = GPS.altitude();
    int   satellites = GPS.satellites();
    Serial.print("LAT: ");
    Serial.println(latitude, 7);
    lcd.setCursor(0,0);
    lcd.print("LAT:");
    lcd.print(latitude, 7);
    Serial.print("LON: ");
    Serial.println(longitude, 7);
    lcd.setCursor(0,1);
    lcd.print("LON:");
    lcd.print(longitude, 7);
    Serial.print("ALT: ");
    Serial.print(altitude);
    Serial.println("m");
    Serial.print("SAT: ");
    Serial.println(satellites);
  }
}
