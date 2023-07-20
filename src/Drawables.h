#include <TFT_eSPI.h>
#include <Arduino.h>
#include <vector>
#include <utils.h>

/////////////////////
// DRAWABLES CLASS //
/////////////////////

/*
This is the base class that new gauge elements are made off of. To create a new element, create a class that
derives from 'Drawable'. 
*/

class Drawable {
    public:
        virtual void draw(TFT_eSprite* canvas) = 0;
};


////////////////////////
// REFERENCED CLASSES //
////////////////////////

class Dial : public Drawable {

    /* A highly customizable traditional gauge design */

    private:
        /*  GENERIC VARIABLES  */
        int radius;
        int x;
        int y;

        int angleStart;
        int angleEnd;
        int angleTotal;

        uint16_t color;

        /*  TICK VARIABLES  */
        int majorTickCount;
        int majorTickLength; 
        float majorTickThickness; // Thickness of major tick marks, set in degrees rotation
        int majorTickMode; // Modes 0-2 display different styles

        int minorTickCount;
        int minorTickLength;
        uint16_t minorTickColor;


        /*  VALUE VARIABLES  */
        int minValue;
        int maxValue;

        int valuesPeriod;     // The period at which values numbers are drawn (i.e.  1 = every tick    2 = every other tick) leave undefined for no values
        uint16_t valuesColor;

        /*  OTHER VARIABLES  */
        int arcThickness; // Thickness of arc drawn on dial, leave undefined for no arc

    public:
        Dial(int x, int y, int radius, int angleStart, int angleEnd, uint16_t color)
            : x(x), y(y), radius(radius), angleStart(angleStart), angleEnd(angleEnd), color(color)
        {
            minorTickColor = color;
            valuesColor = color;

            angleTotal = angleEnd - angleStart;
            arcThickness = 0;
        }

