//DHT, Motion and OLED sketch for "The Kube" sensor

//by bkpsu, https://www.thingiverse.com/thing:2539897

#define MQTT_SOCKET_TIMEOUT 120

/************ HARDWARE CONFIG (CHANGE THESE FOR YOUR SENSOR SETUP) ******************/
#define REMOTE //Uncomment to enable remote sensor functionality (Wifi & MQTT)
//#define OLED_SPI //Uncomment if using SPI OLED screen (assuming I2C otherwise)
#define CELSIUS //Uncomment if you want temperature displayed in Celsius
//#define DEEP_SLEEP //Uncomment if you want sensor to sleep after every update (Does NOT work with MOTION_ON or LED_ON which require constant uptime)
#define FLIP_SCREEN //Uncomment if mounting to wall with USB connector on top
#define MOTION_ON //Uncomment if using motion sensor
//#define OLED_MOTION //Uncomment if you want screen to turn on only if motion is detected
//#define LED_ON //Uncomment if using as LED controller
//#define PRESS_ON //Uncomment if using as Pressure monitor
#define OTA_UPDATE //Uncomment if using OTA updates - REMOTE also needs uncommenting

/************ WIFI, OTA and MQTT INFORMATION (CHANGE THESE FOR YOUR SETUP) ******************/
#define wifi_ssid "wifi_ssid" //enter your WIFI SSID
#define wifi_password "wifi_password" //enter your WIFI Password
#define mqtt_server "mqtt_server" // Enter your MQTT server address or IP.
#define mqtt_device "mqtt_device" //MQTT device for broker and OTA name
#define mqtt_user "" //enter your MQTT username
#define mqtt_password "" //enter your password
#define OTApassword "123" // change this to whatever password you want to use when you upload OTA
/****************************** MQTT TOPICS (change these topics as you wish)  ***************************************/
#define temperaturepub "home/mqtt_device/temperature"
#define humiditypub "home/mqtt_device/humidity"
#define tempindexpub "home/mqtt_device/temperatureindex"
#define motionpub "home/mqtt_device/motion"
#define presspub  "home/mqtt_device/pressure"

/****************************** DHT 22 Calibration settings *************/

float temp_offset = -12.2;
float hum_offset = 14.9;

/**************************************************/

#define DHTPIN 4      // (D2) what digital pin we're connected to
#define MOTIONPIN 5   // (D1) what digital pin the motion sensor is connected to
#define PRESSPIN 5    // (D1) what digital pin the motion sensor is connected to

#define DHTTYPE DHT22   // DHT 22/11 (AM2302), AM2321

#include <PubSubClient.h>
#include "DHT.h"
#include <ESP8266WiFi.h>

#ifdef OLED_SPI
  #include "SSD1306Spi.h" //OLED Lib for SPI version
#else
  #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
  #include "SSD1306.h" //OLED Lib for I2C version
#endif

#ifdef OTA_UPDATE && REMOTE
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

DHT dht(DHTPIN, DHTTYPE);

unsigned long currentMillis = 60001; //Set so temperature's read on first scan of program
unsigned long previousMillis = 0;
unsigned long interval = 60000;

float h,t,f,p;
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

  #ifdef MOTION_ON
    pinMode(MOTIONPIN, INPUT);
  #endif
  #ifdef PRESS_ON
    pinMode(PRESSPIN, INPUT);
  #endif

  display.init();

  #ifdef FLIP_SCREEN
    display.flipScreenVertically();
  #endif
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  #ifdef REMOTE
    setup_wifi();

    client.setServer(mqtt_server, 1883); //CHANGE PORT HERE IF NEEDED
  #else
    WiFi.mode(WIFI_OFF);
  #endif

  #ifdef OTA_UPDATE && REMOTE
    setupOTA();
  #endif
}

void setupOTA (){
    ArduinoOTA.setPort(8266); // change this is you wish to use a different port
    ArduinoOTA.setHostname(mqtt_device);
    ArduinoOTA.setPassword((const char *)OTApassword);

    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
      ESP.restart();
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.print("OTA running");
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

void drawDHT(float h, float t, float f, float p)
{
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,0, String(mqtt_device));
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  #ifdef CELSIUS
    display.drawString(60,18, String(t) + " *C");
  #else
    display.drawString(60,18, String(f) + " *F");
  #endif
  display.setFont(ArialMT_Plain_10);
  #ifdef PRESS_ON
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0,40,"Prs: ");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(120,40, String(p) + "i");
  #endif
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,50,"Hum: ");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(120,50, String(h) + " %");
}

void loop() {
  #ifdef REMOTE && OTA_UPDATE
  ArduinoOTA.handle();
  #endif

  char strCh[10];
  String str;
  #ifdef REMOTE
    if (!client.loop()) {
      reconnect();
    }
  #endif

  if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;

      h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      f = dht.readTemperature(true);

      // Read pressure (if external pressure sensor connected)
      p = analogRead(PRESSPIN);

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        h=t=f=-1;
      }
      else { //add offsets, if any
        t = t + ((5 / 9) * temp_offset);
        f = f + temp_offset;
        h = h + hum_offset;
      }

      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);

      #ifdef REMOTE
        #ifdef CELSIUS
          str = String(t,2);
        #else
          str = String(f,2);
        #endif
        str.toCharArray(strCh,9);
        client.publish(temperaturepub, strCh);
        str = String(h,2);
        str.toCharArray(strCh,9);
        client.publish(humiditypub, strCh);
        #ifdef CELSIUS
          str = String(hic,2);
        #else
          str = String(hif,2);
        #endif
        str.toCharArray(strCh,9);
        client.publish(tempindexpub, strCh);
        client.loop();
        str = String(p,2);
        str.toCharArray(strCh,9);
        client.publish(presspub, strCh);
      #endif

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
      Serial.print(p);
      Serial.println(" in/H2O");

      display.invertDisplay();
      display.display();
      display.clear();
      display.normalDisplay();
      drawDHT(h,t,f,p);
      display.display();
  }
  currentMillis = millis();

#ifdef MOTION_ON
  motionState = digitalRead(MOTIONPIN);
//   print out the state of the button:
  if (motionState == 1)
{
    Serial.println("Motion Event:");
    #ifdef REMOTE
      client.publish(motionpub, "1");
      client.loop();
    #endif
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(120,0, "M");
    display.display();
    delay (4000);
    display.clear();
    drawDHT(h,t,f,p);
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
      client.subscribe(motionpub);
      client.subscribe(presspub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
