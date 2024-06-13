//Library inclusions
#include <ArduinoJson.h>  // Include the ArduinoJson library
#include <SoftwareSerial.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>
#include <String.h>
#include <Ticker.h>

//Initializing LCD Library
LiquidCrystal lcd(10,9,5,4,3,2);

//GSM using SofwareSerial...Initiating Serial connection on the digital pins
SoftwareSerial esp(6,8);

//Global constants
#define ledPin 12
#define ldrPin A0
#define dhtPin A1
#define DHTTYPE DHT11
#define fanRelayPin 13
#define heaterRelayPin 7
#define moistureSensorPin A2
#define waterPumpRelayPin 11

//Global variables
// Ticker timer;
String gsm_read_string = "";
String alert_message = "";
float temperature;
float humidity;
float light;
float moisture;
float temperature_threshold;
float humidity_threshold;
float light_threshold;
float moisture_threshold;
int ldr_Value;
float temp_min;
float temp_max;
float temp_low_critical = temp_min - (temp_min * 0.25);
float temp_high_critical = temp_max + (temp_max * 0.25);
float hum_min;
float hum_max;
float light_min;
float light_max;
float light_low_critical = light_min - (light_min * 0.25);
float light_high_critical = light_max + (light_max * 0.25);
float moisture_min;
float moisture_max;
float moisture_low_critical = moisture_min - (moisture_min * 0.25);
float moisture_high_critical = moisture_max + (moisture_max * 0.25);
int dry_raw_moisture = 1023;
int wet_raw_moisture = 635;

int raw_no_light = 0;
// int raw_yes_light = 210;
int raw_yes_light = 1023;

// false = off
boolean fan_state_manual = false;
boolean heater_state_manual = false;
boolean pump_state_manual = false;
boolean light_state_manual = false;

// false means it is manual
boolean fan_mode = false;
boolean heater_mode = false;
boolean pump_mode = false;
boolean light_mode = false;

//DHT variable
DHT dht(dhtPin,DHTTYPE);

void setup() 
{
  esp.begin(9600); // Set baud rate since it's a serial connection
  // lcd.begin(16,2); //Specify dimensions of lcd in begin()
  Serial.begin(9600);  // initialize serial communication at 9600 bits per second:
  dht.begin();  
  pinMode(ledPin, OUTPUT);
  pinMode(fanRelayPin, OUTPUT);
  pinMode(heaterRelayPin, OUTPUT);
  pinMode(waterPumpRelayPin, OUTPUT);
}



/************************************Loop Section*******************************/

void loop() 
{
  // read the input on analog pin 0:
  // Automatic reading and controlling
  readTemp();
  readMoisture();
  readLDR();
  actuatorToJSON();
  sensorDataToJSON();
  read_from_esp();

  // Manual prompts
  // read_from_gsm();
  // Check from the arduino side if the gsm has data to give it by reading the data byte by byte  
}

/********************************SENSOR READING SECTION******************************/

//Read Temperature from DHT
void readTemp(){

  //Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check if readings are valid(Working sensors)

  if (isnan(temperature) || isnan(humidity)) {
    // lcd_display_invalid_dht();
    Serial.println("Null Reading");
    temperature = 0;
    humidity = 0;
  }
 
  else{

    //Print both humidity and temperature on LCD
    // lcd_display_temp_hum();

    //Activate or Deactivate Actuators
    temp_control();
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    delay(5000);

  }
}

//LDR reading value and control LED
void readLDR(){
  int raw_ldr_Value = analogRead(ldrPin);
  light = map(raw_ldr_Value,raw_no_light,raw_yes_light,0,100);
  Serial.println(String("Light: ") + light + String("%"));
  delay(5000);
  // lcd_display_light();

  // Control Light
  
  light_control();
  
     
}

// Soil Moisture
void readMoisture(){
  int raw_moisture = analogRead(moistureSensorPin);
    // Prints Message on the LCD
  moisture = map(raw_moisture,dry_raw_moisture,wet_raw_moisture,0,100);
  // moisture = 100 - moisture;

  // Monitor
  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println("%");
  delay(5000);
  // lcd_display_moisture();

  // Control moisture  
  control_moisture();  
}


