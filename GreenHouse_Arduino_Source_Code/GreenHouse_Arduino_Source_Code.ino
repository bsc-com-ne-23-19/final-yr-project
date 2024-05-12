//Library inclusions
#include <SoftwareSerial.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>

//Initializing LCD Library
LiquidCrystal lcd(10,9,5,4,3,2);

//GSM using SofwareSerial...Initiating Serial connection on the digital pins
SoftwareSerial GPRS(6,7);

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
float temperature;
float humidity;
int ldr_Value;
int temp_min = 20;
int temp_max = 30;
float soil_moisture;

//DHT variable
DHT dht(dhtPin,DHTTYPE);

void setup() 
{
  GPRS.begin(9600); // Set baud rate since it's a serial connection
  lcd.begin(16,2); //Specify dimensions of lcd in begin()
  Serial.begin(9600);  // initialize serial communication at 9600 bits per second:
  dht.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(fanRelayPin, OUTPUT);
  pinMode(heaterRelayPin, OUTPUT);
  pinMode(waterPumpRelayPin, OUTPUT);


  // Delete messages on the sim card on the gsm
  for (int i = 1; i <= 15; i++) {
    Serial.print("AT+CMGD=");
    Serial.println(i);
    delay(200);

    // Not really necessary but prevents the serial monitor from dropping any input
    // while(GPRS.available()) 
      // Serial.write(GPRS.read());
  }

  // Set GSM in receiving mode
  receive_message();

  // 
  lcd.clear();
  lcd.println("Finished Startup");
  delay(1000);
}

void loop() 
{
  // read the input on analog pin 0:
  // Automatic reading and controlling
  readTemp();
  readMoisture();
  readLDR();

  // Manual prompts
  read_from_gsm();
  // Check from the arduino side if the gsm has data to give it by reading the data byte by byte
  

  delay(2000);
}

/********************************SENSOR READING SECTION******************************/

//Read Temperature from DHT
void readTemp(){

  //Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check if readings are valid(Working sensors)

  if (isnan(temperature) || isnan(humidity)) {
    lcd_display_invalid_dht();
  }
 
  else{

    //Print both humidity and temperature on LCD
    lcd_display_temp_hum();

    //Activate or Deactivate Actuators
    // temp_control();

  }
}

//LDR reading value and control LED
void readLDR(){
  int raw_ldr_Value = analogRead(ldrPin);
  ldr_Value = map(raw_ldr_Value,0,1023,0,100);
  lcd_display_light();
  // light_control();  
}

// Soil Moisture
void readMoisture(){
  int raw_soil_moisture = analogRead(moistureSensorPin);
    // Prints Message on the LCD
  soil_moisture = map(raw_soil_moisture,0,1023,0,100);
  lcd_display_moisture();
  // controlMoisture();
}


/******************************************END OF SENSOR READING SECTION*********************************************/


/***********************************CONTROL SECTION********************************************/

// Controlling heater and fan using temperature
void temp_control(){
  //Controlling the fan

    // Right Range-----Switch fan and heater OFF
    if((temperature > temp_min) && (temperature < temp_max)){
      fan_off();
      heater_off();
    }
    
    // Temperature is high
    else if(temperature > temp_max){
      fan_on();
      // Find out if heater is on or off, then act appropriately
      heater_off();
    }

    // Temperature is Low
    else{
      heater_on();
      fan_off();
    }
}

// Light Control
void light_control(){
  
  if((ldr_Value > 50) && (ldr_Value < 80)){
    light_off();
  }
  else if(ldr_Value < 50){
    light_on();
  }

  else{
    light_off(); 
  }
}

