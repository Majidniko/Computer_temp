#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_NeoPixel.h>


#define PIN 6


RTC_DS3231 rtc;
Adafruit_AHTX0 aht;
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int result[6];
int RelayPin = 4;
unsigned long previousMillis = 0;  //will store last time LED was blinked
const long period = 30000;         // period at which to blink in ms
int R = 0;
int G = 0;
int B = 0;
float maxtemp = 35;
float maxleveltemp = 34;
float Ntemp = 0;
float maxTemp=0, maxHum=0,minTemp=101, minHum=101;




void setup() {
  Serial.begin(9600);
  delay(3000);  // wait for console opening

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");


  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");

    // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);

}

void flasher(int RA, int GA, int BA) {
  
  if (Ntemp < maxtemp) {
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    delay(1000);
    pixels.setPixelColor(0, pixels.Color(RA, GA, BA));
    pixels.show();
    delay(2000);
  } else {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(300);
      pixels.setPixelColor(0, pixels.Color(RA, GA, BA));
      pixels.show();
      delay(100);
    }
  }
}

void history(float temp, float humidiyt) {
  if (temp > maxTemp)
    maxTemp = temp;
  if (temp < minTemp)
    minTemp = temp;
  if (humidiyt > maxHum)
    maxHum = humidiyt;
  if (humidiyt < minHum)
    minHum = humidiyt;
}

void test(){
  digitalWrite(RelayPin, LOW);
  for(int i=0;i<10;i++)
      flasher(0,0,255);
    digitalWrite(RelayPin, HIGH);
}

void loop() {
  DateTime now = rtc.now();
  Serial.print("*Date: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print("*Time: ");
  if (now.hour() < 10) {
    Serial.print('0');
    Serial.print(now.hour(), DEC);
  } else
    Serial.print(now.hour(), DEC);
  Serial.print(':');
  if (now.minute() < 10) {
    Serial.print('0');
    Serial.print(now.minute(), DEC);
  } else
    Serial.print(now.minute(), DEC);
  Serial.print(':');
  if (now.second() < 10) {
    Serial.print('0');
    Serial.print(now.second(), DEC);
  } else
    Serial.print(now.second(), DEC);
  Serial.print(' ');

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  Serial.print(" *Temperature: ");
  Ntemp = temp.temperature;
  Serial.print(Ntemp);
  Serial.print(" degrees C & ");
  Serial.print("*Humidity: ");
  Serial.print(humidity.relative_humidity);
  history(temp.temperature, humidity.relative_humidity);
  Serial.print("% rH");
  if (digitalRead(RelayPin) == LOW)
    Serial.print(" *Fan on ");
  else
    Serial.print(" *Fan off ");
  Serial.print(" (Max Temp: " + String(maxTemp) + ") (Min Temp: " + String(minTemp) + ") (Max Humidity: " + String(maxHum) + ") (Min Humidity: " + String(minHum)+")");
  if (digitalRead(RelayPin) == LOW) {
    if (temp.temperature < maxleveltemp) {
      R = 150;
      G = 0;
      B = 150;
      pixels.setPixelColor(0, pixels.Color(R, G, B));
      digitalWrite(RelayPin, HIGH);
      pixels.show();
    }
  }

  if (digitalRead(RelayPin) == HIGH) {
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    if (temp.temperature <= 30) {
      R = 0;
      G = 150;
      B = 0;
      pixels.setPixelColor(0, pixels.Color(0, 150, 0));
      digitalWrite(RelayPin, HIGH);
      pixels.show();
    }

    if (temp.temperature > 30 && temp.temperature < maxtemp) {
      R = 150;
      G = 0;
      B = 150;
      pixels.setPixelColor(0, pixels.Color(R, G, B));
      //digitalWrite(RelayPin, HIGH);
      pixels.show();
    }

    if (temp.temperature >= maxtemp) {
      R = 150;
      G = 0;
      B = 0;
      pixels.setPixelColor(0, pixels.Color(R, G, B));
      digitalWrite(RelayPin, LOW);

      pixels.show();
    }
  }
  Serial.print(" ");
  unsigned long currentMillis = millis();     // store the current time
  while (currentMillis + 10000 > millis()) {  // check if 1000ms passed
    flasher(R, G, B);
    if (Serial.available()){
      Serial.println();
      Serial.println("Command mode:");
      delay(500);
      String command = Serial.readString();
      Serial.println(command + " Recived");
      Serial.println(command.indexOf("test"));
      if(command.indexOf("test")>=0){
        Serial.println("Test is running...");
          test();
          Serial.println("Test Finished");
      }
      Serial.println("Exit command mode.");
    }
  }
  Serial.println();
}
