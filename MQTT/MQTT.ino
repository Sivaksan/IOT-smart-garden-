#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleTimer.h>
#include <DHT.h>


#define DHTTYPE DHT11   // DHT 11

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PSSWD";

const char* mqtt_server = "192.168.1.16";

/ Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

#define DHTTYPE DHT11 
#define DHTPIN D8
#define DHTVCC D2
#define soilMoisterPin A0
#define soilMoisterVcc D6
#define ptrVcc D5
#define relay_pump D4
#define relay_leds D7

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);


//Define variables
float hum = 0;
float temp = 0;
int soilMoister = 0;
int lum =0;
int soilPourcents =0;
long lastRecu =0;


int seuil_lum = 60;
int seuil_moisture = 80;
int time_pump_ON = 2000; 
bool leds_ON = false;
//Flag master if command received => the actuators won't be triggered while flag_master == 1
int Flag_master_LED =0;

SimpleTimer timer;

//Delcaration of prototypes 
void getData();

void setup_wifi() {
  delay(10);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void callback(String topic, byte* message, unsigned int length) {
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();


     if(topic=="Garden/Lights")
      {
          Serial.print(" Switching Leds ");
          if(messageTemp == "true"){
            Flag_master_LED = 1;
            digitalWrite(relay_leds, LOW);
            Serial.print("On");
          }
          else if(messageTemp == "false"){
            Flag_master_LED =0;
            digitalWrite(relay_leds, HIGH);
            Serial.print("Off");
            }
        
      }
      else if(topic=="Garden/Water")
      {
      Serial.print(" Switching water pump ");
      if(messageTemp == "true"){
        digitalWrite(relay_pump, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "false"){
        digitalWrite(relay_pump, HIGH);
        Serial.print("Off");
      }
  }
  
  Serial.println();
}


void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("Garden/Lights");
      client.subscribe("Garden/Water");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.println("start setup \n");
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
  
  digitalWrite(relay_pump,HIGH); 
  digitalWrite(relay_leds,HIGH);
  dht.begin();
  //timer.setInterval(12000L,getData);
  
  Serial.begin(115200);
  setup_wifi();
  // Set the callback function, the server IP and port to communicate with
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe("Garden/Lights");
  client.subscribe("Garden/Water");
  delay(2000);



Serial.println("End setup \n");

}
void getDhtData(void)
{
  digitalWrite (DHTVCC,HIGH);
  delay(500);
  float tempIni = temp;
  float humIni = hum;
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(temp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    temp = tempIni;
    hum = humIni;
    return;
  }
  Serial.println("Temperature :");
  Serial.println(temp);
  Serial.println("Humidity :");
  Serial.println(hum);
  
  float hic = dht.computeHeatIndex(temp, hum, false);
  static char temperatureTemp[7];
  dtostrf(hic, 6, 2, temperatureTemp);

  static char humidityTemp[7];
  dtostrf(hum, 6, 2, humidityTemp);
  client.publish("Garden/Temperature", temperatureTemp);
  client.publish("Garden/Humidity", humidityTemp);

  
}
void loop() 
{
 if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
    long now= millis();
getData();
delay(5000);
client.loop();


}

void getData()
 {
  
  Serial.println("analysis started");
  getDhtData();
  getSoilMoisterData();
  ProcessingLights();
  Processing_Moisture();
 }

void getSoilMoisterData(void)
{
  digitalWrite (DHTVCC,LOW);
  digitalWrite (soilMoisterVcc, HIGH);
  delay (500);  
  soilMoister = analogRead(soilMoisterPin);
  soilMoister = (soilMoister *100)/1024;
  soilPourcents = soilMoister;
  static char soilTemp[7];
  dtostrf(soilMoister, 6, 2, soilTemp);
  client.publish("Garden/Soil", soilTemp);
  
  Serial.println("Soil moisture measured:");
  Serial.println(soilMoister);
  soilMoister = map(soilMoister, 380, 0, 0, 100); 
  digitalWrite (soilMoisterVcc, LOW);  
  delay(500);  
  digitalWrite (ptrVcc,HIGH);
  delay(500);  
  lum = analogRead(soilMoisterPin);
  lum = (lum *100)/1024;
  static char lumTemp[7];
  dtostrf(lum, 6, 2, lumTemp);
  client.publish("Garden/Lum", lumTemp);
  
  Serial.println("Light measured:");
  Serial.println(lum);
  digitalWrite(ptrVcc,LOW);  
}
void ProcessingLights()
{
  if(Flag_master_LED ==0)
  {
  Serial.println("Start processing Light \n");
  if(lum < seuil_lum && (leds_ON == false ))
  {
    Serial.println("LEDs ON");
    digitalWrite(relay_leds,LOW);
  }
  else if(lum < seuil_lum && (leds_ON == true))
  {
    Serial.println("LEDs already ON");
  }
  else
  {
    Serial.println("LEDs OFF");
    digitalWrite(relay_leds,HIGH);
  }
  }
  else 
  {
    Serial.println("flag_master_LEDs : ON");
  }
  delay(200);
}

void Processing_Moisture()
{
  Serial.println("Processing water pump \n");
  if(soilPourcents < seuil_moisture)
  {
    Serial.println("PUMP ON");
    digitalWrite(relay_pump,LOW);
    delay(10000);
    digitalWrite(relay_pump,HIGH);
    Serial.println("PUMP OFF \n");

  }
  else
  {
     Serial.println("PUMP OFF");
     digitalWrite(relay_pump,LOW);
  }  
}
