// MATH FUNCTIONS

#ifndef UTILS_H
#define UTILS_H

float lerp(float a, float b, float t) { return (a + (b - a) * t); } // Linear interpolation function, possibly the best function ever created



// CONVERSIONS

float degToRad(float d) { return ((d * 1000) / 57296); } // Convert degrees -> radians

#endif // UTILS_H