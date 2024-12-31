#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define MQ303_PIN 1
#define HEATER_PIN 0
#define EEPROM_SIZE (256)       // TAMANHO DA EEPROM
#define EEPROM_ADDR (0x50)      // ENDEREÇO DA EEPROM 24C256
#define ALC_SENSOR_ADDR (0x48)  // ENDEREÇO I2C DO SENSOR (ADS1115)
Adafruit_ADS1115 ads;
bool analog_debug = false;
bool sensor_debug = false;

unsigned long espMillis(){
    return esp_timer_get_time()/1000;
}

bool checkAddress(int address, int timeout) {
    const unsigned long startTime = espMillis();

    if(sensor_debug)
        return true;

    while(espMillis() - startTime < timeout || timeout == 0){
        Wire.beginTransmission(address);

        if(Wire.endTransmission() == 0)
            return true;

        if(timeout == 0)
            return false;

        delay(10);
    }

    return false;
}

int getAlcohol(){
    static unsigned long startTime;
    static float X_n1, X_n2;
    static float Y_n1, Y_n2;

    if(espMillis() - startTime < 50)
        return Y_n1;

    startTime = espMillis();
    
    if(!checkAddress(ALC_SENSOR_ADDR, 0))
        return -1;
    
    const float X_n = ads.readADC_SingleEnded(MQ303_PIN);
    const float Y_n = X_n*(0.558698) + X_n1*(0.257307) + Y_n1*(0.292448) + Y_n2*(-0.108453);

    Y_n2 = Y_n1; Y_n1 = Y_n;
    X_n2 = X_n1; X_n1 = X_n;
    return Y_n;
}

int getAnalogValue(const bool filterOn){
    int analog = getAlcohol();

    if(analog_debug)
        return 23000;

    if(analog == -1)
        return -1;

    if(!filterOn)
        return ads.readADC_SingleEnded(MQ303_PIN);
    
    if(analog < 1000 || analog > 50000)
        return -1;

    return analog;
}

void setup(){
    Serial.begin(115200);
    Wire.begin(18, 19);
    ads.begin(ALC_SENSOR_ADDR);

    pinMode(HEATER_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, LOW);
}

void loop(){
    int analog = getAnalogValue(true);
    int raw    = getAnalogValue(false);

    if(raw == -1 || analog == -1)
        return;
    
    Serial.print(raw);
    Serial.print(",");
    Serial.println(analog);
    delay(20);
}

