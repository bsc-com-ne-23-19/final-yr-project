#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define RXp2 16
#define TXp2 17

/***********Wifi connection******************/
const char* ssid = "El";
const char* password = "1R24GN5YH77";

/************Broker details******************/
const char* mqtt_server = "a1cce71a95ba420dace176e1e393bffc.s1.eu.hivemq.cloud";
const char* mqtt_username = "Chisale Chiwaya";
const char* mqtt_password = "1234567890";
const int mqtt_port = 8883;

/************Wifi connectivity***************/
WiFiClientSecure espClient;

/************MQTT Client using WiFi***********/
PubSubClient client(espClient);


unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];


/***************root certificate*****************/

static const char *root_ca PROGMEM = R"EOF(
  -----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----

)EOF";


/************************************************/

void setup_wifi(){
  delay(10);
  Serial.print("\nConnecting to :");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}


/******************Connect to Broker**************/
void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    String clientId = "Esp32-";
    clientId += String(random(0xffff), HEX);

    //Attempt to connect
    if(client.connect(clientId.c_str(),mqtt_username, mqtt_password)){
      Serial.println("connected");
      client.subscribe("mode");
      client.subscribe("status");
      client.subscribe("threshold_values");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print("Will try again after 5 seconds");
      delay(5000);
    }

  }
}


/****************Call back for receiving messages************* Just send the message to the arduino*/
void callback(char* topic, byte* payload, unsigned int length){
  String incomingMessage = "";
  for(int i = 0; i < length; i++){
    incomingMessage+=(char)payload[i];
  }

  Serial.println("Message arrived [" + String(topic) + "]" + incomingMessage);
  Serial2.println(incomingMessage);
}


  // Parse the JSON payload
  /*
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, incomingMessage);
  
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }
*/
  // const char* command;

// Check if the topic is about mode
/*
  if (strcmp(topic, "mode") == 0){
      // Extract device and command from the JSON
    if (doc.containsKey("fan")) {
      command = doc["fan"];
      Serial.print("Fan command: ");
      Serial.println(command);
    }
  
    if (doc.containsKey("heater")) {
      command = doc["heater"];
      Serial.print("Heater command: ");
      Serial.println(command);
    }

    if (doc.containsKey("light")) {
      command = doc["light"];
      Serial.print("Light command: ");
      Serial.println(command);
    }
  
    if (doc.containsKey("pump")) {
      command = doc["pump"];
      Serial.print("Pump command: ");
      Serial.println(command);
    }

    // Act on the command
    if (strcmp(command, "auto") == 0) {
      // Turn the device into auto 
      Serial.println("Turning the device into auto");
      // Send a command to arduino
    }
    
    else {
      // Turn device into manual
      Serial.println("Turning device into manual");
      // Send a command to arduino
    }    
  }
  
  // If the topic is about status
  else if (strcmp(topic, "status") == 0){
      // Extract device and command from the JSON
    if (doc.containsKey("fan")) {
      command = doc["fan"];
      Serial.print("Fan command: ");
      Serial.println(command);
    }
  
    if (doc.containsKey("heater")) {
      command = doc["heater"];
      Serial.print("Heater command: ");
      Serial.println(command);
    }

    if (doc.containsKey("light")) {
      command = doc["light"];
      Serial.print("Light command: ");
      Serial.println(command);
    }
  
    if (doc.containsKey("pump")) {
      command = doc["pump"];
      Serial.print("Pump command: ");
      Serial.println(command);
    }

    // Act on the command
    if (strcmp(command, "on") == 0) {
      // Turn the device on 
      Serial.println("Turning the device on");
      // Send a command to arduino
    }
    
    else {
      // Turn device off
      Serial.println("Turning device off");
      // Send a command to arduino
    }    
  }

  else{
    Serial.println("Invalid Topic.");
  }

*/ 


/********Publishing MQTT Messages************/
void publishMessage(const char* topic, String payload, boolean retained){
  if(client.publish(topic,payload.c_str(), true)){
    Serial.println("Message published [" + String(topic) + "]: " + payload);
    delay(100);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  while(!Serial){
    delay(1);
  }
  setup_wifi();
  espClient.setCACert(root_ca);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  read_from_arduino();
}

// Read from arduino
void read_from_arduino(){
  String incomingString;
  if (Serial2.available()) {  
    incomingString = Serial2.readStringUntil('\n');  
    Serial.println("Received from Arduino: " + incomingString);
    delay(100); 
  }

  // Parse the JSON data
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, incomingString);

    if (!error) {
      String dataType = doc["type"];

      // Determine the topic based on the data type
      String topic;
      if (dataType == "sensor_data") {
        topic = dataType;
        publishMessage("sensor_data", incomingString.c_str(),true);
      } else if (dataType == "actuator_data") {
        topic = dataType;
        publishMessage("actuator_data", incomingString.c_str(),true);
      } else if (dataType == "notification_data") {
        topic = dataType;
        publishMessage("notification_data", incomingString.c_str(),true);
      } else {
        Serial.println("Unknown data type");
        return;
      }
      
    }

    else {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    }
    delay(5000);  
}