        void draw(TFT_eSprite* canvas) override {
            float mtc = majorTickCount - 1;
            int padding = 5;

            canvas->setTextDatum(MC_DATUM);
            canvas->setTextFont(2);
            canvas->setTextColor(valuesColor);

            int majorTickRadius = (radius - majorTickLength);
            int minorTickRadius = (radius - minorTickLength);

            for (int i = 0; i < majorTickCount; i++) {
                float angle = degToRad(angleStart + angleTotal * (i / mtc));
                double cosAngle, sinAngle;
                sincos(angle, &sinAngle, &cosAngle); // Supposed to be more efficient, I don't really know nor care ¯\_(ツ)_/¯

                // Draw the major lines
                if (majorTickThickness == 0) {
                    canvas->drawLine(x + radius * cosAngle, y + radius * sinAngle, x + majorTickRadius * cosAngle, y + majorTickRadius * sinAngle, color);
                } else {
                    float angleOffset = degToRad(majorTickThickness); // In degrees
                    double cosAngleMinusOffset, sinAngleMinusOffset, cosAnglePlusOffset, sinAnglePlusOffset;
                    sincos(angle - angleOffset, &sinAngleMinusOffset, &cosAngleMinusOffset);
                    sincos(angle + angleOffset, &sinAnglePlusOffset, &cosAnglePlusOffset);

                    switch (majorTickMode) {
                        case 0: // Unfilled rectangle tick
                            canvas->fillTriangle(x + radius * cosAngleMinusOffset, y + radius * sinAngleMinusOffset,    x + majorTickRadius * cosAngleMinusOffset, y + majorTickRadius * sinAngleMinusOffset,    x + radius * cosAnglePlusOffset, y + radius * sinAnglePlusOffset, TFT_WHITE);
                            canvas->fillTriangle(x + majorTickRadius * cosAngleMinusOffset, y + majorTickRadius * sinAngleMinusOffset,    x + radius * cosAnglePlusOffset, y + radius * sinAnglePlusOffset,    x + majorTickRadius * cosAnglePlusOffset, y + majorTickRadius * sinAnglePlusOffset, TFT_WHITE);
                            break;
                        case 1: // Filled rectangle tick
                            canvas->drawLine(x + radius * cosAngleMinusOffset, y + radius * sinAngleMinusOffset, x + majorTickRadius * cosAngleMinusOffset, y + majorTickRadius * sinAngleMinusOffset, color); // counter-clockwise line
                            canvas->drawLine(x + radius * cosAnglePlusOffset, y + radius * sinAnglePlusOffset, x + majorTickRadius * cosAnglePlusOffset, y + majorTickRadius * sin(angle+angleOffset), color); // clockwise line
                            canvas->drawLine(x + radius * cosAngleMinusOffset, y + radius * sinAngleMinusOffset, x + (radius) * cosAnglePlusOffset, y + (radius) * sinAnglePlusOffset, color);                                     // outside line
                            canvas->drawLine(x + majorTickRadius * cosAnglePlusOffset, y + majorTickRadius * sinAnglePlusOffset, x + majorTickRadius * cosAngleMinusOffset, y + majorTickRadius * sinAngleMinusOffset, color); // inside line
                            break;
                        case 2: // filled triangle tick
                            canvas->fillTriangle(x + radius * cosAngleMinusOffset, y + radius * sinAngleMinusOffset,    x + radius * cosAnglePlusOffset, y + radius * sinAnglePlusOffset,    x + majorTickRadius * cos(angle), y + majorTickRadius * sin(angle), TFT_WHITE);
                            break;
                    }
                }
                
                // Draw the value numbers
                if ((valuesPeriod > 0) && ((i % valuesPeriod) == 0)) {
                    int value = lerp(minValue, maxValue, (float(i) / mtc));
                    int stringWidth = (canvas->textWidth(String(value), 2) >> 1);
                    int stringHeight = (canvas->fontHeight(2) >> 1);
                    int length = lerp(stringWidth, stringHeight, abs(sin(angle))) + padding;
                    canvas->drawString(String(value), x + (majorTickRadius - length) * cos(angle), y + (majorTickRadius - length) * sin(angle), 2);
                }

                // for loop to draw all minor ticks in between each major ticks
                if (i != (mtc)) {
                    float deltaAngle = degToRad((angleTotal / mtc) / float(minorTickCount + 1));

                    for (int j = 1; j <= minorTickCount; j++) {
                        double cosAngle2, sinAngle2;
                        sincos(angle + deltaAngle * j, &sinAngle2, &cosAngle2);
                        canvas->drawLine(x + radius * cosAngle2, y + radius * sinAngle2, x + minorTickRadius * cosAngle2, y + minorTickRadius * sinAngle2, minorTickColor);
                    }
                }
            }

            if (arcThickness>0) { canvas->drawArc(x, y, (radius+1), (radius+1)-arcThickness, angleStart - 90, angleEnd - 90, TFT_WHITE, TFT_WHITE, false); }
        }

        void setRadius(int r) { radius = r; }

        int getRadius() { return radius; }

        void setPosition(int _x, int _y) { x = _x; y = _y; }

        int getX() { return x; }
        int getY() { return y; }

        void setTicks(int _majorTickCount, int _majorTickLength, int _minorTickCount, int _minorTickLength) {
            majorTickCount = _majorTickCount;
            majorTickLength = _majorTickLength;
            minorTickCount = _minorTickCount;
            minorTickLength = _minorTickLength;
        }

        // Sets all colors to defined color
        void setColors(uint16_t _color) { color = _color; valuesColor = color; minorTickColor = color; }

        // Sets each color to defined colors
        void setColors(uint16_t _color, uint16_t _valuesColor, uint16_t _minorTickColor) { color = _color; valuesColor = _valuesColor; minorTickColor = _minorTickColor; }

        void setTickStyle(int _mode) { majorTickMode = _mode; }

        void setValueRange(int _minValue, int _maxValue) 
        { 
            minValue = _minValue;
            maxValue = _maxValue;
        }

        void setValuesPeriod(int _valuesPeriod) { valuesPeriod = _valuesPeriod; } 

        void setAngles(int _angleStart, int _angleEnd) { angleStart = _angleStart; angleEnd = _angleEnd; angleTotal = angleEnd - angleStart; }

        int& getMinValue()    { return minValue; }
        int& getMaxValue()    { return maxValue; }
        int& getAngleStart()  { return angleStart; }
        int& getAngleEnd()    { return angleEnd; }
};

class InfiniteDial : public Drawable {

    /* Similar to the normal dial, however the dial moves through infinite values */
    private:
        /*  GENERIC VARIABLES  */
        int radius;
        int x;
        int y;