/******************************************END OF SENSOR READING SECTION*********************************************/


/************************************************CONTROL SECTION****************************************************/

// Controlling heater and fan using temperature
void temp_control(){
  //Controlling the fan

    // Right Range-----Switch fan and heater OFF
    if((temperature > temp_min) && (temperature < temp_max)){

      // Alert for normal temperature
      send_normal_alert_temp();

      // check if fan is manual
      if(fan_state_manual == true){

        // Print fan is in manual mode
        lcd.clear();
        lcd.println("Fan Manual");
        delay(300);

        // check if heater is manual
        if(heater_state_manual == true){
          lcd.clear();
          lcd.print("Heater Manual");
          delay(300);
        }

        //if it is auto
        else{
          heater_off();
        }
      }

      // Fan is auto
      else if(fan_state_manual == false){

        if(heater_state_manual == true){
          lcd.clear();
          lcd.print("Heater Manual");
          delay(300);
        }

        //if it is auto
        else{
          heater_off();
        }

        fan_off();
      }

      
      // check if heater is manual
      else if(heater_state_manual == true){

        // Print status of heater
        lcd.clear();
        lcd.println("Heater Manual");
        // check if fan is manual
        if(fan_state_manual == true){
          lcd.clear();
          lcd.println("Fan Manual");
          delay(300);
        }

        //if it is auto
        else{
          fan_off();
        }
      }

      // Heater is auto
      else{

        if(fan_state_manual == true){
          lcd.clear();
          lcd.print("Fan Manual");
          delay(300);
        }

        //if it is auto
        else{
          fan_off();
        }
        
        heater_off();
      }
      
    }

    // For a critical alert
    else if(temperature <= temp_low_critical){
      send_low_critical_alert_temp();
    }

    else if(temperature >= temp_high_critical){
      send_high_critical_alert_temp();
    }
    
    // Temperature is high
    else if(temperature > temp_max){

      // Alert HIGH
      send_high_alert_temp();

      // check if fan is manual
      if(fan_state_manual == true){

        // Print fan is in manual mode
        lcd.clear();
        lcd.println("Fan Manual");
        delay(300);

        // check if heater is manual
        if(heater_state_manual == true){
          lcd.clear();
          lcd.print("Heater Manual");
          delay(300);
        }

        //if it is auto
        else{
          heater_off();
        }
      }

      // Fan is auto
      else{

        if(heater_state_manual == true){
          lcd.clear();
          lcd.print("Heater Manual");
          delay(300);
        }

        //if it is auto
        else{
          heater_off();
        }

        fan_on();
      }

    }

    // Temperature is Low
    else{

      // Alert LOW
      send_low_alert_temp();

      // check if heater is manual
      if(heater_state_manual == true){

        // Print status of heater
        lcd.clear();
        lcd.println("Heater Manual");

        // check if fan is manual
        if(fan_state_manual == true){
          lcd.clear();
          lcd.println("Fan Manual");
          delay(300);
        }

        //fan auto
        else{
          fan_off();
        }
      }

      // Heater is auto
      else{

        // fan manual
        if(fan_state_manual == true){
          lcd.clear();
          lcd.print("Fan Manual");
          delay(300);
        }

        //fan auto
        else{
          fan_off();
        }
        
        heater_on();
      }
      
    }
}


// Light Control
void light_control(){
  
  // Normal Light
  if((ldr_Value > light_min) && (ldr_Value < light_max)){

    // Alert Normal
    send_normal_alert_light();

    // Light is in Manual mode
    if(light_state_manual == true){
      lcd.clear();
      lcd.print("Light Manual");
      delay(300);
    }
    
    // Light in Auto mode
    else{
      light_off();
    }
    
  }

  // For a critical alert
  else if(light <= light_low_critical){
    send_low_critical_alert_light();
  }

  // Low Light
  else if(ldr_Value < light_min){

    // Alert Low
    send_low_alert_light();

    // Light is in Manual mode
    if(light_state_manual == true){
      lcd.clear();
      lcd.print("Light Manual");
      delay(300);
    }
    
    // Light in Auto mode
    else{
      light_on();
    }
  }

  // High Light
  else{

    // Alert High
    send_high_alert_light();

    // Light is in Manual mode
    if(light_state_manual == true){
      lcd.clear();
      lcd.print("Light Manual");
      delay(300);
    }
    
    // Light in Auto mode
    else{
      light_off();
    }
  }
}

