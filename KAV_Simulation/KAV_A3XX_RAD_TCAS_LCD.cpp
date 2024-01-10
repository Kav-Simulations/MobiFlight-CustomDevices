#include "KAV_A3XX_RAD_TCAS_LCD.h"

#define DIGIT_ONE 0
#define DIGIT_TWO 1
#define DIGIT_THREE 2
#define DIGIT_FOUR 3
#define DIGIT_FIVE 4
#define DIGIT_SIX 5

// Helper macro to set a specific bit in the buffer
#define SET_BUFF_BIT(addr, bit, enabled) buffer[addr] = (buffer[addr] & (~(1 << (bit)))) | (((enabled & 1)) << (bit));

/**
 * Setup the LCD
 * This function is called when the deivce is initialised using the 'attach' function.
 * It sets up the LCD and clears it.
 */
void KAV_A3XX_RAD_TCAS_LCD::begin()
{
    ht_rad_tcas.begin();
    ht_rad_tcas.sendCommand(HT1621::RC256K);
    ht_rad_tcas.sendCommand(HT1621::BIAS_THIRD_4_COM);
    ht_rad_tcas.sendCommand(HT1621::SYS_EN);
    ht_rad_tcas.sendCommand(HT1621::LCD_ON);
    // This clears the LCD
    for (uint8_t i = 0; i < ht_rad_tcas.MAX_ADDR; i++)
        ht_rad_tcas.write(i, 0);

    // Initialises the buffer to all 0's.
    memset(buffer, 0, BUFFER_SIZE_MAX);
}

/**
 * Initialise the LCD (Entry Point)
 * This function is called to initialise the LCD.
 * @param CS The Chip Select pin
 * @param CLK The Clock pin
 * @param DATA The Data pin
 */
void KAV_A3XX_RAD_TCAS_LCD::attach(byte CS, byte CLK, byte DATA)
{
    _CS = CS;
    _CLK = CLK;
    _DATA = DATA;
    _initialised = true;
    begin();
}

/**
 * Detach the LCD
 * Required for MobiFLight
 */
void KAV_A3XX_RAD_TCAS_LCD::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

/**
 * Refresh the LCD
 * After a change is made to a segment, the display must be refreshed so that
 * the change is visible.
 * @param address The address to refresh
 */
void KAV_A3XX_RAD_TCAS_LCD::refreshLCD(uint8_t address)
{
    ht_rad_tcas.write(address * 2, buffer[address], 8);
}

/**
 * Clear the LCD
 * This function clears the LCD and resets the buffer.
 */
void KAV_A3XX_RAD_TCAS_LCD::clearLCD()
{
    for (uint8_t i = 0; i < ht_rad_tcas.MAX_ADDR; i++)
        ht_rad_tcas.write(i, 0);
    memset(buffer, 0, BUFFER_SIZE_MAX);
}

void KAV_A3XX_RAD_TCAS_LCD::clearDigit(uint8_t address)
{
    ht_rad_tcas.write(address * 2, 0);
    buffer[address] = 0;
}

/**
 * Set the dot to show or hide.
 * @param enabled Whether to show the dot (true) or hide it (false)
 */
void KAV_A3XX_RAD_TCAS_LCD::setRadioDot(bool enabled)
{
    // The dot is the 4th bit of the DIGIT_THREE address.
    SET_BUFF_BIT(DIGIT_THREE, 4, enabled);
    refreshLCD(DIGIT_THREE);
}

/**
 * Set a specific dot to show or hide.
 * The address for each character are 0 indexed:
 * 0: DIGIT_ONE, 1: DIGIT_TWO, 2: DIGIT_THREE, 3: DIGIT_FOUR, 4: DIGIT_FIVE, 5: DIGIT_SIX
 * @param address The address of the dot to show or hide
 * @param enabled Whether to show the dot (true) or hide it (false)
 */
void KAV_A3XX_RAD_TCAS_LCD::setSpecificDot(uint8_t address, bool enabled)
{
    // The dot is the 4th bit of the address.
    SET_BUFF_BIT(address, 4, enabled);
    refreshLCD(address);
}

/**
 * Set all dots to show or hide.
 * @param enabled Whether to show the dots (true) or hide them (false)
 */
void KAV_A3XX_RAD_TCAS_LCD::setAllDots(bool enabled)
{
    for (uint8_t i = 0; i < 6; i++)
        setSpecificDot(i, enabled);
}

/**
 * Set the value of the LCD using an integer for radio (6 characters).
 * @param value The value to display
 */
