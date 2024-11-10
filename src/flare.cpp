#include "flare.h"

// Constructor: allocate memory based on ledcount
Flare::Flare(uint8_t ledcount, size_t bitPositionStart) : ledCount(ledcount), bitPosition(bitPositionStart)
{
    // Allocate memory for the brightness array based on ledCount
    maxBrightnessArray = new uint8_t[ledCount];
    brightnessArray = new uint8_t[ledCount];
    // Initialize the array to zero brightness
    for (uint8_t i = 0; i < ledCount; ++i)
    {
        maxBrightnessArray[i] = 0;
        brightnessArray[i] = 0;
    }
    blink_timer = millis();
    led_state = FLARE_FADE_DIRECTION::ON;
}

// Destructor: free the allocated memory
Flare::~Flare()
{
    delete[] maxBrightnessArray; // Free the dynamically allocated array
}

// Set the entire brightness array
void Flare::set_maxbrightnessarray(const uint8_t *brightnessArrayInput, size_t length)
{
    // Copy values, ensuring we don’t exceed the allocated size
    for (size_t i = 0; i < length && i < ledCount; ++i)
    {
        maxBrightnessArray[i] = brightnessArrayInput[i];
    }
}

// Set brightness for a specific LED
void Flare::set_maxbrightness(uint8_t brightness, uint8_t ledindex)
{
    // Ensure the index is within bounds
    if (ledindex < ledCount)
    {
        maxBrightnessArray[ledindex] = brightness;
    }
}

size_t Flare::get_brightnessarray(uint8_t *outArray, size_t maxLength) const
{
    // Determine the number of elements to copy
    size_t elementsToCopy = (ledCount < maxLength) ? ledCount : maxLength;

    // Copy brightness values to the output array
    for (size_t i = 0; i < elementsToCopy; ++i)
    {
        outArray[i] = brightnessArray[i];
    }

    return elementsToCopy;
}

uint8_t Flare::get_brightness(uint8_t ledIndex)
{
    return brightnessArray[ledIndex];
}

uint8_t Flare::get_arraysize()
{
    return ledCount;
}

bool Flare::getBit()
{
    // Calculate the byte index and the bit index within that byte
    size_t byteIndex = bitPosition / 8;
    size_t bitIndex = bitPosition % 8;

    // Get the bit at the specified position
    return (flareSequence[byteIndex] >> (7 - bitIndex)) & 1;
}

// Update method (add your logic for LED brightness update here)
void Flare::update()
{
    unsigned long time_in_sequence = millis() - blink_timer;
    if (time_in_sequence > 50)
    {
        bool last_state = getBit();
        blink_timer = millis();
        if (5999 <= bitPosition)
            bitPosition = 0;
        else
            bitPosition++;
        bool act_state = getBit();

        if (act_state)
            led_state = last_state ? FLARE_FADE_DIRECTION::ON : FLARE_FADE_DIRECTION::FADE_IN;
        else
            led_state = last_state ? FLARE_FADE_DIRECTION::FADE_OUT : FLARE_FADE_DIRECTION::OFF;
    }

    for (int i = 0; i < ledCount; i++)
        brightnessArray[i] = getBit() ? maxBrightnessArray[i] : 100;

    /*
        if (FLARE_FADE_DIRECTION::ON == led_state)
        {
            for (int i = 0; i < ledCount; i++)
                brightnessArray[i] = maxBrightnessArray[i];
        }
        else if (FLARE_FADE_DIRECTION::OFF == led_state)
        {
            for (int i = 0; i < ledCount; i++)
                brightnessArray[i] = 0;
        }
        else if (FLARE_FADE_DIRECTION::FADE_IN == led_state)
        {
            for (int i = 0; i < ledCount; i++)
                brightnessArray[i] = constrain(map(time_in_sequence, 0, 50, 0, maxBrightnessArray[i]), 0, maxBrightnessArray[i]);
        }
        else
        {
            for (int i = 0; i < ledCount; i++)
                brightnessArray[i] = constrain(map(time_in_sequence, 0, 50, maxBrightnessArray[i], 0), 0, maxBrightnessArray[i]);
        }*/
}