// Water Control
void control_moisture(){

  // Stop Watering
  if(moisture > moisture_max){

    // Alert High
    send_high_alert_moisture();

    // Pump in manual
    if(pump_state_manual == true){
      lcd.clear();
      lcd.print("Pump Manual");
      delay(300);
    }

    // Pump in Auto
    else{
      pump_off();
    }
    
  }

  // For a critical alert
  else if(moisture <= moisture_low_critical){
    send_low_critical_alert_moisture();
  }

  else if(moisture >= moisture_high_critical){
    send_high_critical_alert_moisture();
  }

  // Start Watering
  else if(moisture < moisture_min){

    // Alert Low
    send_low_alert_moisture();

    // Pump in manual
    if(pump_state_manual == true){
      lcd.clear();
      lcd.print("Pump Manual");
      delay(300);
    }

    // Pump in Auto
    else{
      pump_on();
    }
  }

  //Good level dont water 
  else{

    // Alert Normal
    send_normal_alert_moisture();

    // Pump in manual
    if(pump_state_manual == true){
      lcd.clear();
      lcd.print("Pump Manual");
      delay(300);
    }

    // Pump in Auto
    else{
      pump_off();
    }
  }

}

/*************************************END OF CONTROL SECTION*******************************************/

/**************************************ACTUATOR SECTION************************************************/

// Fan on fuction
void fan_on(){
  digitalWrite(fanRelayPin, HIGH);
  lcd_display_fan_on();
  delay(100);
}

// Fan off function
void fan_off(){
  digitalWrite(fanRelayPin,LOW);
  lcd_display_fan_off(); 
  delay(100);
}

// Heater on fuction
void heater_on(){
  digitalWrite(heaterRelayPin,HIGH);
  lcd_display_heater_on();
  delay(100);
}

// Heater off function
void heater_off(){
  digitalWrite(heaterRelayPin,LOW);
  lcd_display_heater_off();
  delay(100);
}

// Pump on fuction
void pump_on(){
  digitalWrite(waterPumpRelayPin, HIGH);
  lcd_display_pump_on();
  delay(100);
}

// Pump off function
void pump_off(){
  digitalWrite(waterPumpRelayPin, LOW);
  lcd_display_pump_off(); 
  delay(100); 
}

// Bulb on fuction
void light_on(){
  digitalWrite(ledPin,HIGH);
  lcd_display_light_on();
  delay(100);
}

// Bulb off function
void light_off(){
    digitalWrite(ledPin,LOW);
    lcd_display_light_off();
    delay(100);
}

/*******************************END OF ACTUATOR SECTION***********************************************/


/********************************************Sending Alerts*******************************************/

// Normal Temperature
void send_normal_alert_temp(){
  alert_message = "Temperature is Normal";
  send_message(alert_message);
}

// High Temperature
void send_high_alert_temp(){
  alert_message = "Temperature is High";
  send_message(alert_message);
}

// Low Temperature
void send_low_alert_temp(){
  alert_message = "Temperature is Low";
  send_message(alert_message);
}

// Critical Temperature
void send_high_critical_alert_temp(){
  alert_message = "Temperature is critical. Check the fan";
  send_message(alert_message);
}

void send_low_critical_alert_temp(){
  alert_message = "Temperature is critical. Check the heater.";
  send_message(alert_message);
}

// High Humidity
void send_high_alert_hum(){
  alert_message = "Humidity is High";
  send_message(alert_message);
}

// Low Humidity
void send_low_alert_hum(){
  alert_message = "Humidity is Low";
  send_message(alert_message);
}

// Normal Humidity
void send_normal_alert_hum(){
  alert_message = "Humidity is Normal";
  send_message(alert_message);
}

// Low Light
void send_low_alert_light(){
  alert_message = "Light is Low";
  send_message(alert_message);
}

