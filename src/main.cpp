#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <GaugeFace.h>
#include <ezButton.h>
#include <MPU9250_WE.h>

#include <langauges.h> // File containing all sentences and translations

#include <units.h> // File containing all units and conversions

#include <menu.h>

#include "vehicleSprites.h"
#define MPU9250_ADDR 0x68

#include <list>


std::vector<String> stringList;

MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

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

ezButton button1(12);
ezButton button2(13);


void drawWheel() {
  wheel.fillCircle( wheel.width() >> 1, wheel.height() >> 1, 100, colors[primaryColorIndex]);
  wheel.fillCircle( wheel.width() >> 1, wheel.height() >> 1, 85, TFT_BLACK);
  wheel.drawArc( wheel.width() >> 1, wheel.height() >> 1, 80, 70, 0, 360, colors[secondaryColorIndex], colors[secondaryColorIndex], false);
  for (int i = 0; i < 6; i++) {
    int angle = i*60;
    wheel.drawArc( wheel.width() >> 1, wheel.height() >> 1, 70, 30, angle, angle+20, colors[secondaryColorIndex], colors[secondaryColorIndex], false);
  }
  wheel.fillCircle( wheel.width() >> 1, wheel.height() >> 1, 30, colors[secondaryColorIndex]);
  for (int i = 0; i < 5; i++) {
    float angle = degToRad(i*72);
    wheel.fillCircle( (wheel.width() >> 1) + cos(angle) * 18, (wheel.height() >> 1) + sin(angle) * 18, 4, TFT_BLACK);
  }
  wheel.drawArc( wheel.width() >> 1, wheel.height() >> 1, 11, 8, 0, 360, TFT_BLACK, TFT_BLACK, false);
}