        int angleStart;
        int angleEnd;
        int angleTotal;

        uint16_t color;
        uint16_t valuesColor;
        uint16_t minorTickColor;

        /*  TICK VARIABLES  */
        int majorTickCount;
        int majorTickLength; 
        float majorTickThickness; // Thickness of major tick marks, set in degrees rotation
        int majorTickMode; // Modes 0-2 display different styles

        int minorTickCount;
        int minorTickLength;

        float fadeRangeDegrees;


        /*  VALUE VARIABLES  */
        float value;
        float valueWidth;

        int valuesPeriod;     // The period at which values numbers are drawn (i.e.  1 = every tick    2 = every other tick) leave undefined for no values

        /*  OTHER VARIABLES  */
        int arcThickness; // Thickness of arc drawn on dial, leave undefined for no arc

    public:
        InfiniteDial(int _x, int _y, int _radius, int _angleStart, int _angleEnd, uint16_t _color) : x(_x), y(_y), radius(_radius), angleStart(_angleStart), angleEnd(_angleEnd), color(_color)
        {
            value = 0;
            valueWidth = 50;
            minorTickColor = _color;
            valuesColor = _color;
            angleTotal = angleEnd - angleStart;
            
            fadeRangeDegrees = 35;
        }

        void draw(TFT_eSprite* canvas) override {
            float a1 = degToRad(angleStart);
            float a2 = degToRad(angleEnd);
            float a3 = degToRad(angleTotal);
            int padding = 5;

            canvas->setTextDatum(MC_DATUM);
            canvas->setTextFont(2);

            float mtc = majorTickCount - 1;

            float _value = value + (1-(majorTickCount % 2)) * (valueWidth/2); // Fixes a bug where an even number of major ticks offsets the gauge from starting at 0
            int majorTickRadius = (radius - majorTickLength);
            int minorTickRadius = (radius - minorTickLength);

            float angleOffset = (fmod(_value, valueWidth) / valueWidth) * ((a3 / float(mtc)) );
            for (int i = 0; i < majorTickCount; i++) {
                float angle = a1 + a3 * (i / mtc) - angleOffset;

                if ((angle >= a1) && (angle <= a2)) {
                    double cosAngle, sinAngle;
                    sincos(angle, &sinAngle, &cosAngle); // Supposed to be more efficient, I don't really know nor care ¯\_(ツ)_/¯

                    uint16_t _color;
                    float blendingFactor;
                    float de = degToRad(fadeRangeDegrees);
                    if (angle <= a1 + de) {
                        _color = canvas->alphaBlend(int(((angle - a1) / de) * 255), TFT_WHITE, TFT_BLACK);
                    } else if (angle >= a2 - de) {
                        _color = canvas->alphaBlend(int(((a2 - angle)/de) * 255), TFT_WHITE, TFT_BLACK);
                    } else{
                        _color = TFT_WHITE;
                    }

                    canvas->drawLine(x + radius * cosAngle, y + radius * sinAngle, x + majorTickRadius * cosAngle, y + majorTickRadius * sinAngle, _color);

                    if ((valuesPeriod > 0) && ((i % valuesPeriod) == 0)) {
                        int _displayValue = int(value - fmod(value + (1-(majorTickCount % 2)) * (valueWidth/2), valueWidth) + valueWidth * i - mtc * valueWidth / 2); // Calculates the value to display on the tick mark
                        int stringWidth = (canvas->textWidth(String(_displayValue), 2) >> 1);
                        int stringHeight = (canvas->fontHeight(2) >> 1);
                        int length = lerp(stringWidth, stringHeight, abs(sin(angle))) + padding;
                        canvas->setTextColor(_color);
                        canvas->drawString(String(_displayValue), x + (majorTickRadius - length) * cos(angle), y + (majorTickRadius - length) * sin(angle), 2);
                    }

                }

                if (i != majorTickCount) {
                    float deltaAngle = (a3 / mtc) / float(minorTickCount + 1);

                    for (int j = 1; j <= minorTickCount; j++) {
                        float subAngle =  angle + deltaAngle * j;
                        if ((subAngle >= a1) && (subAngle <= a2)) {
                            uint16_t _color;
                            float blendingFactor;
                            float de = degToRad(fadeRangeDegrees);
                            if (subAngle <= a1 + de) {
                                _color = canvas->alphaBlend(int(((subAngle - a1) / de) * 255), TFT_WHITE, TFT_BLACK);
                            } else if (subAngle >= a2 - de) {
                                _color = canvas->alphaBlend(int(((a2 - subAngle)/de) * 255), TFT_WHITE, TFT_BLACK);
                            } else{
                                _color = TFT_WHITE;
                            }
                            double cosAngle2, sinAngle2;
                            sincos(subAngle, &sinAngle2, &cosAngle2);
                            canvas->drawLine(x + radius * cosAngle2, y + radius * sinAngle2, x + minorTickRadius * cosAngle2, y + minorTickRadius * sinAngle2, _color);
                        }
                    }
                }
            }
            canvas->setTextColor(TFT_WHITE);
            //canvas->drawString(String(int(value)),canvas->width() >> 1, 80, 6);
        }
        void setTicks(int _majorTickCount, int _majorTickLength, int _minorTickCount, int _minorTickLength) {
            majorTickCount = _majorTickCount;
            majorTickLength = _majorTickLength;
            minorTickCount = _minorTickCount;
            minorTickLength = _minorTickLength;
        }

