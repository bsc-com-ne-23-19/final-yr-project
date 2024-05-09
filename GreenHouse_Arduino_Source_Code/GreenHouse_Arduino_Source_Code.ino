//Library inclusions
// #include <SofwareSerial.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>

//Initializing LCD Library
LiquidCrystal lcd(10,9,5,4,3,2);

//GSM using SofwareSerial...Initiating Serial connection on the digital pins
// SoftwareSerial gsm(6,7);

//Global constants
#define ledPin 12
#define ldrPin A0
#define dhtPin A1
#define DHTTYPE DHT11
#define fanRelayPin 13
#define heaterRelayPin 7
#define moistureSensorPin A2

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
  // gsm.begin(9600); // Set baud rate since it's a serial connection
  lcd.begin(16,2); //Specify dimensions of lcd in begin()
  Serial.begin(9600);  // initialize serial communication at 9600 bits per second:
  dht.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(fanRelayPin, OUTPUT);
  pinMode(heaterRelayPin, OUTPUT);
  delay(1000);
}

void loop() 
{
  // read the input on analog pin 0:
  readLDR();
  readTemp();
  readMoisture();
  delay(2000);
}

//LDR reading value and control LED
void readLDR(){
  ldr_Value = analogRead(ldrPin);
  
  if(ldr_Value < 500)
  {
    digitalWrite(ledPin,HIGH);  // Turn ON LED
    Serial.println("LED ON!!!");
  }

  else{
    digitalWrite(ledPin,LOW);  // Turn OFF LED
    Serial.println("LED OFF!!!"); 
  }
  
}

//Read Temperature from DHT
void readTemp(){

  //Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check if readings are valid(Working sensors)

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
    lcd_display_invalid_dht();
  }
 
  else{
    //Print temperature
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("C");
    delay(500);

    //Print humidity
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");    
    delay(500);

    //Print both humidity and temperature on LCD
    lcdDisplay_Temp_Hum();

    //Activate or Deactivate Actuators
    tempControl();

  }
}

//Displaying on the lcd
void lcdDisplay_Temp_Hum(){
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
  delay(1000);
}

//Displaying on the lcd
void lcd_display_Moisture(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moisture: ");
  lcd.print(soil_moisture);//Print Humidity
  lcd.print("%");
  delay(1000);
}

//Faulty Sensor
void lcd_display_invalid_dht(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Faulty DHT Sensor");
}

// Controlling heater and fan using temperature
void tempControl(){
  //Controlling the fan

    // Right Range-----Switch fan and heater OFF
    if((temperature > temp_min) && (temperature < temp_max)){
      digitalWrite(fanRelayPin,LOW);
      Serial.println("FAN OFF!!");
      digitalWrite(heaterRelayPin,LOW);
      Serial.println("HEATER OFF!!");
    }
    
    // Temperature is high
    else if(temperature > temp_max){
      digitalWrite(fanRelayPin, HIGH);
      Serial.println("FAN ON!!");
      digitalWrite(heaterRelayPin,LOW);
      Serial.println("HEATER OFF!!");
    }

    // Temperature is Low
    else{
      digitalWrite(heaterRelayPin,HIGH);
      Serial.println("HEATER ON!!");
      digitalWrite(fanRelayPin, LOW);
      Serial.println("FAN OFF!!");
    }
}

// Soil Moisture
void readMoisture(){
  int raw_soil_moisture = analogRead(moistureSensorPin);
    // Prints Message on the LCD
  soil_moisture = map(raw_soil_moisture,0,1023,0,100);
  lcd_display_Moisture();
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