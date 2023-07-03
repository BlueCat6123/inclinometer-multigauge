#include <Arduino.h>

#include <TFT_eSPI.h>
#include "vehicleSprites.h"

#include <Wire.h>

#include <MPU9250_WE.h>
#define MPU9250_ADDR 0x68
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

#include <ezButton.h>
ezButton button(13);

TFT_eSPI lcd = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&lcd);
TFT_eSprite vehicleSide   = TFT_eSprite(&lcd); // Sprite object for dial
//TFT_eSprite vehicleRear   = TFT_eSprite(&lcd); // Sprite object for dial

//Set the CS pins for each individual screen
int firstScreenCS = 17;
int secondScreenCS = 18;

int disp = 1;
int maxdisp = 4;

void createVehicleSide(){
  vehicleSide.createSprite(vehicleRear_w, vehicleRear_h);
  vehicleSide.setPivot(vehicleRear_w >> 1, 10);
  vehicleSide.fillSprite(TFT_BLACK);
}

void createVehicleRear(){
  /*
  vehicleRear.createSprite(vehicleRear_w, vehicleRear_h);
  vehicleRear.setPivot(vehicleRear_w >> 1, vehicleRear_h);
  vehicleRear.fillSprite(TFT_BLACK);
  */
}

char velocityUnits[] = "MPH";

void setup() {
  //Serial.begin(115200);
  Wire.begin();
  //if(!myMPU9250.init()) { Serial.println("MPU9250 does not respond"); }
  //else { Serial.println("MPU9250 is connected"); }
  //Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  //Serial.println("Done!");

  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);

  //INIT DUAL DISPLAYS
  /*
  pinMode(firstScreenCS, OUTPUT);
  digitalWrite(firstScreenCS, LOW);

  pinMode(secondScreenCS, OUTPUT);
  digitalWrite(secondScreenCS, LOW);
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setPivot(120,120);
  digitalWrite(firstScreenCS, HIGH);
  digitalWrite(secondScreenCS, HIGH);
  */
  
  lcd.init();
  lcd.fillScreen(TFT_BLACK);
  canvas.createSprite(236,236);
  canvas.fillSprite(TFT_BLACK);
  lcd.startWrite();

  lcd.setPivot(120,120);


  createVehicleSide();
  //createVehicleRear();
  
  vehicleSide.setSwapBytes(true);
  vehicleSide.pushImage(0,0,vehicleRear_w,vehicleRear_h,vehicle_rear);

  /*
  vehicleRear.setSwapBytes(true);
  vehicleRear.pushImage(0,0,vehicleRear_w,vehicleRear_h,vehicle_rear);
  */
}

float angle = 0;
float previousAngle = 0;
int t = 0;
int lineSpacing = 10;
float m = 0;
float degToRad(float d) { return ((d * 1000) / 57296); }
float altitude = 1000;
xyzFloat acceleration;
xyzFloat gValue;
xyzFloat a;
xyzFloat gyr;

float lerp(float a, float b, float t) { return (a + (b - a) * t); }

float linePosition = 0;

float spd = 0;



