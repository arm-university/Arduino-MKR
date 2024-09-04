#include <WiFi101.h> //MKR Wifi1000
#include <WiFiNINA.h> //MKR Wifi1010
#include <LiquidCrystal.h>

const char* MY_SSID = "ENTER YOUR SSID HERE";
const char* MY_PWD =  "ENTER YOUR WIFI PASSWORD HERE";
const int btn = 8; //Button pin
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
char serverName[] = "random-ize.com";  //Host of the website
char path[] = "/bad-jokes/bad-jokes-f.php"; //Path of webpage

int status = WL_IDLE_STATUS;
String htmlContent = "";
String prompt = "";
String punchline = "";

WiFiSSLClient client; //The client connection (Note: SSL alows us to use https)
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); The LCD screen

void setup() {
  pinMode(btn, INPUT); //Set up the input button
  lcd.begin(16, 2); //Begin the LCD
  lcd.clear(); //Clear the LCD
  lcd.home(); //Go back to 0,0
  lcd.print("Wait!"); //Instruct use to wait for connection

  Serial.begin(9600); //Begin serial monitor

  //Check if WiFi is possible
  if (WiFi.status() == WL_NO_SHIELD || WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi device not present");
    while (true);     // don't continue:
  }

  while (status != WL_CONNECTED)  //Try and connect to WiFi
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(MY_SSID);
    status = WiFi.begin(MY_SSID, MY_PWD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  Serial.println("Connected to wifi"); 
  printWifiStatus(); 
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
  //Report to user
  lcd.clear();
  lcd.home();
  lcd.print("Ready!");
}

void loop()
{
  //Tell the website to send data
  if (digitalRead(btn) == HIGH) {
    //Button pressed
    Serial.println("\nStarting connection to server...");

    // if you get a connection, report back via serial:
    if (client.connect(serverName, 443)) {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.print("GET ");
      client.print(path);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(serverName);
      client.println("Connection: close");
      client.println();
    }
  } else {
    //Buton not pressed
  }
  
  while (client.available()) {
    char c = client.read();
    htmlContent += c;
  }
  
  if (!client.connected() && htmlContent != "") {
    client.stop(); //Close the client connection

    //Get the prompt and punchline
    int start = htmlContent.indexOf(">")+1;
    prompt = htmlContent.substring(start);
    int end = prompt.indexOf("<");
    punchline = prompt.substring(end+8);
    prompt = prompt.substring(0,end);
    end = punchline.indexOf("<");
    punchline = punchline.substring(0,end);
    //Display on serial for debugging
    Serial.print("Prompt: ");
    Serial.println(prompt);
    Serial.print("Punchline: ");
    Serial.println(punchline);
    
    //Pad to help them display nicer. 
    String padding = "                ";
    prompt = padding + prompt + padding;
    punchline = padding + punchline + padding;
    lcd.setCursor(15, 0);
    lcd.clear();

    int s = 0;
    int e = 16;
    String l = "";
    int smax = prompt.length()-16;
    while (s <= smax) {
      l = prompt.substring(s,e);
      lcd.setCursor(0,0);
      lcd.print(l);
      s += 1;
      e += 1;
      delay(200);
    }
    
    s = 0;
    e = 16;
    smax = punchline.length()-16;
    while (s <= smax) {
      l = punchline.substring(s,e);
      lcd.setCursor(0,1);
      lcd.print(l);
      s += 1;
      e += 1;
      delay(200);
    }
    htmlContent = "";
  }
}


