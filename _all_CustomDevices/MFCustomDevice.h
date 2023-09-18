#pragma once

#include <Arduino.h>
#include "../_template/MyCustomClass.h"
#include "../KAV_Simulation/EFIS_FCU/KAV_A3XX_FCU_LCD.h"
#include "../KAV_Simulation/EFIS_FCU/KAV_A3XX_EFIS_LCD.h"
#include "../Mobiflight/GNC255/GNC255.h"
#include "../Mobiflight/TM1637/TM1637.h"
#include "../Mobiflight/GenericI2C/GenericI2C.h"

enum {
    MY_CUSTOM_DEVICE_1 = 1,
    MY_CUSTOM_DEVICE_2,
    KAV_LCD_FCU,
    KAV_LCD_EFIS,
    MOBIFLIGHT_GNC255,
    MOBIFLIGHT_4TM1637,
    MOBIFLIGHT_6TM1637,
    MOBIFLIGHT_GENERICI2C
};

class MFCustomDevice
{
public:
    MFCustomDevice(uint16_t adrPin, uint16_t adrType, uint16_t adrConfig);
    void detach();
    void update();
    void set(int8_t messageID, char *setPoint);

private:
    bool               getStringFromEEPROM(uint16_t addreeprom, char *buffer);
    bool               _initialized = false;
    MyCustomClass     *_mydevice;
    KAV_A3XX_FCU_LCD  *_FCU_LCD;
    KAV_A3XX_EFIS_LCD *_EFIS_LCD;
    GNC255            *_GNC255_OLED;
    TM1637            *_TM1637;
    GenericI2C        *_myGenericI2C;
    uint8_t            _customType;
};
