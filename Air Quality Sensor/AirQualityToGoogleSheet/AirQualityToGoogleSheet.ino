#include "Air_Quality_Sensor.h"
#include <ArduinoHttpClient.h> //Handles connection to Google sheet file
//#include <WiFi101.h> //MKR1000
#include <WiFiNINA.h> #wifi1010 //MKR1010

AirQualitySensor sensor(A0);
const char WEBSITE[] = "script.google.com";
const int SSLPORT = 443;
const char* MY_SSID = "ENTERYOURSSIDEHERE";
const char* MY_PWD =  "ENTERYOURPASSWORDHERE";
const char DEPLOYMENTID[] = "ENTERYOURDEPLOYMENTIDHERE";

int status = WL_IDLE_STATUS;

WiFiSSLClient wifi; //Wifi connection
HttpClient client = HttpClient(wifi, WEBSITE, SSLPORT); //SSL client

void setup(void) {
    //Initialise serial monitor
    Serial.begin(9600);
    while (!Serial);

    //Wait for the air quality sensort to initialise.
    //This can take some time so wait 20s
    Serial.println("Waiting for sensor to initialise...");
    delay(20000);

    if (sensor.init()) {
        Serial.println("Sensor ready.");
    } else {
        Serial.println("Sensor error.");
    }

   //Check if WiFi possible (shouldn't be a problem if you have MKR1000 or 1010)
   if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi device not present");
    //while (true);     // don't continue:
  }

  //Connect to WiFi
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(MY_SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(MY_SSID, MY_PWD);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop(void) {
    int aqPollutionVal = sensor.slope();
    int aqReading = sensor.getValue();
    Serial.print("Sensor value: ");
    Serial.println(aqReading);

    if (aqPollutionVal == AirQualitySensor::FORCE_SIGNAL) {
        Serial.println("High pollution! Force signal active.");
    } else if (aqPollutionVal == AirQualitySensor::HIGH_POLLUTION) {
        Serial.println("High pollution!");
    } else if (aqPollutionVal == AirQualitySensor::LOW_POLLUTION) {
        Serial.println("Low pollution!");
    } else if (aqPollutionVal == AirQualitySensor::FRESH_AIR) {
        Serial.println("Fresh air.");
    }
  //Send data to Google sheet
  Serial.println("\nSending Data to Server...");
  //Build send string
  String URL = (String) "/macros/s/"+DEPLOYMENTID+"/exec?"
    + "aqReading=" + (String) aqReading;
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
  delay(10000); //Wait 10s before resending
}

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
