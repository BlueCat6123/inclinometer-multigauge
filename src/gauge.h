#include <TFT_eSPI.h>
#include <Arduino.h>
#include <vector>

class Drawable {
    public:
        virtual void draw() const = 0;
};

class Dial : public Drawable {
    public:
        void draw() const override;
};

class Needle : public Drawable {
    public:
        void draw() const override;
};



class GaugeFace {
    private:
        std::vector<Drawable*> drawables;
        TFT_eSPI& tft;

    public:
        GaugeFace(TFT_eSPI& display);
        void addDrawable(Drawable* drawable);
        void drawDrawables() const;
        void drawCircle();
};