void drawRotor() {
  float _x1, _y1, _x2, _y2, _x3, _y3;
  float radius = (wheel.width() >> 1) - 10;

  _x1 = (wheel.width() >> 1)  + cos(0) * radius;
  _y1 = (wheel.height() >> 1) + sin(0) * radius;
  _x2 = (wheel.width() >> 1)  + cos(2.0944) * radius;
  _y2 = (wheel.height() >> 1) + sin(2.0944) * radius;
  _x3 = (wheel.width() >> 1)  + cos(4.18879) * radius;
  _y3 = (wheel.height() >> 1) + sin(4.18879) * radius;
  
  float dist = sqrt(pow(_x2-_x1,2)+pow(_y2-_y1,2));

  wheel.drawArc(_x1,_y1,(dist),0,60,120,  colors[primaryColorIndex], colors[primaryColorIndex],false);
  wheel.drawArc(_x2,_y2,(dist),0,180,240, colors[primaryColorIndex], colors[primaryColorIndex],false);
  wheel.drawArc(_x3,_y3,(dist),0,300,360, colors[primaryColorIndex], colors[primaryColorIndex],false);

  _x1 = (wheel.width() >> 1)  + cos(0) * (radius - 9);
  _y1 = (wheel.height() >> 1) + sin(0) * (radius - 9);
  _x2 = (wheel.width() >> 1)  + cos(2.0944) * (radius - 9);
  _y2 = (wheel.height() >> 1) + sin(2.0944) * (radius - 9);
  _x3 = (wheel.width() >> 1)  + cos(4.18879) * (radius - 9);
  _y3 = (wheel.height() >> 1) + sin(4.18879) * (radius - 9);
  dist = sqrt(pow(_x2-_x1,2)+pow(_y2-_y1,2));

  wheel.drawArc(_x1,_y1,(dist+2),(dist),60,120,TFT_BLACK,TFT_BLACK,false);
  wheel.drawArc(_x2,_y2,(dist+2),(dist),180,240,TFT_BLACK,TFT_BLACK,false);
  wheel.drawArc(_x3,_y3,(dist+2),(dist),300,360,TFT_BLACK,TFT_BLACK,false);
  wheel.fillCircle(_x1,_y1,5,TFT_BLACK);
  wheel.fillCircle(_x2,_y2,5,TFT_BLACK);
  wheel.fillCircle(_x3,_y3,5,TFT_BLACK);

  int teeth = 40;
  float spacing = 360/float(teeth);
  
  int gearRadius = radius * 0.50;
  int gearRadius2 = gearRadius - 6;
  float gearPadding = degToRad(spacing * 0.40);

  wheel.fillCircle((wheel.width() >> 1), (wheel.height() >> 1), gearRadius + 9, colors[secondaryColorIndex]);
  wheel.drawArc((wheel.width() >> 1), (wheel.height() >> 1), gearRadius+10, gearRadius+9, 0,360, TFT_BLACK,TFT_BLACK);
  for (float i = 0; i < 360; i += spacing) {
    float angle = degToRad(i);
    float nextAngle = degToRad(i + spacing);

    //wheel.drawLine((wheel.width() >> 1) + cos(angle) * gearRadius,     (wheel.height() >> 1) + sin(angle) * gearRadius,       (wheel.width() >> 1) + cos(angle + gearPadding) * gearRadius2,     (wheel.height() >> 1) + sin(angle + gearPadding) * gearRadius2, TFT_BLACK);
    //wheel.drawLine((wheel.width() >> 1) + cos(nextAngle - gearPadding) * gearRadius2, (wheel.height() >> 1) + sin(nextAngle - gearPadding) * gearRadius2,       (wheel.width() >> 1) + cos(angle + gearPadding) * gearRadius2,     (wheel.height() >> 1) + sin(angle + gearPadding) * gearRadius2, TFT_BLACK);
    //wheel.drawLine((wheel.width() >> 1) + cos(nextAngle) * gearRadius, (wheel.height() >> 1) + sin(nextAngle) * gearRadius,   (wheel.width() >> 1) + cos(nextAngle - gearPadding) * gearRadius2, (wheel.height() >> 1) + sin(nextAngle - gearPadding) * gearRadius2, TFT_BLACK);
    wheel.fillTriangle((wheel.width() >> 1) + cos(angle) * gearRadius,     (wheel.height() >> 1) + sin(angle) * gearRadius,   (wheel.width() >> 1) + cos(angle + gearPadding) * gearRadius2,     (wheel.height() >> 1) + sin(angle + gearPadding) * gearRadius2,   (wheel.width() >> 1) + cos(angle - gearPadding) * gearRadius2,     (wheel.height() >> 1) + sin(angle - gearPadding) * gearRadius2, TFT_BLACK);

  }
  wheel.fillCircle((wheel.width() >> 1), (wheel.height() >> 1), gearRadius2 + 1, TFT_BLACK);
  //wheel.drawString(String(eAngle), 0, 30,2);
  //wheel.drawArc(wheel.width() >> 1, wheel.height() >> 1, radius, radius - 1, 0, 90, TFT_WHITE, TFT_WHITE, false);
}

void createWheel() {
  wheel.createSprite(230,230);
  wheel.setPivot(wheel.width() >> 1, wheel.height() >> 1);
  wheel.setSwapBytes(true);

  drawRotor();
}

float vtest = 0;

GaugeFace<Drawable> gauge(&canvas);

Dial dial(120, 120, 105, 0, 270, TFT_WHITE);
Needle ndl(dial, 120, 120, 90, TFT_RED);
DigitalGauge dgauge(dial, 120, 120, 115,105, TFT_RED);
BarGauge bar(100,80,40,80,TFT_RED);
BarScale bscale(100,80, 80, 90, true, TFT_WHITE);
Value val(120,120,vtest);
InfiniteDial idial(120,120,105,0,90,TFT_WHITE);
Turbo turb(120,120,80,40,0);