void KAV_A3XX_RAD_TCAS_LCD::setRadioValue(uint32_t value)
{
    if (value == 0)
    {
        displayDigit(DIGIT_ONE, 11);
        displayDigit(DIGIT_TWO, 13);
        displayDigit(DIGIT_THREE, 14);
        displayDigit(DIGIT_FOUR, 15);
        displayDigit(DIGIT_FIVE, 14);
        displayDigit(DIGIT_SIX, 11);
    } else {
        if (value > 999999)
            value = 999999;
        displayDigit(DIGIT_SIX, (value % 10));
        value = value / 10;
        displayDigit(DIGIT_FIVE, (value % 10));
        value = value / 10;
        displayDigit(DIGIT_FOUR, (value % 10));
        value = value / 10;
        displayDigit(DIGIT_THREE, (value % 10));
        value = value / 10;
        displayDigit(DIGIT_TWO, (value % 10));
        displayDigit(DIGIT_ONE, (value / 10));
    }
}

/**
 * Set the value of the LCD using an integer for TCAS (4 characters).
 * @param value The value to display
 */
void KAV_A3XX_RAD_TCAS_LCD::setTcasValue(uint16_t value)
{
    if (value > 9999)
        value = 9999;
    displayDigit(DIGIT_FIVE, (value % 10));
    value = value / 10;
    displayDigit(DIGIT_FOUR, (value % 10));
    value = value / 10;
    displayDigit(DIGIT_THREE, (value % 10));
    displayDigit(DIGIT_TWO, (value / 10));
}

// Show values as a combined function
/**
 * Show the value on the display for radio using an integer.
 * This function will also set the dot for radio.
 * @param value The value to display
 */
void KAV_A3XX_RAD_TCAS_LCD::showRadio(uint32_t value)
{
    setRadioValue(value);
    if (value == 0)
        setRadioDot(false);
    else
        setRadioDot(true);
}

/**
 * Show the value on the display for TCAS using an integer.
 * This will also clear all the dots.
 * @param value The value to display
 */
void KAV_A3XX_RAD_TCAS_LCD::showTcas(uint16_t value)
{
    setTcasValue(value);
    displayDigit(DIGIT_ONE, 11);
    displayDigit(DIGIT_SIX, 11);
    setRadioDot(false);
}

/**
 * Show the aircraft 'test' pattern on the display.
 * @param enabled Whether to show the test pattern (true) or hide it (false)
 */
void KAV_A3XX_RAD_TCAS_LCD::showTest(bool enabled)
{
    if (enabled)
    {
        setRadioValue(888888);
        setAllDots(enabled);
    }
    else
    {
        clearLCD();
    }
}

// Global Functions
/**
 * A list of the binary patterns to show different characters on the LCD.
 */
uint8_t digitPatternRadTcas[16] = {
    0b11101011, // 0
    0b01100000, // 1
    0b11000111, // 2
    0b11100101, // 3
    0b01101100, // 4
    0b10101101, // 5 or S
    0b10101111, // 6
    0b11100000, // 7
    0b11101111, // 8
    0b11101101, // 9
    0b00000100, // -
    0b00000000, // blank
    0b11001100, // small 0 (For V/S)
    // Below are characters for 'dAtA'
    0b01100111, // d
    0b11101110, // A
    0b00001111, // t
};

/**
 * Display a digit on a specific address.
 * @param address The address to display the digit on
 * @param digit The digit to display
 * @see digitPatternRadTcas
 */
void KAV_A3XX_RAD_TCAS_LCD::displayDigit(uint8_t address, uint8_t digit)
{
    // This ensures that anything over 12 is turned to 'blank', and as it's unsigned, anything less than 0 will become 255, and therefore, 'blank'.
    if (digit > 15)
        digit = 11;

    buffer[address] = digitPatternRadTcas[digit];

    refreshLCD(address);
}

/**
 * Handle MobiFlight Commands
 * This function shouldn't be called be a user, it should only be called by the
 * custom device function. This is where data from MobiFlight enters the
 * library and is handled to be displayed on the LCD.
 * @param cmd The command from MobiFlight
 * @see handleMobiFlightRaw
 */
void KAV_A3XX_RAD_TCAS_LCD::set(int16_t messageID, char *setPoint)
{
    int32_t data = strtoul(setPoint, NULL, 10);
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -1 will be send from the connector when Mobiflight is closed
        Put in your code to shut down your custom device (e.g. clear a display)
        MessageID == -2 will be send from the connector when PowerSavingMode is entered
        Put in your code to enter this mode (e.g. clear a display)
    ********************************************************************************** */
    if (messageID == -1)
        return; // Ignore for now, handle this condition later.
    else if (messageID == -2)
        return; // Ignore for now, handle this condition later.
    else if (messageID == 0)
        setRadioDot((uint16_t)data);
    else if (messageID == 1)
        setAllDots((uint16_t)data);
    else if (messageID == 2)
        showRadio((uint32_t)data);
    else if (messageID == 3)
        showTcas((uint16_t)data);
}