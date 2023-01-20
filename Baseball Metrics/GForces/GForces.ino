
#include "BMA400.h"
#include <ArduinoHttpClient.h> //Handles connection to Excel file
#include <WiFi101.h> //MKR1000
//#include <WiFiNINA.h> #wifi1010 //MKR1010

float x = 0, y = 0, z = 0;
int buttonPin = 13;
int readingsCount = -1;
double readings[200];

const char WEBSITE[] = "script.google.com";
const int SSLPORT = 443;
const char* MY_SSID = "YOURWIFISSID";
const char* MY_PWD =  "YOURPASSWORD";
const char DEPLOYMENTID[] = "YOURDEPLOYMENTID";

int status = WL_IDLE_STATUS;

WiFiSSLClient wifi; //Wifi connection
HttpClient client = HttpClient(wifi, WEBSITE, SSLPORT); //SSL client

void setup(void) {
  pinMode(buttonPin,INPUT_PULLUP);
  Wire.begin();

  Serial.begin(115200);
  delay(10000); //wait 10s as may not be connected to serial
  Serial.println("BMA400 Raw Data");

  while (1) {
      if (bma400.isConnection()) {
          bma400.initialize();
          Serial.println("BMA400 is connected");
          break;
      } else {
          Serial.println("BMA400 is not connected");
      }

      delay(2000);
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
  int buttonPressed = digitalRead(buttonPin);
  if (buttonPressed == LOW) {
    readingsCount = 0;
  }
  while (readingsCount >=0 and readingsCount < 200) {
    bma400.getAcceleration(&x, &y, &z);
    double g = sqrt(x*x+y*y+z*z);
    Serial.print((g/1000));
    //Serial.print(",");
    readings[readingsCount] = g/1000;
    readingsCount++;
    delay(20);
  }
  if (readingsCount >= 0) {
    //Send data to Google sheet
    Serial.println("\nSending Data to Server...");
    //Build send string
    String URL = (String) "/macros/s/"+DEPLOYMENTID+"/exec?"
      + "data=";
    for (int i = 0; i < 200; i++) {
      Serial.print(readings[i]);
      Serial.print(",");
      URL += readings[i];
      URL += ",";
    }
    Serial.println("");
    Serial.println(URL);
    Serial.println("");
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
    readingsCount = -1;
  }
//Serial.print(int(x/1000));
  //Serial.print(",");
  //Serial.print(int(y/1000));
  //Serial.print(",");
  //Serial.print(int(z/1000));
  //Serial.print(",");

  //Serial.print(temp);

  //Serial.println();

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