void setup() {
  Serial.begin(115200);

  dial.setTicks(8,15,3,7);
  dial.setValueRange(0,10);
  dial.setColors(TFT_WHITE);
  dial.setTickStyle(0);
  dial.setValuesPeriod(1);

  idial.setValueWidth(10);
  idial.setValuesPeriod(1);
  bar.setValueRange(0,10);

 /*
  gauge.addDrawable(&dial);
  gauge.addDrawable(&ndl);
  gauge.addDrawable(&dgauge);
  gauge.addDrawable(&bar);
  gauge.addDrawable(&bscale);
  gauge.addDrawable(&val);
  gauge.addDrawable(&idial);
  */
  gauge.addDrawable(&turb);

  //Wire.begin();
  myMPU9250.autoOffsets();
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);  

  lcd.init();
  lcd.setRotation(3);

  lcd.fillScreen(TFT_BLACK);
  canvas.createSprite(238,238);
  //lcd.setPivot(lcd.width() >> 1, lcd.height() >> 1);
  canvas.setPivot(canvas.width() >> 1, canvas.height() >> 1);
  lcd.startWrite();

  createWheel();


  /*
  vehicle.createSprite(vehicleSide_w, vehicleSide_h);
  vehicle.setPivot(vehicleSide_w >> 1, vehicleSide_h);
  vehicle.fillSprite(TFT_BLACK);
  vehicle.setSwapBytes(true);
  vehicle.pushImage(0,0,vehicleSide_w,vehicleSide_h,vehicle_side);
  */
 /*
  MenuItem item1("Settings");
  MenuItem item2("Data");
  MenuItem item3("Calibrate");
  MenuItem item4("Other thing idk");

  menu.addItem(item1);
  menu.addItem(item2);
  menu.addItem(item3);
  menu.addItem(item4);
  */
}

int t = 0;

float m = 0;

float angleMenu = 0;
int currentIndex = 0;
int numExtra = 3;
float angle = 75.0;

double gap = angle / numExtra;

float menuAngle = 0.0;
double menuTargetAngle = 0.0;

float wheelX = 0;

int mn = 0;

float wheelAngle = 0;
float wheelTargetAngle = 0;

void changePrimaryColor() {
  primaryColorIndex++;
  if (primaryColorIndex >= numColors) { primaryColorIndex = 0; }
  //drawWheel();
  drawRotor();
}

void changeSecondaryColor() {
  secondaryColorIndex++;
  if (secondaryColorIndex >= numColors) { secondaryColorIndex = 0; }
  //drawWheel();
  drawRotor();
}

void _settingsMenu();
void _languageMenu();
void _infoScreen();
void _mainMenu();
void _unitScreen();
void _customizationMenu();

const char* getString(int charIndex) {
  return languageList[languageIndex][charIndex];
}

std::vector<MenuItem> menuItems = {
  { 1, &_settingsMenu },      //Settings
  { 2, &_settingsMenu },      //Calibrate
  { 3, &_customizationMenu }, //Customization
  { 4, &_infoScreen }         //Info
};

std::vector<MenuItem> unitsMenu = {
  { 1, &_settingsMenu },      //Settings
  { 2, &_settingsMenu },      //Calibrate
  { 3, &_customizationMenu }, //Customization
  { 4, &_infoScreen }         //Info
};

std::vector<MenuItem> customizationMenu = {
  { 5, &_mainMenu },           //Back
  { 6, &changePrimaryColor },  //Primary Color
  { 7, &changeSecondaryColor } //Secondary Color
};

std::vector<MenuItem> settingsMenu = {
  { 5, &_mainMenu },     //Back
  { 8, &_languageMenu }, //Language
  { 9, &_unitScreen }    //Units
};

std::vector<MenuItem> selectedMenu = menuItems;

void subMenu(std::vector<MenuItem> menu) {
  mn = 1;
  wheelTargetAngle -= 360;
  selectedMenu = menu;
  menuTargetAngle = 0.0;
  currentIndex = 0;
}

void _settingsMenu() {
  subMenu(settingsMenu);
}

void _languageMenu() {
  languageIndex++;
  if (languageIndex == languageList.size()) { languageIndex = 0; }
}

void _customizationMenu() {
  subMenu(customizationMenu);
}

void _infoScreen() {

}

void _unitScreen() {

}

void _mainMenu() {
  mn = 0;
  wheelTargetAngle += 360;
  selectedMenu = menuItems;
  menuTargetAngle = 0.0;
  currentIndex = 0;
}


