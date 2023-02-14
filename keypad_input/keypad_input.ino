#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>

// co2
#include <SoftwareSerial.h>
#include <MHZ.h>

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 11, 9, 8}; 
byte colPins[COLS] = {7, 6, 4, 3}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Dht variables
#define DHTTYPE DHT22
#define DHTPIN 13
DHT dht(DHTPIN, DHTTYPE);

// temperature values
int minTempTarget = 18;
int maxTempTarget = 24;

// humidity values
int minHumidityTarget = 80;
int maxHumidityTarget = 95;

// co2 values
int minCo2Target = 600;
int maxCo2Target = 1000;

// relay pins
int lightRelayPin = 1;
int fanRelayPin = 2;

// fan state
bool isFanBlowing;

// co2 variables
// pin for pwm reading
#define CO2_IN 5

// pin for uart reading
#define MH_Z19_RX 0  // D7
#define MH_Z19_TX 1  // D6

int ppm_pwm;

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

// timer variables
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;



// temperature
float temperature;

// numPad
char customKey;

void setup(){
  isFanBlowing = true;
  startMillis = millis();
  LcdSetup();
  //pinMode(dhtPin, INPUT);
  dht.begin();
  Serial.begin(9600);

  // relay setup
  pinMode(lightRelayPin, OUTPUT);
  pinMode(fanRelayPin, OUTPUT);

  // co2 setup
  pinMode(CO2_IN, INPUT);
/*
  if (co2.isPreHeating()) {
    Serial.print("Preheating");
    while (co2.isPreHeating()) {
      LcdSetup();
      Serial.print(".");
      delay(5000);
      if(!co2.isPreHeating())
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ready - press A");
        Serial.println("ready");
        break;
      }
    }
  }
  */
}
  
void loop(){

  // Start light once
  digitalWrite(lightRelayPin, HIGH);

  Serial.println("entering loop");
  TemperatureControl();

  Input();
  
  lcd.clear();
  lcd.setCursor(0, 0);

  // temp = analogRead(dhtPin); 

} 

void Input()
{
  customKey = customKeypad.getKey();

  if (customKey){
    Serial.println("got an input");

    // currentMillis = currentMillis - currentMillis;
    lcd.setCursor(1, 0);
    lcd.clear();
    lcd.print("pressed: ");
    lcd.setCursor(10, 0);
    lcd.print(customKey);
    delay(2000);
    lcd.setCursor(1, 1);
    lcd.print("loading.");
    delay(2000);
    lcd.clear();
    lcd.print("loading..");
    delay(2000);
    lcd.clear();
    lcd.print("loading...");
    delay(2000);

  }
  
  if(customKey == 'A')
  {

    Serial.println("reading temperature");
    // currentMillis = currentMillis - currentMillis;
    temperature = dht.readTemperature();
   
    lcd.clear();
    lcd.print("temperature");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.setCursor(5, 1);
    lcd.print(" *C");
    delay(5000);
    // reset Key
    customKey == NULL;
  }

  if(customKey == 'B')
  {
    Serial.println("wind control");
    if(isFanBlowing)
    {
      Serial.println("fans off");
      digitalWrite(fanRelayPin, LOW);
      isFanBlowing = false;
      delay(1000);
      // reset Key
      customKey == NULL;
    }
    else if(!isFanBlowing)
    {
      Serial.println("fans off");
      digitalWrite(fanRelayPin, HIGH);
      isFanBlowing = true;
      delay(1000);
      // reset Key
      customKey == NULL;
    }
  }

  /*

  if(customKey == 'C')
  {

    Serial.println("reading co2");
    ppm_pwm = co2.readCO2PWM();
    Serial.println("got co2 data!");

    // currentMillis = currentMillis - currentMillis;
    lcd.clear();
    lcd.print("co2");
    lcd.setCursor(0, 1);
    lcd.print(ppm_pwm);
    lcd.setCursor(5, 1);
    lcd.print(" *pwm");
    delay(5000);
  }
  */
  else
    MainMenu();
    
  Serial.println("going in main menu");

}


