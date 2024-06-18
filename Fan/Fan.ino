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
                   // Set RelayPin as an output pin
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
}


void loop() {
  DateTime now = rtc.now();

  Serial.println("Current Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Serial.println("Unix Time: ");
  // Serial.print("elapsed ");
  // Serial.print(now.unixtime());
  // Serial.print(" seconds/");
  // Serial.print(now.unixtime() / 86400L);
  // Serial.println(" days since 1/1/1970");

  // // calculate a date which is 7 days & 30 seconds into the future
  // DateTime future(now + TimeSpan(7, 0, 0, 30));

  // Serial.println("Future Date & Time (Now + 7days & 30s): ");
  // Serial.print(future.year(), DEC);
  // Serial.print('/');
  // Serial.print(future.month(), DEC);
  // Serial.print('/');
  // Serial.print(future.day(), DEC);
  // Serial.print(' ');
  // Serial.print(future.hour(), DEC);
  // Serial.print(':');
  // Serial.print(future.minute(), DEC);
  // Serial.print(':');
  // Serial.print(future.second(), DEC);
  // Serial.println();

  Serial.println();

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println("% rH");

  Serial.println();
  if( digitalRead(RelayPin)==LOW){
      if (temp.temperature <35) {
    pixels.setPixelColor(0, pixels.Color(0, 150, 0));
    digitalWrite(RelayPin, HIGH);
    pixels.show();
  }
  }
  if( digitalRead(RelayPin)==HIGH){
       // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  if (temp.temperature <= 30) {
    pixels.setPixelColor(0, pixels.Color(0, 150, 0));
    digitalWrite(RelayPin, HIGH);
    pixels.show();
  }

  if (temp.temperature > 30 && temp.temperature < 36) {

    pixels.setPixelColor(0, pixels.Color(150, 0, 150));
    //digitalWrite(RelayPin, HIGH);
    pixels.show();
  }

  if (temp.temperature >= 36) {

    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    digitalWrite(RelayPin, LOW);
    pixels.show();
  }
  }
for(int i=0;i<30;i++){
  Serial.print('-');
  delay(1000);
}
  Serial.println();
  // if (Serial.available() > 0) {
  //   String command = Serial.readString();
  // }
}