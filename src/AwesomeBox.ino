/*
 * Project AwesomeBox
 * Description:
 * Author: Kai Nakamura
 * Date: 8/9/2018
 */

#include <LiquidCrystal.h>

/* Setup our awesome constants */
#define GET_TRAFFIC_EVENT_NAME "AwesomeBoxGoogle"
#define GET_WEATHER_EVENT_NAME "AwesomeBoxDarkSky"

String minutelyWeatherSummary = "";
String hourlyWeatherSummary = "";
String dailyWeatherSummary = "";

String timeWithTrafficStr = "";
int timeWithTraffic = 0;
int timeWithoutTraffic = 0;

unsigned long previousFetchDataMillis = 0;
unsigned long previousDisplayDataMillis = 0;

/* Setup our awesome liquid crystal display */
LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

void setupCloudSubscriptions() {
  //Particle.subscribe
  Particle.subscribe("hook-response/" + String(GET_TRAFFIC_EVENT_NAME), gotTrafficData, MY_DEVICES);
  Particle.subscribe("hook-response/" + String(GET_WEATHER_EVENT_NAME), gotWeatherData, MY_DEVICES);

}
void setupLCD() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16,2);
  lcd.print("Hello Friend");
}


void setup() {
  setupLCD();
  setupCloudSubscriptions();
  requestCloudInfo();
}
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousFetchDataMillis >= 30 * 1000) {
    //Every 30 seconds, fetch data
    requestCloudInfo();
    previousFetchDataMillis = currentMillis;
    displayTraffic();
  }

  if (currentMillis - previousDisplayDataMillis >= 5 * 1000) {
    //Every 5 seconds, refresh the LCD
    lcd.clear();
    displayTraffic();
    displayWeather();
  }

}
void requestCloudInfo() {

  Particle.publish(String(GET_TRAFFIC_EVENT_NAME), PRIVATE);
  Particle.publish(String(GET_WEATHER_EVENT_NAME), PRIVATE);

}
void gotTrafficData(const char *name, const char *data) {
  //Parse them and store in global variable

  String d = String(data);

  int prevIdx;
  int currIdx;

  for (int i = 0; i < 3; i++) {
    if (i == 0) {
      prevIdx = 0;
      currIdx = d.indexOf(";");
      timeWithTrafficStr = d.substring(prevIdx + 1, currIdx);
    } else {
      prevIdx = currIdx;
      currIdx = d.indexOf(";", prevIdx + 1);
      if (i == 1) {
        timeWithTraffic = d.substring(prevIdx + 1, currIdx).toInt();
      } else if (i == 2) {
        timeWithoutTraffic = d.substring(prevIdx + 1, currIdx).toInt();
      }
    }
  }

}
void gotWeatherData(const char *name, const char *data) {

  String d = String(data);

  int prevIdx;
  int currIdx;

  for (int i = 0; i < 3; i++) {
    if (i == 0) {
      prevIdx = 0;
      currIdx = d.indexOf(";");
      minutelyWeatherSummary = d.substring(prevIdx + 1, currIdx);
    } else {
      prevIdx = currIdx;
      currIdx = d.indexOf(";", prevIdx + 1);
      if (i == 1) {
        hourlyWeatherSummary = d.substring(prevIdx + 1, currIdx);
      } else if (i == 2) {
        dailyWeatherSummary = d.substring(prevIdx + 1, currIdx);
      }
    }
  }

}


void displayWeather() {

  //Minutely Weather
  lcd.setCursor(16, 0);
  lcd.autoscroll();

  String msg = minutelyWeatherSummary + " " + hourlyWeatherSummary + " " + dailyWeatherSummary;

  int strLen = msg.length() + 1;
  char charArr[strLen];
  msg.toCharArray(charArr, strLen);

  for (int c = 0; c < msg.length(); c++) {
    lcd.print(charArr[c]);
    delay(100);
  }

  lcd.noAutoscroll();

}
void displayTraffic() {

  lcd.setCursor(16, 1);

  String msg = timeWithTrafficStr;

  if (timeWithTraffic > timeWithoutTraffic) {
    //There is traffic
    msg += " :( +";
  } else {
    msg += " :) -";
  }

  int diff = abs(timeWithTraffic - timeWithoutTraffic);
  msg += String(diff);

}
