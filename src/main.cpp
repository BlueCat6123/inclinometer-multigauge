#include <Arduino.h>
#include <TFT_eSPI.h>
//#include <Wire.h>
#include <GaugeFace.h>
#include <ezButton.h>

#include <langauges.h> // File containing all sentences and translations

#include <units.h> // File containing all units and conversions

#include <menu.h>

#include "vehicleSprites.h"

#include <list>

#include "FastLED.h"

#define DATA_PIN    26
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    7
#define BRIGHTNESS  255


CRGB leds[NUM_LEDS];

void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}


std::vector<String> stringList;

TFT_eSPI lcd = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&lcd);

TFT_eSprite wheel = TFT_eSprite(&lcd);
//TFT_eSprite vehicle = TFT_eSprite(&lcd);

int rpm = 0;

float pitch, roll, yaw;

const uint16_t colors[] = {TFT_RED, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_CYAN, TFT_DARKGREY};
const std::vector<String> colorNames = {"Red", "Blue", "Green", "Yellow", "Cyan", "Grey"};

const int numColors = sizeof(colors) / sizeof(colors[0]);
int colorIndex = 0;

int primaryColorIndex = 0;
int secondaryColorIndex = 0;

#define BUTTON_LEFT_PIN   32
#define BUTTON_RIGHT_PIN  25
#define BUTTON_MIDDLE_PIN 33

//ezButton leftButton(BUTTON_LEFT_PIN);
ezButton middleButton(BUTTON_MIDDLE_PIN);
ezButton rightButton(BUTTON_RIGHT_PIN);



void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  lcd.init();
  lcd.setRotation(2);

  lcd.fillScreen(TFT_BLACK);
  canvas.createSprite(238,238);
  canvas.setPivot(canvas.width() >> 1, canvas.height() >> 1);
  lcd.startWrite();
}

int t = 0;

void ledUpdate() {
  pride();
  FastLED.show();  
}

void loop() {

  canvas.fillScreen(TFT_BLACK);
  canvas.fillCircle(120,120,int(50+sin(t*0.1)*40),TFT_RED);
  ledUpdate();

  canvas.pushSprite(1, 1);
  t++;
}