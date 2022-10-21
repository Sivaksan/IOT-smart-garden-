#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>


char auth[] = "get auth token in blynk app and put here";


char ssid[] = "sivakshan";
char pass[] = "sivaksan@2k";

/* DHT11*/
//#include "DHT.h"

#define DHTTYPE DHT11 
#define DHTPIN D8
#define DHTVCC D2
#define soilMoisterPin A0
#define soilMoisterVcc D6
#define ptrVcc D5
#define relay_pump D4
#define relay_leds D7

DHT dht(DHTPIN, DHTTYPE);


//Define variables
float hum = 0;
float tem = 0;
int soilMoister = 0;
int lum =0;
SimpleTimer timer;


int s_lum = 200;
int s_moisture = 300;
int time_pump_ON = 1000; //
bool leds_ON = false;

//declare the widgets 

WidgetLED led1(V4);
WidgetLED led2(V5);
void setup()
{
  //Soil moisture
  pinMode(soilMoisterVcc,OUTPUT);
  digitalWrite(soilMoisterVcc,LOW);
  //DHT
  pinMode(DHTVCC,OUTPUT);
  digitalWrite(DHTVCC,LOW);
  pinMode(DHTPIN,INPUT);
  //LDR
  pinMode(ptrVcc,OUTPUT);
  digitalWrite(ptrVcc,LOW);
  //Relays
  pinMode(relay_pump, OUTPUT);
  pinMode(relay_leds, OUTPUT);
  
  digitalWrite(relay_pump,LOW); 
  digitalWrite(relay_leds,HIGH);
  
  Serial.begin(115200); 
   delay(10);
   //BLYNK
  Blynk.begin(auth, ssid, pass); 
  dht.begin();

  led1.on();
  led2.on();
  timer.setInterval(12000L,getData);

}
void getData()
 {
  Serial.println("analysis started");
  getDhtData();
  getSoilMoisterData();
  ProcessingLights();
  Processing_Moisture();
 }
void loop()
{
  Blynk.run(); 
  timer.run(); 
}
void getDhtData(void)
{
  digitalWrite (DHTVCC,HIGH);
  delay(500);
  float temIni = tem;
  float humIni = hum;
  tem = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(tem))  
  {
    Serial.println("Failed to read from DHT sensor!");
    tem = temIni;
    hum = humIni;
    return;
  }
  Serial.println("Temperature :");
  Serial.println(tem);
  Serial.println("Humidity :");
  Serial.println(hum);
  Blynk.virtualWrite(V2, hum);
  Blynk.virtualWrite(V0, tem);
  
}


void getSoilMoisterData(void)
{
  
  digitalWrite (DHTVCC,LOW);
  digitalWrite (soilMoisterVcc, HIGH);
  delay (500);  
  soilMoister = analogRead(soilMoisterPin);
  Serial.println("Soil moisture measured:");
  Serial.println(soilMoister);
  soilMoister = (soilMoister *100) /1024;
  Blynk.virtualWrite(V3, soilMoister);
  digitalWrite (soilMoisterVcc, LOW);  
  delay(500);  
  digitalWrite (ptrVcc,HIGH);
  delay(500);  
  lum = analogRead(soilMoisterPin);
  lum = (lum*100)/1024;
  Serial.println("Light measured:");
  Serial.println(lum);
  Blynk.virtualWrite(V1,lum);
  digitalWrite(ptrVcc,LOW);  
}

void ProcessingLights()
{
  if(lum < s_lum && (leds_ON == false ))
  {
    Serial.println("LEDs ON");
    digitalWrite(relay_leds,LOW);
    Blynk.setProperty(V5, "color", "#00FF2E");
  }
  else if(lum < s_lum && (leds_ON == true))
  {
    Serial.println("LEDs already ON");
  }
  else
  {
    Serial.println("LEDs OFF");
    digitalWrite(relay_leds,HIGH);
    Blynk.setProperty(V5, "color", "#FF0000");
  }
  delay(200);
}
void Processing_Moisture()
{
  if(soilMoister < s_moisture)
  {
    Serial.println("PUMP OFF");
     digitalWrite(relay_pump,LOW);
     Blynk.setProperty(V4, "color", "#FF0000");
  }
  else
  {
    Serial.println("PUMP ON");
    digitalWrite(relay_pump,HIGH);
    delay(time_pump_ON);
    digitalWrite(relay_pump,LOW);
    Blynk.setProperty(V4, "color", "#00FF2E");
  }  
}
