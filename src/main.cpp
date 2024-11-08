#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Gammacorrection.h>

#define LED_COUNT 22
#define LED_PIN 0

enum class FADE_DIRECTION{
  FADE_IN,
  FADE_OUT,
  SADDLE_POINT,
  OFF,
  INTERRUPT
};

typedef struct
{
  unsigned long event;
  FADE_DIRECTION direction;
} LED_FADER;

LED_FADER ring[12];

byte stepper_step = 0;
byte stepper_flag = 0;
unsigned long  stepper_timer;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

ColorHSV colorHSV;
ColorRGB colorRGB;


void setup() {

  (void)pinMode(LED_PIN, OUTPUT);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255);

  colorHSV.h = 40;
  colorHSV.s = 255;
  colorHSV.v = 255;
  HSVtoRGB2(&colorRGB,&colorHSV);

  for (int i = 0; i < 12; i++){
    ring[i].direction = (i == 0) ? FADE_DIRECTION::FADE_IN : FADE_DIRECTION::FADE_OUT;
    ring[i].event = millis();
  }

  for (int i = 0; i < 22; i++){
    if (i < 10){
      if (i == 4)
        strip.setPixelColor(i, 255,255,255);
      else if (i > 5 )
        strip.setPixelColor(i, 255,255,255);
      else
        strip.setPixelColor(i, 100,100,100);
    } else
      strip.setPixelColor(i, 0,0,0);
  }
  strip.show();

  stepper_timer = millis();
}

void loop() {
  
  if (millis() - stepper_timer > 300){
    stepper_timer = millis();
    stepper_step++;
    if (12 <= stepper_step)
      stepper_step = 0;
  }

  if (stepper_step != stepper_flag){
    ring[stepper_step].direction = FADE_DIRECTION::FADE_IN;
    ring[stepper_step].event = millis();
    ring[stepper_flag].direction = FADE_DIRECTION::FADE_OUT;
    ring[stepper_flag].event = millis();
  }
  stepper_flag = stepper_step;

  for (int i = 0; i < 12; i++){
    if (ring[i].direction == FADE_DIRECTION::FADE_IN){
      colorHSV.v = constrain(map(millis() - ring[i].event,0,300,2,255),2,255);
    } else{
      colorHSV.v = constrain(map(millis() - ring[i].event,0,3300,255,2),2,255);
    }

    HSVtoRGB2(&colorRGB,&colorHSV);
    strip.setPixelColor(i+10, colorRGB.r,colorRGB.g,colorRGB.b);
  }
  strip.show();

  /*
  for (int i = 0; i < 12; i++){
    for (int j = 0; j < 12; j++){
      if (i == j)
        strip.setPixelColor(j+10, colorRGB.r,colorRGB.g,colorRGB.b);
      else
        strip.setPixelColor(j+10, 1,1,1);
    }
    strip.show();
    delay(50);
  }*/


}
