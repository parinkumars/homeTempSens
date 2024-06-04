
#define BLYNK_TEMPLATE_ID "<Your Template ID>"
#define BLYNK_TEMPLATE_NAME "<Your Template Name>"

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include "Arduino.h"
#include <stdbool.h>


#define BLYNK_AUTH_TOKEN "<Your Auth Token>"
#define DHT_TYPE DHT11
#define DHT_PIN 5
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
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
        i==1?i--:;
        i--;
        //Displays SOS sequence in the builtin led ;)
    }

void readAndSendValues(){
    humidity = dht.readHumidity();
    temp = dht.readTemperature();//reads temp values in celcius
    if(isnan(humidity) || isnan(temp)){
        Serial.println("Failed to read from DHT sensor!");
        //Serial.print(humidity);
        //Serial.print(temp);
        Panic(5);
        return;
    }
    Blynk.virtualWrite(V0, temp); 
    Blynk.virtualWrite(V1, humidity);
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
    ESP.deepSleep(15 * MIN_TO_SEC_CONV * SEC_TO_MICROSEC_CONV);
    //system_deep_sleep_instant();
    }
  else{
    digitalWrite(LED_BUILTIN,HIGH);
    return;
  }
}

void setup(){
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
  Serial.println("Beginning Si1145!");
  Serial.println("Si1145 is ready!");
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