void speedometerHorizon() { // A really fancy retro speedometer with a moving ground
  
  float halfHeight = (canvas.height() >> 1) + (canvas.height() >> 1)*(1-m); // should really make this a global variable

  // Pseudo-3D horizon variables
  int horizonDensityVertical = 25; // Density variables can be changed to show more/less lines
  int horizonDensityHorizontal = 12;
  int horizonVAngle = 8; // Angle variables have nothing to do with degrees, just a general number to multiply by
  int horizonHAngle = 8;

  //canvas.fillRectVGradient(0,halfHeight,canvas.width(),canvas.height()-halfHeight,TFT_PURPLE,0x0010); 
  
  canvas.drawLine(0, halfHeight, canvas.width(), halfHeight, TFT_RED);
  
  // Display all the vertical lines
  for (float i = 0; i < canvas.width(); i += canvas.width() / horizonDensityVertical) {
    float pos = i; // Made this variable incase I wanted to add turning
    canvas.drawLine(pos, halfHeight, (pos - halfHeight) * horizonVAngle + halfHeight, canvas.height(), TFT_RED);
  }

  // Display all horizontal lines
  for (int i = 0; i < horizonDensityHorizontal; i++) {
    float pos = i + fmod(linePosition, 1); // Gives the moving effect
    float maxValue = pow((horizonDensityHorizontal), horizonHAngle); // Maximum value to determine window

    int y = int(halfHeight + pow(pos, horizonHAngle) * (halfHeight / maxValue)); // Exponential equation gives Pseudo-3D effect
    
    canvas.drawLine(0, y, canvas.width(), y, TFT_RED);
  }

  // Speedometer reading
  canvas.setTextColor(TFT_WHITE);
  canvas.setTextDatum(MC_DATUM);
  canvas.drawString("88", canvas.width() >> 1, 50*m,6); //Currently just using a placeholder
  canvas.drawString(velocityUnits, canvas.width() >> 1, 80*m,2);  

  // Draw vehicle
  vehicleSide.setPivot(vehicleRear_w >> 1, 10-120*(1-m));
  vehicleSide.pushRotated(&canvas, int16_t(angle), uint32_t (0x39C7));
}



void InclinometerPitch() {
  //UPDATE
  angle = a.y;
  
  //if (int(angle) != int(previousAngle)) {

  //PRINT ANGLE
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  String str = String(int(-angle));
  canvas.drawString(str, 120-(str.length()*12), 200, 6);

  //draw angle difference indicator
  canvas.drawLine(0,120,240,120,TFT_RED); //Zero Degree Line
  for (int i=0; i<(240/lineSpacing); i++) { 
    canvas.drawLine((i*lineSpacing),  120 - (tan(degToRad(angle))*(120-i*lineSpacing)),  (i*lineSpacing),  120,  TFT_RED);
  }

  // Draw car
  vehicleSide.pushRotated(&canvas, int16_t(angle), uint32_t (0x0000));
    
  canvas.drawLine(120*cos(degToRad(angle))+120, 120*sin(degToRad(angle))+120, -120*cos(degToRad(angle))+120, -120*sin(degToRad(angle))+120, TFT_WHITE);
}

/*
void turningAngle() {
  float turnAngle = atan((acceleration.y + 0.1f) / acceleration.x) * (180.0f / 3.1415926f) - 90.0f;
  createVehicleTop(turnAngle);
  
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  String str = String(int(turnAngle));
  canvas.drawString(str, 160, 120, 6);
  
  delay(10);

  canvas.setTextColor(TFT_BLACK, TFT_BLACK);
  str = String(int(turnAngle));
  canvas.drawString(str, 160, 120, 6);
  
  coverTop.fillScreen(TFT_BLACK);
  coverTop.pushRotated(-turnAngle);
}
*/

void GForceGauge() {
  canvas.drawCircle(120, 120, 100, TFT_WHITE);
  canvas.drawCircle(120, 120, 50, TFT_WHITE);
  canvas.fillCircle(120 + int(acceleration.x*50), 120 + int(acceleration.y*50), 10, TFT_RED);
  delay(30);
  canvas.fillCircle(120 + int(acceleration.x*50), 120 + int(acceleration.y*50), 10, TFT_BLACK);
}


void drawGauge(float x, float y, float r, float w, float anglestart, float angleend, int divisions, int subdivisions, uint16_t c) {
  float angletotal = angleend - anglestart;

  if (angletotal < 360) {
    float an = degToRad(angletotal)*m + degToRad(anglestart);
    canvas.drawLine(x+r*cos(an), y+r*sin(an), x+(r-w)*cos(an), y+(r-w)*sin(an),c);
  }

  for (int i = 0; i < divisions; i++) {
    float an = degToRad(anglestart + angletotal*(i/float(divisions))*m);
    canvas.drawLine(x+r*cos(an), y+r*sin(an), x+(r-w)*cos(an), y+(r-w)*sin(an),c);
    for (int j = 1; j < subdivisions; j++) {
      float an2 = an + degToRad((angletotal/float(divisions))/float(subdivisions))*j*m;
      float r2 = r - w / 3;
      canvas.drawLine(x+r*cos(an2), y+r*sin(an2), x+r2*cos(an2), y+r2*sin(an2),c);
    }
  }
}

