//DHT, Motion and OLED sketch

#define MQTT_SOCKET_TIMEOUT 120

/************ HARDWARE CONFIG (CHANGE THESE FOR YOUR SENSOR SETUP) ******************/
//#define OLED_SPI //Uncomment if using SPI OLED screen (assuming I2C otherwise)
//#define DEEPSLEEP //Uncomment if you want sensor to sleep after every update (Does NOT work with MOTION_ON or LED_ON which require constant uptime) - Note: Screen will turn off/on during reboot cycle
#define FLIP_SCREEN //Uncomment if mounting to wall with USB connector on top
#define MOTION_ON //Uncomment if using motion sensor
//#define OLED_MOTION //Uncomment if you want screen to turn on only if motion is detected
//#define LED_ON //Uncomment if using as LED controller

/************ WIFI and MQTT INFORMATION (CHANGE THESE FOR YOUR SETUP) ******************/
#define wifi_ssid "ssid" //enter your WIFI SSID
#define wifi_password "password" //enter your WIFI Password
#define mqtt_server "mqtt_server" // Enter your MQTT server address or IP.
#define mqtt_device "mqtt_device" //MQTT device
#define mqtt_user "" //enter your MQTT username
#define mqtt_password "" //enter your password

/****************************** MQTT TOPICS (change these topics as you wish)  ***************************************/
#define temperaturepub "home/mqtt_device/temperature"
#define humiditypub "home/mqtt_device/humidity"
#define tempindexpub "home/mqtt_device/temperatureindex"
#define motionpub "home/mqtt_device/motion"

/*****************************************************/

#define DHTPIN 4      // (D2) what digital pin we're connected to
#define MOTIONPIN 5   // (D1) what digital pin the motion sensor is connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#ifdef OLED_SPI
  #include "SSD1306Spi.h" //OLED Lib for SPI version
#else 
  #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
  #include "SSD1306.h" //OLED Lib for I2C version
#endif

DHT dht(DHTPIN, DHTTYPE);

unsigned long currentMillis = 60001; //Set so temperature's read on first scan of program
unsigned long previousMillis = 0;
unsigned long interval = 60000;

float h,t,f;
int motionState;

WiFiClient espClient;
PubSubClient client(espClient);

#ifdef OLED_SPI
  SSD1306Spi  display(D10, D9, D8); //RES, DC, CS (SPI Initialization)
#else
  SSD1306     display(0x3c, 3 /*D9*/, 1 /*D10*/); //0 /*D3*/, 2 /*D4*/); //SDA, SCL (I2C Initialization)
#endif


void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 PIR/OLED/DHT Sensor");

  dht.begin();
  
  pinMode(MOTIONPIN, INPUT);

  display.init();

  #ifdef FLIP_SCREEN
    display.flipScreenVertically(); 
  #endif
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
    
  setup_wifi();

  client.setServer(mqtt_server, 1883); //CHANGE PORT HERE IF NEEDED

}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  //display.drawString(0,0,"Wifi: " + String(wifi_ssid));
  //display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  //WiFi.printDiag(Serial);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //display.setTextAlignment(TEXT_ALIGN_RIGHT);
  //display.drawString(120,10, "OK");
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.drawString(0,20, "IP: " + WiFi.localIP());
  //display.display();
}

void drawDHT(float h, float t, float f)
{
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,0, String(mqtt_device));
  display.setFont(ArialMT_Plain_24);  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(60,18, String(f) + " *F");
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,50,"Hum: ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(120,50, String(h) + " %");
}

void loop() {

  char strCh[10];
  String str;
  
 if (!client.loop()) {
  reconnect();
 }
 
 
  if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
  
      h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      f = dht.readTemperature(true);
    
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        h=t=f=-1;
      }
    
      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
    
      str = String(f,2);
      str.toCharArray(strCh,9);
      client.publish(temperaturepub, strCh);
      client.loop();
      str = String(h,2);
      str.toCharArray(strCh,9);
      client.publish(humiditypub, strCh);
      client.loop();
      str = String(hif,2);
      str.toCharArray(strCh,9);
      client.publish(tempindexpub, strCh);
      client.loop();
      
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hic);
      Serial.print(" *C ");
      Serial.print(hif);
      Serial.println(" *F");

      display.invertDisplay();
      display.display();
      display.clear();
      display.normalDisplay();
      drawDHT(h,t,f);
      display.display();
  }
  currentMillis = millis();

#ifdef MOTION_ON
  motionState = digitalRead(MOTIONPIN);
//   print out the state of the button:
  if (motionState == 1)
{
    Serial.println("Motion Event:");
    client.publish(motionpub, "1");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(120,0, "M");
    display.display();
    client.loop();
    delay (4000);
    display.clear();
    drawDHT(h,t,f);
    display.display();
  }
#else
  #ifdef DEEP_SLEEP
   ESP.deepSleep(60000000,WAKE_RF_DEFAULT);   //If a motion sensor is not being used, we can put ESP into deep-sleep to save energy/wifi channel
  #endif
#endif
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //display.drawString(0,30, "MQTT: ");
    //display.display();

    // Attempt to connect
  if (client.connect(mqtt_device, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      //display.setTextAlignment(TEXT_ALIGN_RIGHT);
      //display.drawString(120,40, "OK");
      //display.display();

      client.subscribe(temperaturepub);
      client.subscribe(humiditypub);
      client.subscribe(tempindexpub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

