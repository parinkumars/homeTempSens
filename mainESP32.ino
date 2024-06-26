//This file was created by Parin Kumar Susil on the 30th of may 2024

#define BLYNK_TEMPLATE_ID "<Your Template ID>"
#define BLYNK_TEMPLATE_NAME "<Your Template Name>"

#include "DHT.h"
#include <WiFi.h> 
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Arduino.h"
#include <stdbool.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp;

#define BLYNK_AUTH_TOKEN "<Your Auth Token>"
#define DHT_TYPE DHT11
#define DHT_PIN 15
#define SEC_TO_MICROSEC_CONV 1000000
#define MIN_TO_SEC_CONV 60

/*
Virtual Pins Legend
V0 --> Temperature
V1 --> Humidity
V2 --> Deep Sleep State Switch // I dont know if this will work as is right now?!

if pins are arranged different in the blink setup please edit the values of these pins in the code
*/

//Defining Parameters of the code

float temp;
float humidity;
float pressure;
int altitude;
int pinVal=0;

// WiFi Auth Details

char ssid[]="<WiFi ssid>";
char psswd[]="<WiFi Password>";

//Initialising DHT and Blync timer 

DHT dht(DHT_PIN,DHT_TYPE);
BlynkTimer timer;

void Panic(int i){
    /*
    Panic int i values > 0 yeilds board blinking led in sos pattern i times
    Panic int i value = 0 board outputs on serial that it has pannicked and blinks onboard led in sos pattern 1 time
    Panic int i value < 0 board outputs on serial that it has pannicked
    */
    digitalWrite(LED_BUILTIN,LOW);
    if(i<=0) Serial.print("Board has panicked!");
    while(i>=0){
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(3000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(3000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(3000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN,LOW);
        delay(1000);
        i==1?i-=2:i--;
        //Displays SOS sequence in the builtin led ;)
    }
}

void readAndSendValues(){
    humidity = dht.readHumidity();
    temp = dht.readTemperature();//reads temp values in celcius
    pressure = bmp.readPressure()/100;
    altitude = bmp.readAltitude(1010);
    if(isnan(humidity) || isnan(temp)){
        Serial.println("Failed to read from DHT sensor!");
        Panic(5);
        return;
    }
    Blynk.virtualWrite(V0, temp); 
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V3,pressure);
    Blynk.virtualWrite(V4,altitude);
}

/*
BLYNK_WRITE(V2){
  pinVal = param.asInt();
  Serial.printf("%d",pinVal);
}
*/

void checkMode(){
  if(digitalRead(26)==HIGH){
    digitalWrite(LED_BUILTIN,LOW);
    Serial.print("Entered Deep Sleep For 15 mins!");
    esp_sleep_enable_timer_wakeup(15 * MIN_TO_SEC_CONV * SEC_TO_MICROSEC_CONV);
    esp_deep_sleep_start();
    }
  else{
    digitalWrite(LED_BUILTIN,HIGH);
    return;
  }
}

void setup(){
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(26,INPUT);
  pinMode(V2,INPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  digitalWrite(25,HIGH);
  digitalWrite(26,LOW);
  Serial.begin(115200);
  Serial.println("Beginning Si1145!");
  Serial.println("Si1145 is ready!");
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
    }
  dht.begin();
  delay(1000);
  Blynk.begin(BLYNK_AUTH_TOKEN , ssid, psswd);
  delay(1000);
  //Serial.print("I was in setup1");
  //Serial.print("I was in setup2");
  timer.setInterval(1000L, readAndSendValues); // sendSensor function will run every 1000 milliseconds
  checkMode();//pins 25 and 26 are shorted on the board so as to give it a permenant deep sleep state and update temp every 15 mins, if this is undesired u can choose to remove the shunt wire.
}

void loop(){
  digitalWrite(LED_BUILTIN,HIGH);
  Blynk.run();
  timer.run();
  checkMode();
}
