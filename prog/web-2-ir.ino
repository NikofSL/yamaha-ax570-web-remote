/*********
  Nikolai Georgiev
  Complete project details at https://github.com/NikofSL/yamaha-ax570-web-remote  
*********/

// Load IR library
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

////IR Data for Send - Yamaha AX-570
uint64_t power = 0x5EA1F807;
uint64_t vol_up = 0x5EA158A7;
uint64_t vol_down = 0x5EA1D827;
uint64_t tuner = 0x5EA16897;
uint64_t cd = 0x5EA1A857;
uint64_t phono = 0x5EA128D7;
uint64_t aux = 0x5EA1E817;
uint64_t tape = 0x5EA1E01F;
uint64_t tape1 = 0x5EA118E7;
uint64_t tape2 = 0x5EA19867;

// Load library for ESP8266 Wifi
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

void setup() {
    irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  
  
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();
}

void loop(){
   WiFiClient client = server.available();   // Listen for incoming clients

if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /on") >= 0) {
              Serial.println("Power on");
              output5State = "on";
              irsend.sendNEC(power, 32);
              delay(2000);
              //digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /off") >= 0) {
              Serial.println("Power off");
              output5State = "off";
              irsend.sendNEC(power, 32);
              delay(2000);
              //digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /aux") >= 0) {
              Serial.println("Change to AUX");
              output4State = "AUX";
              irsend.sendNEC(aux, 32);
              delay(2000);
              //digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /tuner") >= 0) {
              Serial.println("Change Output to - Tuner");
              output4State = "Tuner";
              irsend.sendNEC(tuner, 32);
              delay(2000);
              //digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /cd") >= 0) {
              Serial.println("Change Output to - CD");
              output4State = "CD";
              irsend.sendNEC(cd, 32);
              delay(2000);
              //digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /phono") >= 0) {
              Serial.println("Change Output to - Phono");
              output4State = "Phono";
              irsend.sendNEC(phono, 32);
              delay(2000);
              //digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /tape1") >= 0) {
              Serial.println("Change Output to - Cassete Deck(tape1)");
              output4State = "Cassete Deck(tape1)";
              irsend.sendNEC(tape1, 32);
              delay(2000);}
              else if (header.indexOf("GET /tape2") >= 0) {
              Serial.println("Change Output to - Cassete Deck(tape1)");
              output4State = "Cassete Deck(tape2)";
              irsend.sendNEC(tape2, 32);
              delay(2000);
              //digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /vol_up") >= 0) {
              Serial.println("Volume UP");
              for(int pp=0; pp <4; pp++){
              irsend.sendNEC(vol_up, 32);
              delay(200);
              }
              //digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /vol_down") >= 0) {
              Serial.println("Volume DOWN"); 
              for(int pp=0; pp <4; pp++){
              irsend.sendNEC(vol_down, 32);
              delay(200);
              }}
              
              //digitalWrite(output4, LOW);
            
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: gray;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 15px; text-align: center; width: 20%;");
            client.println("text-decoration: none; font-size: 30px; font-size: 2vw; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Web Remote Control For YAMAHA AX-570</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>Power is " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            
            client.println("<p>VOLUME</p>");
            client.println("<p><a href=\"/vol_up\"><button class=\"button\">Volume UP</button></a></p>");
            client.println("<p><a href=\"/vol_down\"><button class=\"button\">Volume Down</button></a></p>");


               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>Output is " + output4State + "</p>");
            client.println("<p><a href=\"/aux\"><button class=\"button\">AUX</button></a></p>");
            client.println("<p><a href=\"/tape1\"><button class=\"button\">Casette Deck(tape1)</button></a></p>");
            client.println("<p><a href=\"/tape2\"><button class=\"button\">Casette Deck(tape2)</button></a></p>");
            client.println("<p><a href=\"/cd\"><button class=\"button\">CD</button></a></p>");
            client.println("<p><a href=\"/tuner\"><button class=\"button\">Tuner</button></a></p>");
            client.println("<p><a href=\"/phono\"><button class=\"button\">Phono</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
