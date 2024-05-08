//#include <DHT.h>
//#include <DHT_U.h>

//Library inclusions
#include <dht.h>
#include <LiquidCrystal.h>

//Initializing LCD Library
LiquidCrystal lcd(10,9,5,4,3,2);

//Global variables
#define ledPin 12
#define ldrPin A0
#define dhtPin 11
#define relayPin 13

//DHT variable
dht DHT;

void setup() 
{
  
  lcd.begin(16,2); //Specify dimensions of lcd in begin()
  Serial.begin(9600);  // initialize serial communication at 9600 bits per second:
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  delay(1000);
}

void loop() 
{
  // read the input on analog pin 0:
  readLDR();
  readTemp();
  delay(3000);
}

//LDR reading value and control LED
void readLDR(){
  int LDR_Value = analogRead(ldrPin);
  
  if(LDR_Value < 500)
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
 // float temp = dht.readTemperature();
  //float hum = dht.readHumidity();
  int readings = DHT.read11(dhtPin);

  //Print temperature
  Serial.print("Temperature: ");
  Serial.print(DHT.temperature);
  Serial.println("C");
  delay(500);

  //Print humidity
  Serial.print("Humidity: ");
  Serial.print(DHT.humidity);
  Serial.println("%");    
  delay(500);

  //Print both humidity and temperature on LCD
  lcdDisplay_Temp_Hum();

  //Controlling the fan
  if(DHT.temperature > 28){
    digitalWrite(relayPin, HIGH);
    Serial.println("FAN ON!!");
  }
  else{
    digitalWrite(relayPin, LOW);
    Serial.println("FAN OFF!!");
  }
}

//Displaying on the lcd
void lcdDisplay_Temp_Hum(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);//Print Humidity
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);//Print Temperature
  lcd.setCursor(10,1);//Move to the position
  lcd.print( (char)223);//the degree symbol
  lcd.print("C");
}
