#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Gammacorrection.h>
#include <Flare.h>

#define LEDSTRIP_COUNT 22
#define LEDSTRIP_PIN 0

enum class RING_FADE_DIRECTION
{
  FADE_IN,
  FADE_OUT
};

typedef struct
{
  unsigned long event;
  RING_FADE_DIRECTION direction;
} RING_STATE;

RING_STATE ring[12];
byte ring_min_brightness = 10;
byte ring_stepper_step = 0;
byte ring_stepper_flag = 0;
unsigned long ring_stepper_timer;

Adafruit_NeoPixel strip(LEDSTRIP_COUNT, LEDSTRIP_PIN, NEO_GRB + NEO_KHZ800);

ColorHSV colorHSV;
ColorRGB colorRGB;

Flare bottle2(3, 3);

void setup()
{

  (void)pinMode(LEDSTRIP_PIN, OUTPUT);

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.setBrightness(255);

  colorHSV.h = 40;
  colorHSV.s = 255;
  colorHSV.v = 255;
  HSVtoRGB2(&colorRGB, &colorHSV);

  for (int i = 0; i < 12; i++)
  {
    ring[i].direction = (i == 0) ? RING_FADE_DIRECTION::FADE_IN : RING_FADE_DIRECTION::FADE_OUT;
    ring[i].event = millis();
  }

  for (int i = 0; i < 22; i++)
  {
    if (i < 10)
    {
      if (i == 4)
        strip.setPixelColor(i, 255, 255, 255);
      else if (i > 5)
        strip.setPixelColor(i, 255, 255, 255);
      else
        strip.setPixelColor(i, 100, 100, 100);
    }
    else
      strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();

  bottle2.set_maxbrightness(100,0);
  bottle2.set_maxbrightness(255,1);
  bottle2.set_maxbrightness(100,2);

  ring_stepper_timer = millis();
}

void loop()
{
  // BOTTLES
  bottle2.update();
  for (int i = 0 ; i < 3; i++){
        strip.setPixelColor(i+3, gamma8[bottle2.get_brightness(i)], gamma8[bottle2.get_brightness(i)], gamma8[bottle2.get_brightness(i)]);
  }


  // RING

  if (millis() - ring_stepper_timer > 300)
  {
    ring_stepper_timer = millis();
    ring_stepper_step++;
    if (12 <= ring_stepper_step)
      ring_stepper_step = 0;
  }

  if (ring_stepper_step != ring_stepper_flag)
  {
    ring[ring_stepper_step].direction = RING_FADE_DIRECTION::FADE_IN;
    ring[ring_stepper_step].event = millis();
    ring[ring_stepper_flag].direction = RING_FADE_DIRECTION::FADE_OUT;
    ring[ring_stepper_flag].event = millis();
  }
  ring_stepper_flag = ring_stepper_step;

  for (int i = 0; i < 12; i++)
  {
    if (ring[i].direction == RING_FADE_DIRECTION::FADE_IN)
    {
      colorHSV.v = constrain(map(millis() - ring[i].event, 0, 300, ring_min_brightness, 255), ring_min_brightness, 255);
    }
    else
    {
      colorHSV.v = constrain(map(millis() - ring[i].event, 0, 3300, 255, ring_min_brightness), ring_min_brightness, 255);
    }
    HSVtoRGB2(&colorRGB, &colorHSV);
    strip.setPixelColor(i + 10, colorRGB.r, colorRGB.g, colorRGB.b);
  }
  strip.show();
}
