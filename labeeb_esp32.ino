
#include <WiFi.h>
#include <Wire.h>
#include <LabeebIoT.h>
#include <NTPClient.h>
#include <DNSServer.h> 

long lastMsg = 0;

const char* DT_USER = "labeeb_device_template_user_name";
const char* DT_PASS = "PASSWORD";
const char* DEVICE_NAME = "PASSWORD";



const int MOTION_PIN = 1;
int motionState = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov", 0, 20000);

// -- MQTT settings -->
const char* mqtt_server = "mea.labeeb-iot.com";
WiFiClient espClient;
LabeebIoT labeeb (espClient,mqtt_server,1883,"labeeb");
// <----


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}



void reconnect() {
  // Loop until we're reconnected
  while (!labeeb.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (labeeb.connect("client_id13123",DT_USER, DT_PASS)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
     // client.subscribe("/qmic_intern/devicetemplate/test_device/dataCSV");
   //   Serial.println("Subscribed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(labeeb.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {

Serial.begin(9600);
Serial.println("Labeeb IoT Snesor");

pinMode(MOTION_PIN, INPUT_PULLUP);
pinMode(A0, INPUT);

setup_wifi();


delay (60000);

}




void loop() {

 labeeb.loop();

delay(1000);
if (!labeeb.connected()) {
         Serial.println("reconnecting");

   reconnect();


   }else {

Serial.println (digitalRead(MOTION_PIN));


long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;



      timeClient.update();

      int currentRead = digitalRead(MOTION_PIN);
       if(currentRead != motionState) {
         motionState = currentRead;


String msgg = String(timeClient.getEpochTime())+",,presence,motion:"+String(motionState);
puchToMqtt("MQTT/TOPIC" ,msgg);

Serial.println (msgg);

       }

  }


 }


}

bool puchToMqtt(String toTopic,String tomsg )
{

Serial.println("topic :"+ toTopic);

        int str_len = tomsg.length() + 1;
        int tpc_len = toTopic.length() + 1;
        char msg[str_len];
        char topic[tpc_len];
        tomsg.toCharArray(msg, str_len);
        toTopic.toCharArray(topic, tpc_len);
        strcpy (msg, tomsg.c_str());
        strcpy (topic, toTopic.c_str());
        labeeb.publish(topic, msg);


}