// Water Control
void controlMoisture(){
  // Stop Watering
  if(soil_moisture > 65){
    pump_off();
  }

  // Start Watering
  else if(soil_moisture < 50){
    pump_on();
  }

  //Good level dont water 
  else{
   pump_off();
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
  delay(100);
}

//Moisture Readings
void lcd_display_moisture(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moisture: ");
  lcd.print(soil_moisture);//Print Humidity
  lcd.print("%");
  delay(100);
}

//Light Readings
void lcd_display_light(){
  lcd.setCursor(0,1);
  lcd.print("Light: ");
  lcd.print(ldr_Value);//Print Humidity
  lcd.print("%");
  delay(100);
}

//Faulty Sensor Message
void lcd_display_invalid_dht(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Faulty DHT Sensor");
  delay(100);
}

//Fan On Message
void lcd_display_fan_on(){
  lcd.clear();
  lcd.print("Fan On");
  delay(100);
}

//Fan Off Message
void lcd_display_fan_off(){
  lcd.clear();
  lcd.print("Fan Off");
  delay(100);
}

//Heater On Message
void lcd_display_heater_on(){
  lcd.clear();
  lcd.print("Heater On");
  delay(100);
}

//Heater Off Message
void lcd_display_heater_off(){
  lcd.clear();
  lcd.print("Heater Off");
  delay(100);

}

//Pump On Message
void lcd_display_pump_on(){
  lcd.clear();
  lcd.print("Pump On");
  delay(100);
}

//Pump Off Message
void lcd_display_pump_off(){
  lcd.clear();
  lcd.print("Pump Off");
  delay(100);
}

//Light On Message
void lcd_display_light_on(){
  lcd.clear();
  lcd.print("Light On");
  delay(100);

}

//Light Off Message
void lcd_display_light_off(){
  lcd.clear();
  lcd.print("Light Off");
  delay(100);
}


/**********************************END OF LCD SECTION******************************************/


/**********************************COMMUNICATION SECTION***************************************/
/*The first part is for the user controlling actuators or sending commands to the arduino for whatever task*/

String gsm_read_string = "";
void receive_message(){
  Serial.println("AT+CMGF=1");
  Serial.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
}

void read_from_gsm(){
  // If there is something to be read, read it.
  while (Serial.available() > 0)
      {
        // Read the whole string from gsm
        gsm_read_string= Serial.readString();

        // The string will have a carriage return character at the end as a delimeter, remove it.
        gsm_read_string.remove(gsm_read_string.length()-1);

        // Analyse the message

        // Turn fan on
        if(gsm_read_string == "FAN_ON")
        {
          fan_on();
        }

        // Turn fan off
        else if(gsm_read_string == "FAN_OFF")
        {
          fan_off();
        }

        // Turn heater on
        else if(gsm_read_string == "HEATER_ON")
        {
          heater_on();
        }

        // Turn heater off
        else if(gsm_read_string == "HEATER_OFF")
        {
          heater_off();
        }

        // Turn Pump on
        else if(gsm_read_string == "PUMP_ON")
        {
          pump_on();
        }

        // Turn Pump off
        else if(gsm_read_string == "PUMP_OFF")
        {
          pump_off();
        }

        // Turn Light on
        else if(gsm_read_string == "LIGHT_ON")
        {
          light_on();
        }

        // Turn Light off
        else if(gsm_read_string == "LIGHT_OFF")
        {
          light_off();
        }

        else
        {
          lcd.clear();
          lcd.print("WRONG COMMAND");
          delay(100);
          // digitalWrite(buzzer, HIGH); 
          // delay(500);
          // digitalWrite(buzzer, LOW); 
          delay(100);
        }
      }
}

/*

This Code Block begins gsm communication. Needs more research and building.


void gsm_communication(){
  // Check if there is anything from the Serial, and initiate gsm
  if(Serial.available() > 0){
      switch(Serial.read()){
        case 's' :
          SendMessage();
          break;

        case 'r' : 
          ReceiveMessage();
          break;
      }
  }

  // Check if there is any data from the gsm and write it to the serial
  if(gsm.available() > 0){
    Serial.write(gsm.read());
  }
}

void SendMessage(){
  //Set gsm module in Send SMS mode -- I want to send SMS
  gsm.println("AT+CMGF=1");
  delay(1000);
  // Punch in phone number -- To Who?
  gsm.println("AT+CMGS=\"+265\"\r");
  delay(1000);
  //Write the text
  gsm.println("Learning happens in iterations.");
  delay(100);
  //Exit
  gsm.println((char) 26);
}

void RecieveMessage(){
  gsm.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
}
*/