void send_low_critical_alert_light(){
  alert_message = "Light is critical. Check the light source.";
  send_message(alert_message);
}

// High Light
void send_high_alert_light(){
  alert_message = "Light is High";
  send_message(alert_message);
}

// Normal Light
void send_normal_alert_light(){
  alert_message = "Light is Normal";
  send_message(alert_message);
}

// Low Moisture
void send_low_alert_moisture(){
  alert_message = "Moisture is Low";
  send_message(alert_message);
}

// High Moisture
void send_high_alert_moisture(){
  alert_message = "Moisture is High";
  send_message(alert_message);
}

// Normal Moisture
void send_normal_alert_moisture(){
  alert_message = "Moisture is Normal";
  send_message(alert_message);
}

// Critical Temperature
void send_high_critical_alert_moisture(){
  alert_message = "Moisture is critical. Check the pump";
  send_message(alert_message);
}

void send_low_critical_alert_moisture(){
  alert_message = "Moisture is critical. Check the pump.";
  send_message(alert_message);
}

/*********************************************END OF ALERT SECTION*****************************************/


/*************************************************LCD SECTION*******************************************************/

//Temperature and Humidity Readings
void lcd_display_temp_hum(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(humidity);//Print Humidity
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperature);//Print Temperature
  lcd.setCursor(10,1);//Move to the position
  lcd.print( (char)223);//the degree symbol
  lcd.print("C");
  delay(300);
}

//Moisture Readings
void lcd_display_moisture(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moisture: ");
  lcd.print(moisture);//Print Humidity
  lcd.print("%");
  delay(300);
}

//Light Readings
void lcd_display_light(){
  lcd.setCursor(0,1);
  lcd.print("Light: ");
  lcd.print(ldr_Value);//Print Humidity
  lcd.print("%");
  delay(300);
}

//Faulty Sensor Message
void lcd_display_invalid_dht(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Faulty DHT Sensor");
  delay(300);
}

//Fan On Message
void lcd_display_fan_on(){
  lcd.clear();
  lcd.print("Fan On");
  delay(300);
}

//Fan Off Message
void lcd_display_fan_off(){
  lcd.clear();
  lcd.print("Fan Off");
  delay(300);
}

//Heater On Message
void lcd_display_heater_on(){
  lcd.clear();
  lcd.print("Heater On");
  delay(300);
}

//Heater Off Message
void lcd_display_heater_off(){
  lcd.clear();
  lcd.print("Heater Off");
  delay(300);
}

//Pump On Message
void lcd_display_pump_on(){
  lcd.clear();
  lcd.print("Pump On");
  delay(300);
}

//Pump Off Message
void lcd_display_pump_off(){
  lcd.clear();
  lcd.print("Pump Off");
  delay(300);
}

//Light On Message
void lcd_display_light_on(){
  lcd.clear();
  lcd.print("Light On");
  delay(300);

}

//Light Off Message
void lcd_display_light_off(){
  lcd.clear();
  lcd.print("Light Off");
  delay(300);
}


/**********************************END OF LCD SECTION******************************************/

/*****************************************Communication ESP8266 WiFi************************************/
String incomingData = "";
void read_from_esp(){ 
  if (esp.available() > 0) {
      incomingData = esp.readStringUntil('\n');
      Serial.println(incomingData);
    }
    parse_command(incomingData);
    delay(100);
}

