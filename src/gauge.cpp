#include "gauge.h"
#include <iostream>


void Dial::draw() const {
    // draw the STUPID dial
}

void Needle::draw() const {
    // draw the STUPID needle
}


//GAUGEFACE CLASS

GaugeFace::GaugeFace(TFT_eSPI& display) : tft(display) {}

void GaugeFace::addDrawable(Drawable* drawable) {
    drawables.push_back(drawable);
}

void GaugeFace::drawDrawables() const {
    for (const auto& drawable : drawables) {
        drawable->draw();
    }
}

void GaugeFace::drawCircle() {
    tft.drawCircle(120,120,5,TFT_WHITE);
}