void drawNeedle(float  x,float  y,float  r, uint16_t c) {
  float a = 2*sin(t*0.0425)+1*sin(t*0.113)+2.5;
  float w = 10;
  canvas.fillTriangle(x + r*cos(a)*m, y + r*sin(a)*m,
                   x - w*sin(a), y + w*cos(a),
                   x + w*sin(a), y - w*cos(a), c);

  float cr = 15;
  canvas.fillTriangle(x-cr,y,x,y+cr,x+cr,y,TFT_BLACK);
  canvas.fillTriangle(x-cr,y,x,y-cr,x+cr,y,TFT_BLACK);  

  canvas.drawLine(x,y-cr,x+cr,y,c);
  canvas.drawLine(x+cr,y,x,y+cr,c);
  canvas.drawLine(x,y-cr,x-cr,y,c);
  canvas.drawLine(x-cr,y,x,y+cr,c);
}

void drawAltimeter(float x,float y) {
  float angleoffset = (altitude/1000)*(360/8);
  drawGauge(canvas.width() >> 1, canvas.height() >> 1,110,30,angleoffset,360+angleoffset,8,4,TFT_WHITE);

  float cr = 15;
  float bt = 5;
  float v = tan(degToRad(45))*(cr-bt); //stupid trig identity
  uint16_t c = TFT_RED;
  canvas.drawLine(x, y-cr, x+cr, y, c);
  canvas.drawLine(x+cr, y, x, y+cr, c);
  canvas.drawLine(x, y+cr, x-v, y+bt,c);
  canvas.drawLine(x-v,  y-bt, x,    y-cr, c);
  canvas.drawLine(x-v,  y+bt, x-75, y+bt, c);
  canvas.drawLine(x-75, y+bt, x-75, y-bt, c);
  canvas.drawLine(x-75, y-bt, x-v,  y-bt, c);
}



void drawUpdate() {
  canvas.fillSprite(TFT_BLACK); // Start with a black screen

  speedometerHorizon();
  //drawGauge(canvas.width() >> 1, canvas.height() >> 1, 110,10,45,315,6,4,TFT_WHITE);
  //drawNeedle(canvas.width() >> 1, canvas.height() >> 1, 100, TFT_BLUE);
  //drawAltimeter(canvas.width() >> 1, canvas.height() >> 1);
  //InclinometerPitch();
  
  canvas.pushSprite(2, 2);
  /* FOR SCREEN SWITCHING
 switch(disp) {
  case 1:
    GForceGauge();
    break;
  case 2:
    InclinometerPitch(false,false);
    InclinometerPitch(false,true);
    break;
  case 3:
    InclinometerPitch(true,false);
    InclinometerPitch(true,true);
    break;
  case 4:
    drawAltimeter(120,120);
    break;
  }
  */
}

void loop() {
  button.loop();

  if (button.isPressed()) {
    //Serial.println("button press!");
  }
  if (button.isPressed()) {
    canvas.fillScreen(TFT_BLACK);
    disp++;
    if (disp > maxdisp) { disp = 1; }
  }
  
  gValue = myMPU9250.getGValues();
  gyr = myMPU9250.getGyrValues();
  a = myMPU9250.getAngles();
  acceleration = myMPU9250.getGValues();


  drawUpdate();
  /*
  digitalWrite(secondScreenCS, LOW);
  InclinometerPitch(false,false);
  digitalWrite(secondScreenCS, HIGH);
  digitalWrite(firstScreenCS, LOW);
  InclinometerPitch(true,false);
  digitalWrite(firstScreenCS, HIGH);
  */
  
  t++;
  m = lerp(m, 1, 0.1);
  altitude = 1000 + sin(t*0.001)*850;
  linePosition += 0.1;
  
}