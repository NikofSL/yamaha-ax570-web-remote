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
/////IR Date for Send - Yamaha KX-580
uint64_t play = 0xFE0100FF ;
uint64_t stop_p = 0xFE01C03F;
uint64_t ffwd = 0xFE0140BF;
uint64_t rewind_p = 0xFE01807F;


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index.h"

/********** PLEASE CHANGE THIS *************************/
const char* ssid     = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

ESP8266WebServer server(80);

bool POWERstatus = LOW;


void setup() {
  irsend.begin();

  Serial.begin(115200);
  
 

  
  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/togglePOWER", updatePOWER);
  server.on("/set_b_POWER", set_b_POWER);
  server.on("/volume_set", volume_set);
  server.on("/output_set", output_set);
  server.on("/deck_c", deck_c);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", prepareHTML());
}

void updatePOWER() {
  String powerStatusParam = server.arg("powerStatus");
  if (powerStatusParam == "ON"){
    POWERstatus =  HIGH;
    irsend.sendNEC(power, 32);
    Serial.println("Power ON");}
  else if ("OFF"){
    POWERstatus =  LOW;
    irsend.sendNEC(power, 32);
    Serial.println("Power OFF");}

  server.send(200, "text/plain", POWERstatus?"HIGH":"LOW");
}
void set_b_POWER() {
  String temp = "eho";
  if (POWERstatus){
    temp = "true";}
  else{
    temp = "false";}

  server.send(200, "text/plain", temp);
}

void volume_set() {
  String volumeStatusParam = server.arg("volume_state");
  if (volumeStatusParam == "UP"){
    Serial.println("Volume UP");
    for(int pp=0; pp <4; pp++){
      irsend.sendNEC(vol_up, 32);
      delay(200);
    }
  }

  else{
    Serial.println("Volume DOWN");
    for(int pp=0; pp <4; pp++){
      irsend.sendNEC(vol_down, 32);
      delay(200);}}

  server.send(200, "text/plain", "Success!");
}
void output_set() {
  String outputStatusParam = server.arg("output_state");
  if (outputStatusParam == "AUX"){
    Serial.println("Output is AUX");
    irsend.sendNEC(aux, 32);}
  else if (outputStatusParam == "deck1"){
    Serial.println("Output is Deck 1");
    irsend.sendNEC(tape1, 32);}
    else if (outputStatusParam == "deck2"){
      Serial.println("Output is Deck 2");
      irsend.sendNEC(tape2, 32);}
      else if (outputStatusParam == "cd"){
        Serial.println("Output is CD");
        irsend.sendNEC(cd, 32);}
        else if (outputStatusParam == "tuner"){
          Serial.println("Output is Tuner");
          irsend.sendNEC(tuner, 32);}
          else if (outputStatusParam == "phono"){
            Serial.println("Output is Phono");
            irsend.sendNEC(phono, 32);}

  server.send(200, "text/plain", outputStatusParam);
}
void deck_c() {
  String deck_cParam = server.arg("deck_c");
  if (deck_cParam == "play"){
    Serial.println("Play Deck");
    irsend.sendNEC(play, 32);
    delay(200);
  }
  else if (deck_cParam == "stop"){
    Serial.println("Deck Stop Play");
    irsend.sendNEC(stop_p, 32);
    delay(200);
  }
  else if (deck_cParam == "ffwd"){
    Serial.println("Deck Forward");
    irsend.sendNEC(ffwd, 32);
    delay(200);
  }
  else if (deck_cParam == "rewind"){
    Serial.println("Deck Rewind");
    irsend.sendNEC(rewind_p, 32);
    delay(200);
  }


  server.send(200, "text/plain", deck_cParam);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

String prepareHTML() {
  // BuildMyString.com generated code. Please enjoy your string responsibly.
  String html = webpage;
  return html;
}
