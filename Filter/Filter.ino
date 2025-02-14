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

float getAlcohol(){
    static unsigned long startTime;
    static const byte xSize=2, ySize=3;
    static float Xn[xSize] = {0};
    static float Yn[ySize] = {0};

    if(espMillis() - startTime < 100)
        return Yn[0];

    if(!tryConnection(ALC_SENSOR_ADDR))
        return -1;

    startTime = espMillis();

    for(byte n=xSize-1; n>0; n--)
        Xn[n] = Xn[n-1];

    for(byte n=ySize-1; n>0; n--) 
        Yn[n] = Yn[n-1];

    Xn[0] = ads.readADC_SingleEnded(MQ303_PIN);
    Yn[0] = Xn[0]*(0.016239) + Xn[1]*(0.014858) + Yn[1]*(1.734903) + Yn[2]*(-0.766000);
    return Yn[0];
}

int getAnalogValue(bool filter){
    if(analog_debug)
        return 23000;

    if(!filter)
        return ads.readADC_SingleEnded(MQ303_PIN);

    const int analog = getAlcohol();

    if(analog == -1 || analog == 0)
        return -1;
    
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

bool tryConnection(int address){
    if(sensor_debug)
        return true;
    
    Wire.beginTransmission(address);

    if(Wire.endTransmission() == 0)
        return true;

    return false;
}

bool checkAddress(int address, int timeout){
    const unsigned long startTime = espMillis();

    if(sensor_debug) 
        return true;

    while(espMillis() - startTime < timeout){
        if(tryConnection(address))
            return true;

        delay(100);
    }
    
    return false;
}