void MainMenu()
{

  // Welcome
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Shroomies!");

  // break:
  HumidityControl();
  TemperatureControl();
  if(customKey)
    return;

  // Button A - temperature
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button A for");
  lcd.setCursor(0, 1);
  lcd.print("temperature");
  delay(2000);

  // break:
  HumidityControl();
  TemperatureControl();
  if(customKey)
    return;

  // Button B - wind
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button B for");
  lcd.setCursor(0, 1);
  lcd.print("wind control");
  delay(2000);

  // break:
  TemperatureControl();
  HumidityControl();
  if(customKey)
    return;

  // Button C - Co2
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button C for");
  lcd.setCursor(0, 1);
  lcd.print("Co2 control");
  delay(2000);
}

/*
void MainMenu()
{
  currentMillis = millis();
  // Welcome
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Shroomies!");
  // Button A - temperature
  if(currentMillis - startMillis >= period)
  {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button A for");
  lcd.setCursor(0, 1);
  lcd.print("temperature");
  currentMillis = currentMillis - currentMillis;
  }
  // Button B - wind
  if(currentMillis - startMillis >= period)
  {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button B for");
  lcd.setCursor(0, 1);
  lcd.print("wind control");
  currentMillis = currentMillis - currentMillis;
  }
  // Button C - Co2
  if(currentMillis - startMillis >= period)
  {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Use button C for");
  lcd.setCursor(0, 1);
  lcd.print("Co2 control");
  currentMillis = currentMillis - currentMillis;
  }
}

*/





// my version

/*
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char Outputs[] = 
{'1', '2', '3', 'A', 
'4', '5', '6', 'B', 
'7', '8', '9', 'C',
'*', '0', '#', 'D'};

int pinInputs[] = {'1', '2', '4', '5', '7', '9', "10", "12"};
int states[8];


void setup() {
  // put your setup code here, to run once:
  LcdSetup();  
  for(int pin = 0; pin < pinInputs; pin++)
  {
    pinMode(pin, INPUT);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
    for(int pin = 0; pin < pinInputs; pin++)
    {
      states[pin] = digitalRead(pin);
    
      if(states[pin] = HIGH)
      {
        lcd.clear();
        lcd.print(pin);  
      }
    }

}


*/



void LcdSetup()
{
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  /*
  lcd.print("co2 sensor prehe");
  lcd.setCursor(0, 1);
  lcd.print("ating, please wait...");
  delay(10000);
  lcd.clear();
  lcd.print("...");
  delay(2000);
  lcd.print(".....");
  delay(2000);
  lcd.print(".......");
  delay(2000);
  */
 }
 
 void TemperatureControl()
 {
   int temperature = dht.readTemperature();

   if(temperature < minTempTarget)
   {
     digitalWrite(fanRelayPin, LOW);
     Serial.println("temperature too low");
     return;
   }
   if(temperature > maxTempTarget)
    {
      digitalWrite(fanRelayPin, HIGH);
      Serial.println("temperature too high");
      return;
    }
  else 
    digitalWrite(fanRelayPin, HIGH);    
 }
 
 void HumidityControl()
 {
   // call function in loop
   int humidity = dht.readHumidity();

   if(humidity < minHumidityTarget)
   {
     digitalWrite(fanRelayPin, LOW);
     Serial.println("HEHOOO LA HUMIDITÉ PAS FORT");
   }

   if(humidity > maxHumidityTarget)
   {
    digitalWrite(fanRelayPin, HIGH);
    Serial.println("Humidité trop forte");
   }
   else
   {
     Serial.println(humidity);
   }
 }

 void Co2Control()
 {
   int ppm = co2.readCO2PWM();

   if(ppm < minCo2Target)
   {
     Serial.println("too low co2!");
   }
 }
