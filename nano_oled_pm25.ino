//
// Small Simple OLED library demo for AVR platform,
// without line drawing function (saving of 1K RAM)
//
#include <ss_oled.h>

SSOLED ssoled;
#define SDA_PIN 18
#define SCL_PIN 19
// no reset pin needed
#define RESET_PIN -1
// let ss_oled find the address of our display
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
// Use the default Wire library
#define USE_HW_I2C 1


#include "Adafruit_PM25AQI.h"
// If your PM2.5 is UART only, for UNO and others (without hardware serial) 
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// a button with pull-up resistor toggles the "standard" or "env" mode
int buttonPin = 12;

void setup()
{
   // Define pin #12 as input and activate the internal pull-up resistor
   pinMode(buttonPin, INPUT_PULLUP);
  
  int rc;
  rc = oledInit(&ssoled, OLED_128x64, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);       // Standard HW I2C bus at 400Khz

  if (rc == OLED_NOT_FOUND) {
    return;
  }
  
  char *msgs[] =
  {
    (char *)"SSD1306 @ 0x3C",
    (char *)"SSD1306 @ 0x3D",
    (char *)"SH1106 @ 0x3C",
    (char *)"SH1106 @ 0x3D"
  };

  oledFill(&ssoled, 0, 1);
  oledWriteString(&ssoled, 0, 2, 0, (char *)"OLED found:", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0, 4, 2, msgs[rc], FONT_NORMAL, 0, 1);
  delay(2000);

  //-

  oledFill(&ssoled, 0, 1);
  oledWriteString(&ssoled, 0, 2, 0, (char *)"Adafruit PMSA003I", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0, 2, 2, (char *)"Air Quality", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0, 4, 4, (char *)"Sensor", FONT_NORMAL, 0, 1);
  
  

  // Wait at least one second for sensor to boot up!
  delay(2000);

  // If using serial, initialize it and set baudrate before starting!
  // Uncomment one of the following
  //Serial1.begin(9600);
  pmSerial.begin(9600);

  // There are 3 options for connectivity!
//    if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0, 0, 0, (char *)"Waiting for sensor...", FONT_NORMAL, 0, 1);
    while (1) delay(100);
  }

  oledFill(&ssoled, 0, 1);
  oledWriteString(&ssoled, 0, 0, 0, (char *)"Sensor found", FONT_NORMAL, 0, 1);
  delay(1000);
}

void loop()
{
  bool envMode = envModeEnabled();
  
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
//    oledWriteString(&ssoled, 0, 0, 0,(char *)"Could not read from AQI", FONT_NORMAL, 0, 1);
    delay(500);  // try again in a bit!
    return;
  }

  char pm10cstr[16];
  char pm25cstr[16];
  char pm100cstr[16];

  char *score_pm10;
  char *score_pm25;
  char *score_pm100;

  // clears it
  oledFill(&ssoled, 0, 1);
  
  if (envMode) {
    oledWriteString(&ssoled, 0, 0, 0, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 1, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 2, (char *)"E ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 3, (char *)"N ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 4, (char *)"V ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 5, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 6, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 7, (char *)"  ", FONT_NORMAL, 1, 1);
    score_pm10  = (char *)itoa(data.pm10_env, pm10cstr,  10);
    score_pm25  = (char *)itoa(data.pm25_env, pm25cstr,  10);
    score_pm100 = (char *)itoa(data.pm10_env, pm100cstr, 10);
  } else {
    oledWriteString(&ssoled, 0, 0, 0, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 1, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 2, (char *)"S ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 3, (char *)"T ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 4, (char *)"D ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 5, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 6, (char *)"  ", FONT_NORMAL, 1, 1);
    oledWriteString(&ssoled, 0, 0, 7, (char *)"  ", FONT_NORMAL, 1, 1);
    score_pm10  = (char *)itoa(data.pm10_standard, pm10cstr,  10);
    score_pm25  = (char *)itoa(data.pm25_standard, pm25cstr,  10);
    score_pm100 = (char *)itoa(data.pm10_standard, pm100cstr, 10);
  }

  oledWriteString(&ssoled, 0, 24, 1, (char *)"PM 1.0:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled, 0, 70, 0, score_pm10, FONT_STRETCHED, 0, 1);
  
  oledWriteString(&ssoled, 0, 24, 4, (char *)"PM 2.5:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled, 0, 70, 3, score_pm25, FONT_STRETCHED, 0, 1);
  
  oledWriteString(&ssoled, 0, 24, 7, (char *)" PM 10:", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled, 0, 70, 6, score_pm100, FONT_STRETCHED, 0, 1);
  
  
//  Serial.println();
//  Serial.println(F("---------------------------------------"));
//  Serial.println(F("Concentration Units (standard)"));
//  Serial.println(F("---------------------------------------"));
//  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
//  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
//  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
//  Serial.println(F("Concentration Units (environmental)"));
//  Serial.println(F("---------------------------------------"));
//  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
//  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
//  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
//  Serial.println(F("---------------------------------------"));
//  Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
//  Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
//  Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
//  Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
//  Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
//  Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);
//  Serial.println(F("---------------------------------------"));
//  

  delay(3000);
}


bool envModeEnabled() {
  // Read the value of the input. It can either be 1 or 0
 int buttonValue = digitalRead(buttonPin);
 bool envModeDisabled = buttonValue == LOW;
 return envModeDisabled;
}