        // Sets all colors to defined color
        void setColors(uint16_t _color) { color = _color; valuesColor = color; minorTickColor = color; }

        // Sets each color to defined colors
        void setColors(uint16_t _color, uint16_t _valuesColor, uint16_t _minorTickColor) { color = _color; valuesColor = _valuesColor; minorTickColor = _minorTickColor; }

        void setValueWidth(float width) { valueWidth = width; }

        void setValuesPeriod(int _valuesPeriod) { valuesPeriod = _valuesPeriod; } 

        void setAngles(int _angleStart, int _angleEnd) { angleStart = _angleStart; angleEnd = _angleEnd; angleTotal = angleEnd - angleStart; }

        void setValue(float _value) { value = _value; }
};

class ShiftLight : public Drawable {
    private:
        int x;
        int y;
        int radius;
        int& rpmValue;
        int rpmAlert;
        uint16_t color;

    public:
        ShiftLight(int _x, int _y, int _radius, int& _rpmValue, int _rpmAlert, uint16_t _color) : x(_x), y(_y), radius(_radius), rpmValue(_rpmValue), rpmAlert(_rpmAlert), color(_color) {}

        void draw(TFT_eSprite* canvas) override 
        {
            if (rpmValue > rpmAlert) {
                canvas->fillCircle(x, y, radius, color);
            } else {
                canvas->drawCircle(x, y, radius, color);
                canvas->setTextDatum(MC_DATUM);
                canvas->setTextColor(color);
                canvas->drawString("Shift", x, y, 2);
            }
        }

        void setColor(uint16_t _color) { color = _color; }
};

class BarScale : public Drawable {
    private:
        int x;
        int y;
        int length;
        bool flip;

        int angle;

        /*  VALUE VARIABLES  */
        int minValue;
        int maxValue;

        uint16_t color;

        /*  TICK VARIABLES  */
        int majorTickCount;
        int majorTickLength; 

        int minorTickCount;
        int minorTickLength;        
    public: 
        BarScale(int _x, int _y, int _length, int _angle, bool _flip, uint16_t _color) : x(_x), y(_y), length(_length), flip(_flip), color(_color) {
            majorTickCount = 4;
            majorTickLength = 10;
            minorTickCount = 3;
            minorTickLength = 5;

            angle = _angle;
            minValue = 0; maxValue = 100;

        }

