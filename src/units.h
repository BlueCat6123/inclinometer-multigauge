#include <Arduino.h>
#include <vector>

struct Unit {
    const String name;
    const float amount;
    const float offset;
};

int temperatureUnitIndex = 0;

const std::vector<Unit> temperatureUnit = {
    { "Celcius", 1.0, 0.0 }, //Base unit
    { "Fahrenheit", (9.0 / 5.0), 32.0 }
};

int velocityUnitIndex = 0;

const std::vector<Unit> velocityUnit = {
    { "km/h", 1.0, 0.0 }, //Base unit
    { "MPH", 0.621371, 0.0 }
};

int pressureUnitIndex = 0;

const std::vector<Unit> pressureUnit = {
    { "psi", 1.0, 0.0 }, //Base unit
    { "bar", 0.0689476, 0.0 },
    { "mbar", 68.9476, 0.0 },
    { "in.Hg", 2.03602, 0.0 },
    { "kpa", 6.89476, 0.0}
};

int liquidUnitIndex = 0;

const std::vector<Unit> liquidUnit = {
    { "liter", 1.0, 0.0 }, //Base unit
    { "gallon", 0.264172, 0.0 }
};

int smallDistanceUnitIndex = 0;

const std::vector<Unit> smallDistanceUnit = {
    { "meter", 1.0, 0.0 }, //Base unit
    { "feet", 3.28084, 0.0 }
};

int largeDistanceUnitIndex = 0;

const std::vector<Unit> largeDistanceUnit = {
    { "kilometer", 0.001, 0.0 }, //derived from meters
    { "mile", 0.000621371 , 0.0 }
};