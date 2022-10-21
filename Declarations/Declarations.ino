#define DHTTYPE DHT11 
#define DHTPIN D8
#define DHTVCC D2
#define soilMoisterPin A0
#define soilMoisterVcc D6
#define ptrVcc D5
#define relay_pump D4
#define relay_leds D7

DHT dht(DHTPIN, DHTTYPE);



float hum = 0;
float tem = 0;
int soilMoister = 0;
int lum =0;



int s_lum = 200;
int s_moisture = 300;
int time_pump_ON = 1000; //
bool leds_ON = false;

SimpleTimer timer;

WidgetLED led1(V4);
WidgetLED led2(V5);