        void draw(TFT_eSprite* canvas) override 
        {
            float mtc = majorTickCount - 1;

            float tickSpacing = abs(length)/mtc;
            float minorTickSpacing = tickSpacing / float(minorTickCount+1);

            float _angle = degToRad(angle);
            float _angle2 = degToRad(angle+90);

            float dir = 1;

            if (flip) { dir = -1; }

            for (int i = 0; i < majorTickCount; i++) {
                int _x, _y, _x2, _y2;

                _x = x + cos(_angle) * tickSpacing * i;
                _y = y + sin(_angle) * tickSpacing * i;

                _x2 = _x + cos(_angle2) * majorTickLength * dir;
                _y2 = _y + sin(_angle2) * majorTickLength * dir;

                canvas->drawLine(_x, _y, _x2, _y2, color);

                int _displayValue = lerp(minValue, maxValue, float(i)/mtc); // Calculates the value to display on the tick mark
                int stringWidth = (canvas->textWidth(String(_displayValue), 2) >> 1);
                int stringHeight = (canvas->fontHeight(2) >> 1);
                int stringLength = lerp(stringWidth, stringHeight, abs(sin(_angle2))) + 5;

                canvas->setTextColor(color);
                canvas->drawString(String(_displayValue), _x2 + cos(_angle2) * stringLength * dir, _y2 + sin(_angle2) * stringLength * dir, 2);

                if (i != (mtc)) {
                    for (int j = 1; j <= minorTickCount; j++) {
                        int _x3, _y3, _x4, _y4;

                        _x3 = _x + cos(_angle) * minorTickSpacing * j;
                        _y3 = _y + sin(_angle) * minorTickSpacing * j;
                        _x4 = _x3 + cos(_angle2) * minorTickLength * dir;
                        _y4 = _y3 + sin(_angle2) * minorTickLength * dir;
                        canvas->drawLine(_x3, _y3, _x4, _y4, color);
                    }
                }
                

            }

            canvas->drawLine(x, y, x + cos(_angle) * length, y + sin(_angle) * length, color);
        }
};

class BarGauge : public Drawable {
    private:
        int x;
        int y;
        int width;
        int height;

        int segmentCount;
        int segmentSpacing;

        int minValue;
        int maxValue;
        float value;

        uint16_t color;
    public:
        BarGauge(int _x, int _y, int _width, int _height, uint16_t color) : x(_x), y(_y), width(_width), height(_height)
        {
            segmentCount = 8;
            segmentSpacing = 2;
            minValue = 0;
            maxValue = 100;
        }

        void draw(TFT_eSprite* canvas) override 
        {  
            float segmentLength;
            segmentLength = int(float(height)/float(segmentCount));

            for (int i = 0; i < segmentCount; i++) {
                float _value = lerp(minValue, maxValue, float(i) / float(segmentCount));
                float _x, _y, _w, _h;

                _x = x;
                _w = width;
                _y = y - segmentLength * i;
                _h = segmentLength - segmentSpacing;
                if (value > _value) {
                    canvas->fillRect(_x, _y - _h, _w, _h, TFT_RED);
                } else {
                    canvas->drawRect(_x, _y - _h, _w, _h,TFT_RED);  
                }
            }
        }

        void setValueRange(float _minValue, float _maxValue) 
        { 
            minValue = _minValue;
            maxValue = _maxValue;
        }

        void setValue(float _value) { value = _value; }
};

class DigitalGauge : public Drawable {
    private:
        int x;
        int y;
        int radius;
        int innerRadius;

        int& angleStart;
        int& angleEnd;

        int& minValue;
        int& maxValue;

        int segmentCount;
        int segmentSpacing; // In degrees
        uint16_t color;


        float value;

    public:
        DigitalGauge(Dial& _dial, int _x, int _y, int _radius, int _innerRadius, uint16_t _color) : x(_x), y(_y), radius(_radius), innerRadius(_innerRadius), color(_color), angleStart(_dial.getAngleStart()), angleEnd(_dial.getAngleEnd()), minValue(_dial.getMinValue()), maxValue(_dial.getMaxValue()) { 
            minValue = 0; maxValue = 100; 
            segmentCount = 20;
            segmentSpacing = 2;
        }

        void draw(TFT_eSprite* canvas) override 
        {
            float angleTotal = angleEnd - angleStart;
            float segmentLength = angleTotal / float(segmentCount);
            float angleCutoff = lerp(angleStart, angleEnd, (value/maxValue));
            for (int i = 0; i < segmentCount; i++) {
                float _angle1 = angleStart + (segmentLength)*i;

                if (_angle1 < angleCutoff) {
                float _angle2 = min(angleStart + (segmentLength)*(i+1) - segmentSpacing,angleCutoff);
                canvas->drawArc(x, y, radius, innerRadius, _angle1-90, _angle2-90, color, color, false);
                }
            }
        }