void parse_command(String command){


    if (true) {
      Serial.println("Received event:");
      Serial.println(command);

              // Parse the JSON data using ArduinoJson
      DynamicJsonDocument doc(1024); // Adjust buffer size as needed
      DeserializationError error = deserializeJson(doc, command);
    
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
      }

      else {     
        if (doc.containsKey("fan")) {
              // Key exists, access its value
          String command = doc["fan"];
          // Act based on the command
          if (command.equals("on")){
            fan_on();             
          }

          else if (command.equals("auto")){
            fan_mode = true;
          }

          else if (command.equals("manual")){
            fan_mode = false;
          }

          else if (command.equals("off")){
            fan_off();
          }

          else{
            Serial.println("Invalid command.");
          }
                
        }

          else if (doc.containsKey("heater"))
          {
                
            String command = doc["heater"];
            if (command.equals("on")){
              heater_on();
            }

            else if (command.equals("auto")){
            heater_mode = true;
            }

            else if (command.equals("manual")){
              heater_mode = false;
            }

            else if (command.equals("off")){
              heater_off();
            }

            else{
              Serial.println("Invalid command.");
            }
          }

          else if (doc.containsKey("pump"))
          {
                
            String command = doc["pump"];
            if (command.equals("on")){
              pump_on();
            }

            else if (command.equals("auto")){
              pump_mode = true;
            }

            else if (command.equals("manual")){
              pump_mode = false;
            }

            else if (command.equals("off")){
              pump_off();
            }

            else{
              Serial.println("Invalid command.");
            }
          }


          else if (doc.containsKey("light"))
          {
                
            String command = doc["light"];
            if (command.equals("on")){
              light_on();
            }

            else if (command.equals("auto")){
              light_mode = true;
            }

            else if (command.equals("manual")){
              light_mode = false;
            }

            else if (command.equals("off")){
              light_off();
            }

            else{
              Serial.println("Invalid command.");
            }
          }

          // Set threshold values
          else if (doc.containsKey("temperature_threshold")) {
            // Change value to integer
            temperature_threshold = doc["temperature_threshold"].as<float>();
            humidity_threshold = doc["humidity_threshold"].as<float>();
            moisture_threshold = doc["moisture_threshold"].as<float>();
            light_threshold = doc["light_threshold"].as<float>();
            Serial.println("temp_T" + String(temperature_threshold));
            Serial.println("hum_T" + String(humidity_threshold));
            Serial.println("moisture_T" + String(moisture_threshold));
            Serial.println("light_T" + String(light_threshold));
          }        

          else 
          {
            Serial.println("Invalid command.");    
          }
      }
    }  
}


              
/*******************************************Convert to JSON***********************************/
void sensorDataToJSON(){
  DynamicJsonDocument doc(512);
  // Add sensor data to the document
  doc["type"] = "sensor_data";
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["light"] = light;
  doc["moisture"] = moisture;
  // Serialize the JSON document into a string
  String jsonString;
  serializeJson(doc, jsonString);

  esp.println(jsonString);
}

void actuatorToJSON(){
  DynamicJsonDocument doc(512);
  // Add sensor data to the document
  doc["type"] = "actuator_data";

  if(fan_state_manual == true){
    doc["fan_state"] = "on";
  }

  if(fan_state_manual == false){
    doc["fan_state"] = "off";
  }

  if(fan_mode == true){
    doc["fan_mode"] = "auto";
  }

  if(fan_mode == false){
    doc["fan_mode"] = "manual";    
  }

  if(heater_state_manual == true){
    doc["heater_state"] = "on";  
  }

  if(heater_state_manual == false){
    doc["heater_state"] = "off";
  }

  if(heater_mode == true){
    doc["heater_mode"] = "auto";
  }

  if(heater_mode == false){
    doc["heater_mode"] = "manual";
  }

  if(pump_state_manual == true){
    doc["pump_state"] = "on";
  }

  if(pump_state_manual == false){
    doc["pump_state"] = "off";
  }

  if(pump_mode == true){
    doc["pump_mode"] = "auto";
  }

  if(pump_mode == false){
    doc["pump_mode"] = "manual";
  }

  if(light_state_manual == true){
    doc["light_state"] = "on";
  }

  if(light_state_manual == false){
    doc["light_state"] = "off";
  }

  if(light_mode == true){
    doc["light_mode"] = "auto";
  }

  if(light_mode == false){
    doc["light_mode"] = "manual";
  }

  // Serialize the JSON document into a string
  String jsonString;
  serializeJson(doc, jsonString);

  esp.println(jsonString);
}

void send_message(String msg){
  DynamicJsonDocument doc(512);
  // Add sensor data to the document
  doc["type"] = "notification_data";
  doc["msg"] = msg;
  // Serialize the JSON document into a string
  String jsonString;
  serializeJson(doc, jsonString);

  esp.println(jsonString);
}