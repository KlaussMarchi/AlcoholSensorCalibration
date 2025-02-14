#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define MQ303_PIN 1
#define HEATER_PIN 0
#define EEPROM_SIZE (256)       // TAMANHO DA EEPROM
#define EEPROM_ADDR (0x50)      // ENDEREÇO DA EEPROM 24C256
#define ALC_SENSOR_ADDR (0x48)  // ENDEREÇO I2C DO SENSOR (ADS1115)

Adafruit_ADS1115 ads;
unsigned long startProg;


unsigned long espMillis(){
    return esp_timer_get_time()/1000;
}

void setup(){
    Serial.begin(115200);
    Wire.begin(18, 19);
    ads.begin(ALC_SENSOR_ADDR); 

    pinMode(HEATER_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, LOW);
    
    while(!Serial.available())
        continue;

    startProg = espMillis();
}

void loop(){
    StaticJsonDocument<256> data;
    String response;

    data["time"]   = (espMillis() - startProg) / 1000.0;
    data["analog"] = ads.readADC_SingleEnded(MQ303_PIN);

    serializeJson(data, response);
    Serial.println(response);
}