        void setAngles(int _angleStart, int _angleEnd) {
            angleStart = _angleStart;
            angleEnd = _angleEnd;
        }
        void setValue(float _value) {
            value = _value;
        }
        void setColor(uint16_t _color) { color = _color; }
};

class Value : public Drawable {
    private:
        int x;
        int y;
        int value;

    public:
        Value(int _x, int _y, int _value) : x(_x), y(_y), value(_value) {}

        void draw(TFT_eSprite* canvas) override {

            int maxTextWidth = canvas->textWidth("8000", 2);
            int textHeight   = canvas->fontHeight(2);

            int padding = 4;
            float angle = atan(float(textHeight/2 + padding) / float(maxTextWidth/2 + padding));
            int c = sqrt(pow((maxTextWidth/2 + padding), 2) + pow((textHeight/2 + padding), 2));
            int _x, _y, _w, _h;

            _x = x - maxTextWidth/2 - padding;
            _y = y - textHeight/2 - padding;
            _w = maxTextWidth + padding * 2;
            _h = textHeight + padding * 2;

            canvas->setTextColor(TFT_WHITE);
            canvas->setTextDatum(MC_DATUM);
            //canvas->fillCircle(x, y, 35,TFT_BLACK);
            //canvas->drawCircle(x, y, 35,TFT_WHITE);
            canvas->fillRect(_x,_y,_w,_h,TFT_BLACK);
            //canvas->drawRect(_x,_y,_w,_h,TFT_WHITE);
            canvas->drawArc(x, y, c+1, c, -angle * 57.2958 + 90, angle * 57.2958 + 90, TFT_WHITE, TFT_WHITE, false);
            canvas->drawArc(x, y, c+1, c, -angle * 57.2958 + 270, angle * 57.2958 + 270, TFT_WHITE, TFT_WHITE, false);
            canvas->drawString(String(value), x, y, 2);
            
            canvas->drawLine(_x,_y,_x+_w,_y,TFT_WHITE);
            canvas->drawLine(_x,_y+_h,_x+_w,_y+_h,TFT_WHITE);

        }

    void setValue(float _value) {
        value = _value;
    }
};

class Needle : public Drawable {
    private:
        int x;
        int y;
        int radius;

        int& angleStart;
        int& angleEnd;

        int& minValue;
        int& maxValue;

        uint16_t color;

        float value;


    public:
        Needle(Dial& _dial, int _x, int _y, int _radius, uint16_t _color) : x(_x), y(_y), radius(_radius), color(_color), angleStart(_dial.getAngleStart()), angleEnd(_dial.getAngleEnd()), minValue(_dial.getMinValue()), maxValue(_dial.getMaxValue()) { minValue = 0; maxValue = 100; }

        void draw(TFT_eSprite* canvas) override {
            if (maxValue - minValue != 0) {
            float a = degToRad( lerp(angleStart, angleEnd, (float(value - minValue) / float(maxValue - minValue))));
            float w = 10;
            canvas->fillTriangle(x + radius*cos(a), y + radius*sin(a), x - w*sin(a), y + w*cos(a), x + w*sin(a), y - w*cos(a), color);

            float cr = 15;
            canvas->fillTriangle(x-cr,y,x,y+cr,x+cr,y,TFT_BLACK);
            canvas->fillTriangle(x-cr,y,x,y-cr,x+cr,y,TFT_BLACK);  

            canvas->drawLine(x,y-cr,x+cr,y,color);
            canvas->drawLine(x+cr,y,x,y+cr,color);
            canvas->drawLine(x,y-cr,x-cr,y,color);
            canvas->drawLine(x-cr,y,x,y+cr,color);
            }
        }

        void setAngles(int _angleStart, int _angleEnd) {
            angleStart = _angleStart;
            angleEnd = _angleEnd;
        }
        void setValue(float _value) {
            value = _value;
        }
};

class Incline : public Drawable {
    private:
        float& angle;
        TFT_eSprite& sprite;

    public:
        Incline(float& _angle, TFT_eSprite& _sprite) : angle(_angle), sprite(_sprite) {}

