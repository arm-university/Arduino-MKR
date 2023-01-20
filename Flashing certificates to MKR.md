# Flashing new certificates to an Arduino MKR

For the Arduino MKR to connect to https web servers, it needs to have the relevant certificates installed on it.

## Step 1
Connect yout Arduino to your computer and use the latest *downloaded* version of Arduino Creator from https://www.arduino.cc/en/software

## Step 2
Test your Arduino connection is working by running the *blink* sketch. Remember to change the timing in case the Arduino is already flashed with blink.

```C
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```

## Step 3
Load the Firmware updater from the Tools menu. The example shows the menu on a Mac for a MKR 1010, Windows (and the MKR 1000) should be very similar.
![image](https://user-images.githubusercontent.com/27299926/123954355-35784000-d9a0-11eb-8e24-c2ec00152e5f.png)

Select the board in the list and click to open the Updater sketch.
![image](https://user-images.githubusercontent.com/27299926/123954604-838d4380-d9a0-11eb-8163-39478638d737.png)

## Step 4
Upload the updater sketch to your Arduino.
![image](https://user-images.githubusercontent.com/27299926/123954697-9bfd5e00-d9a0-11eb-81a9-c62e1ecf3f68.png)

Test the connection to ensure everything is working.
![image](https://user-images.githubusercontent.com/27299926/123954779-b2a3b500-d9a0-11eb-9ad5-c7df9b29c559.png)

## Step 5
Click on add a domain and enter the domain and port needed. If you are following the Arm book it will be google.com:443
![image](https://user-images.githubusercontent.com/27299926/123954887-d830be80-d9a0-11eb-80d1-7722cae81b06.png)

Click to upload the certificates to the Arduino.
![image](https://user-images.githubusercontent.com/27299926/123954933-ea126180-d9a0-11eb-9501-40df7aac4872.png)

## Step 6
Create a sketch to test it has worked.

Select to manage libraries from the sketch menu.
![image](https://user-images.githubusercontent.com/27299926/123955454-89375900-d9a1-11eb-983b-0756dca94e46.png)

Search for the ArduinoHTTPLibrary and click to install it.
![image](https://user-images.githubusercontent.com/27299926/123955655-c0a60580-d9a1-11eb-9442-25b451a558d3.png)

Use the following code and upload to your Arduino to check the connection works.
```c
#include <ArduinoHttpClient.h> //Handles connection to Google sheet file
#include <WiFiNINA.h> #wifi1010 //MKR1010


int status = WL_IDLE_STATUS;

const char WEBSITE[] = "www.google.com";
const int SSLPORT = 443;
const char* MY_SSID = "GDog";
const char* MY_PWD = "AnnaDillon69^^";
bool flag = false;

WiFiSSLClient wifi; //Wifi connection
HttpClient client = HttpClient(wifi, WEBSITE, SSLPORT); //SSL client

void setup(void) {
    //Initialise serial monitor
    Serial.begin(9600);
    while (!Serial);
    
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
  if (!flag) {
    Serial.println("\nSending Data to Server..."); 
    String URL = (String) "/";
    client.get(URL); //call the web file
    int statusCode = client.responseStatusCode(); //Get the response code
    Serial.print("Status code: "); 
    Serial.println(statusCode);
    if (statusCode == 302 or statusCode == 301) { //Response code 301/302 means redirect
      while(client.headerAvailable()) //Go through headers
      {
        String headerName = client.readHeaderName();
        String headerValue = client.readHeaderValue();
        Serial.println(headerName);
        Serial.println(headerValue);
      }
    } else {
      if (statusCode == 200) {
        flag = true;
        Serial.println("All fine!");
      }
    }
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
```