void drawMenu() {
  canvas.fillSprite(TFT_BLACK);

  canvas.setPivot(wheelX,120);

  switch(mn) {
    case 0:
      wheelX = lerp(wheelX, 0, 0.1);
      wheelTargetAngle++;
      break;
    case 1:
      wheelX = lerp(wheelX, 240, 0.1);
      wheelTargetAngle--;
      break;
  }


  wheel.pushRotated(&canvas, wheelAngle, TFT_BLACK);


  for (int i = -numExtra; i <= numExtra; i++) {
    int ind = ((i) + static_cast<int>(int(menuAngle / float(gap)))) % (static_cast<int>(menuItems.size())) ;

    if (ind < 0) { ind += menuItems.size(); } //min(numExtra*2 + 1, static_cast<int>(menu.size()) ); }
    float currentAngle = degToRad(gap * (i) - fmod(menuAngle, gap));

    uint16_t color;
    if (ind == currentIndex) {
      color = canvas.alphaBlend(min( ((abs(currentAngle)/degToRad(angle))*255), 255.0f),TFT_BLACK,TFT_WHITE);
    } else {
      color = canvas.alphaBlend(min( ((abs(currentAngle)/degToRad(angle))*255), 255.0f),TFT_BLACK,TFT_DARKGREY);
    }

    //if (i == 0) { color = TFT_RED; }

    canvas.setTextColor(color);
    canvas.setTextDatum(ML_DATUM);
    String item = getString(menuItems[ind].textIndex);
    canvas.drawString(item, wheelX + cos(currentAngle) * 120, 120 + sin(currentAngle) * 120, 2);
  }

  for (int i = -numExtra; i <= numExtra; i++) {
    int ind = ((i) + static_cast<int>(int(menuAngle / float(gap)))) % (static_cast<int>(selectedMenu.size())) ;

    if (ind < 0) { ind += selectedMenu.size(); } //min(numExtra*2 + 1, static_cast<int>(menu.size()) ); }
    float currentAngle = degToRad(gap * (i) - fmod(menuAngle, gap));

    uint16_t color;
    if (ind == currentIndex) {
      color = canvas.alphaBlend(min( ((abs(currentAngle)/degToRad(angle))*255), 255.0f),TFT_BLACK,TFT_WHITE);
    } else {
      color = canvas.alphaBlend(min( ((abs(currentAngle)/degToRad(angle))*255), 255.0f),TFT_BLACK,TFT_DARKGREY);
    }

    //if (i == 0) { color = TFT_RED; }

    canvas.setTextColor(color);
    canvas.setTextDatum(MR_DATUM);
    String item = getString(selectedMenu[ind].textIndex);
    canvas.drawString(item, wheelX - cos(currentAngle) * 120, 120 + sin(currentAngle) * 120, 2);
  }

  menuAngle = lerp(menuAngle, menuTargetAngle + 1, 0.1);
}

void loop() {
  button1.loop();
  button2.loop();
  /*


  xyzFloat angle = myMPU9250.getAngles();
  pitch = angle.x;
  roll = angle.y;
  yaw = angle.z;

  if (button1.isPressed() == 1) {
      menuSelect++;
      if (menuSelect >= sizeof(menu)) { menuSelect = 0; }
  }
  if (button2.getState() == 0) {
  }
  */
  if (button2.isPressed()) {
    
    menuTargetAngle += gap;
    currentIndex++;

    if (mn == 0) { wheelTargetAngle += gap; } else { wheelTargetAngle -= gap; }
  }
  if (button1.isPressed()) {
    selectedMenu[currentIndex].function();
  }

  if (currentIndex >= selectedMenu.size()) { currentIndex = 0; }

  canvas.setTextDatum(MC_DATUM);
  canvas.setTextColor(TFT_WHITE);

  gauge.draw();
  //drawMenu();
  canvas.drawString("UP",120,120,2);
  canvas.pushSprite(1, 1);
  t++;
  m = lerp(m,1,0.1f);

  wheelAngle = lerp(wheelAngle, wheelTargetAngle, 0.1);
}