        void draw(TFT_eSprite* canvas) override {
            int lineSpacing = 10;

            canvas->drawLine(120*cos(degToRad(angle))+120, 120*sin(degToRad(angle))+120, -120*cos(degToRad(angle))+120, -120*sin(degToRad(angle))+120, TFT_RED);
            
            canvas->drawLine(0,120,240,120,TFT_RED); //Zero Degree Line
            for (int i=0; i<(240/lineSpacing); i++) { 
                canvas->drawLine((i*lineSpacing),  120 - (tan(degToRad(angle))*(120-i*lineSpacing)),  (i*lineSpacing),  120,  TFT_RED);
            }

            sprite.pushRotated(canvas, int16_t(angle), uint32_t (0x39C7));
            canvas->drawLine(0,120,240,120,TFT_WHITE); //Zero Degree Line

        }
};

class Turbo : public Drawable {
    private:
        int x;
        int y;
        float radius;
        float turbineRadius;

        float angle;

    public:
        Turbo(int _x, int _y, float _radius, float _turbineRadius, float _angle) : x(_x), y(_y), radius(_radius), turbineRadius(_turbineRadius), angle(_angle) { ; }

        void draw(TFT_eSprite* canvas) override {
            canvas->drawCircle(x,y,turbineRadius, TFT_WHITE);
            float ang = asin(turbineRadius/radius) * 57.2958;
            canvas->drawArc(x,y,radius,radius-1,360 - (90 + ang),360,TFT_WHITE,TFT_WHITE,false);
            canvas->drawArc(x,y,radius,radius-1,0,180,TFT_WHITE,TFT_WHITE,false);

            //canvas->drawRect(x, y-radius, 80,radius-turbineRadius,TFT_WHITE);
            float _angle = degToRad(angle - 90);
            float _angle2 = degToRad(angle);
            canvas->drawLine(x + cos(_angle)*radius, y + sin(_angle)*radius,   x + cos(_angle)*radius + cos(_angle2)*80, y + sin(_angle)*radius + sin(_angle2)*80,TFT_WHITE);
            canvas->drawLine(x + cos(_angle)*turbineRadius, y + sin(_angle)*turbineRadius,   x + cos(_angle)*turbineRadius + cos(_angle2)*80, y + sin(_angle)*turbineRadius + sin(_angle2)*80,TFT_WHITE);
        }
};

class Horizon : public Drawable {
    private:
        int horizonDensityVertical = 25; // Density variables can be changed to show more/less lines
        int horizonDensityHorizontal = 12;
        int horizonVAngle = 8; // Angle variables have nothing to do with degrees, just a general number to multiply by
        int horizonHAngle = 8;

        float zVelMultiplier;
        float xVelMultiplier;

        float zVelocity;
        float xVelocity; 

        float zPosition;
        float xPosition;
        
    public:
        Horizon(int _horizonHAngle) : horizonHAngle(_horizonHAngle) {
            zPosition = 0; 
            xPosition = 0; 
            zVelocity = 0; 
            xVelocity = 0; 
            zVelMultiplier = 0.001;
            xVelMultiplier = 0.01;
        }

        void draw(TFT_eSprite* canvas) override {

            float width = canvas->width();
            float height = canvas->height();
            float halfHeight = canvas->height() >> 1;//(canvas->height() >> 1) + (canvas->height() >> 1); // should really make this a global variable

            canvas->drawLine(0, halfHeight, width, halfHeight, TFT_RED);
            
            // Display all the vertical lines
            for (float i = 0; i < width; i += width / float(horizonDensityVertical)) {
                float pos = i + fmod(xPosition, 1) * width / float(horizonDensityVertical); // Made this variable incase I wanted to add turning
                canvas->drawLine(pos, halfHeight, (pos - halfHeight) * horizonVAngle + halfHeight, height, TFT_RED);
            }

            // Display all horizontal lines
            for (int i = 0; i < horizonDensityHorizontal; i++) {
                float pos = i + fmod(zPosition, 1); // Gives the moving effect
                float maxValue = pow((horizonDensityHorizontal), horizonHAngle); // Maximum value to determine window

                int y = int(halfHeight + pow(pos, horizonHAngle) * (halfHeight / maxValue)); // Exponential equation gives Pseudo-3D effect
                
                canvas->drawLine(0, y, width, y, TFT_RED);
            }

            zPosition += zVelocity * zVelMultiplier;
            xPosition += xVelocity;
        }

        void setZVelocity(float velocity) { zVelocity = velocity; }
        void setXVelocity(float velocity) { xVelocity = velocity; }
};