#include <Arduino.h>
#include <utils.h>
#include <vector>
#include <TFT_eSPI.h>
#include <variant>

struct MenuItem {
    int textIndex;
    void (*function)();
};

class Menu {
public:
};
