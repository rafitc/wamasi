#include <ESP8266WiFi.h>
#include "Ubidots.h"
#define PULSE_PIN D2  //gpio4

const char* UBIDOTS_TOKEN = "YOUR_TOKEN_IS_HERE";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "WIFI_SSD"; // Put here your Wi-Fi SSID
const char* WIFI_PASS = "WIFI_PASS"; // Put here your Wi-Fi password
char const * HTTPSERVER = "things.ubidots.com";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
const int trigP = 2;  //D4 Or GPIO-2 of nodemcu
const int echoP = 0;  //D3 Or GPIO-0 of nodemcu
float cost;        //cost calculator
long duration;
int distance;

volatile long pulseCount=0;
float calibrationFactor = 4.5;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;
float charge;
unsigned long oldTime;

void ICACHE_RAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  pinMode(trigP, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoP, INPUT);   // Sets the echoPin as an Input

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0; 

 
  pinMode(PULSE_PIN, INPUT);
  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
   
}

void loop() {
  digitalWrite(trigP, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay 
  digitalWrite(trigP, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trigP, LOW);   // trigPin low
  duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
  distance= duration*0.034/2;        //Calculating actual/real distance
  Serial.print("Distance = ");        //Output distance on arduino serial monitor 
  Serial.println(distance);

  if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;     
    totalLitres = totalMilliLitres * 0.001;
    unsigned int frac;   
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalLitres);
    Serial.println("L");
    //Serial.print(totalMilliLitres);
    //Serial.println("mL");
    pulseCount = 0;
    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
  }
  float dist =distance;
  ubidots.add("distance", dist);
  float flow= totalLitres;
  ubidots.add("Total litres",flow);
  charge=7.5;
  cost=charge*totalLitres;
  ubidots.add("cost",cost);
  float rate=flowRate;
  ubidots.add("flow rate",rate);
  bool bufferSent = false;
  bufferSent = ubidots.send();  // Will send data to a device label that matches the device Id

  if (bufferSent) {
    // Do something if values were sent properly
       Serial.println("Values sent by the device");
   }
  delay(1000);

}
