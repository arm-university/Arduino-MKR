#include <Arduino_MKRENV.h> //Library for the env module
#include <Time.h> //Allows use of time object for timers
#include <Servo.h> //Allows access to the servo flag
#include <ArduinoHttpClient.h> //Handles connection to Excel file
//#include <WiFi101.h> //MKR1000
#include <WiFiNINA.h> #wifi1010 //MKR1010

const char WEBSITE[] = "script.google.com";
const int SSLPORT = 443;
const char* MY_SSID = "YOUR SSID";
const char* MY_PWD =  "YOUR PASSWORD";
const char DEPLOYMENTID[] = "YOUR DEPLOYMENT ID FROM GOOGLE SCRIPTS";
const float maxTemp = 25.0; //Max temp before fan
const float minIllum = 80; //Min illumination before lights

int status = WL_IDLE_STATUS; //WiFi connetion status
int light1 = 1;  //Pins for LEDs
int light2 = 2;
int light3 = 3;
int light4 = 5;
int fanA = 8;    //Pins for fan
int fanB = 9;
int moistureSensor = A6; //Pin for moisture sensor
int flagPos = 0;   //Whether flag is up or not
int flagPin = 10;  //Pin for flag


Servo flag; //Object to control flag servo

WiFiSSLClient wifi; //Wifi connection
HttpClient client = HttpClient(wifi, WEBSITE, SSLPORT); //SSL client

void setup() {
  //Set pin modes
  pinMode(light1,OUTPUT);
  pinMode(light2,OUTPUT);
  pinMode(light3,OUTPUT);
  pinMode(light4,OUTPUT);
  pinMode(fanA,OUTPUT);
  pinMode(fanB,OUTPUT);
  pinMode(moistureSensor,INPUT);
  setTime(12,0,0,1,1,2000); //Set the current time to midday
  flag.attach(flagPin); //Attach the servo

  Serial.begin(9600);
  while (!Serial); //Block if no serial connection

  //Connect to WiFi
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(MY_SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(MY_SSID, MY_PWD);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  //Connect to env shield
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
}

//Output the WiFi status
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//Turn on all the LEDs
void turnOnTheLights() {
  digitalWrite(light1,HIGH);
  digitalWrite(light2,HIGH);
  digitalWrite(light3,HIGH);
  digitalWrite(light4,HIGH);
}

//Turn off all the LEDs
void turnOffTheLights() {
  digitalWrite(light1,LOW);
  digitalWrite(light2,LOW);
  digitalWrite(light3,LOW);
  digitalWrite(light4,LOW);
}

//Turn on the fan
void turnOnFan() {
  digitalWrite(fanA,HIGH);
  digitalWrite(fanB,LOW);
}

//Turn off the fan
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
  int flagS = 0;
  int lightsS = 0;
  int fanS = 0;

  //Check moisture and raise the flag if needed
  float moisture = analogRead(moistureSensor);
  if (moisture < 200) {
    flag.write(90);
    flagS = 1;
  } else {
    flag.write(0);
    flagS = 0;
  }

  //Turn the lights on or off depending on time and illuminance
  time_t t = now();
  if (hour(t)>=22 or hour(t)<7 or illuminance < minIllum) {
    turnOnTheLights();
    lightsS = 1;
  } else {
    turnOffTheLights();
    lightsS = 0;
  }
  //Turn the fan on or off depending on time and temperature
  if (minute(t)<0 or temperature > maxTemp) {
    turnOnFan();
    fanS = 1;
  } else {
    turnOffFan();
    fanS = 0;
  }


  // print each of the sensor values
  Serial.print("Moisture    = ");
  Serial.println(moisture);

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

  //Send data to Google sheet
  Serial.println("\nSending Data to Server...");
  //Build send string
  String URL = (String) "/macros/s/"+DEPLOYMENTID+"/exec?"
    + "temp=" + (String) temperature
    + "&humid=" + (String) humidity
    + "&illum=" + (String) illuminance
    + "&moist=" + (String) moisture
    + "&lights=" + (String) lightsS
    + "&fan=" + (String) fanS
    + "&flag=" + (String) flagS;
  String newFullURL; //Create new full URL in case of redirect
  client.get(URL); //call the web file
  int statusCode = client.responseStatusCode(); //Get the response code
  Serial.print("Status code: ");
  Serial.println(statusCode);
  if (statusCode == 302) { //Response code 302 means redirect
    while(client.headerAvailable()) //Go through headers
    {
      String headerName = client.readHeaderName();
      String headerValue = client.readHeaderValue();
      if (headerName == "Location") { //Get the location header for recdirect
        newFullURL = headerValue;
      }
    }
    newFullURL.remove(0,8); //Remove protocol
    String newHOST = newFullURL.substring(0,newFullURL.indexOf("/")); //Get host
    String newURL = newFullURL.substring(newFullURL.indexOf("/")); //Get file link
    HttpClient client2 = HttpClient(wifi, newHOST, SSLPORT); //Connect to new web file
    client2.get(newURL); //Get the new response
    statusCode = client2.responseStatusCode(); //Check the new status code - should be 200
    Serial.print("Status code: ");
    Serial.println(statusCode);
  }

  delay(60000); //Wait 1m before running again.
}
