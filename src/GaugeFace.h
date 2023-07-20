#include <vector>
#include <Drawables.h>

/////////////////
// GAUGE CLASS //
/////////////////

/*
This class was created with the intent of making custom gauge faces easy to create. A GaugeFace object
holds an array of objects from or derived from the Drawables class.
*/

template <typename T> //Removes the need to upcast any objects that derive from Drawables

class GaugeFace {
    private:
        std::vector<T*> drawables; // Array of Drawables that make up the gauge face

        TFT_eSprite* _canvas; // Canvas sprite to be drawn to

        uint16_t backgroundColor;

    public:
        GaugeFace(TFT_eSprite* display) : _canvas(display) { backgroundColor = TFT_BLACK; } //Constructor

        void addDrawable(T* drawable) { drawables.push_back(drawable); } // Adds an object from the Drawable class to the list of objects to draw

        void removeDrawable(T* drawable) {
            auto it = std::find(drawables.begin(), drawables.end(), drawable); // Find the iterator pointing to the drawable object in the vector

            // Check if the object was found in the vector
            if (it != drawables.end()) { drawables.erase(it); }
        }
        
        void draw()  { // Loops through all Drawable objects and executes their draw() function
            _canvas->fillSprite(TFT_BLACK);
            for (T* drawable : drawables) { drawable->draw(_canvas); }
        }
};

template class GaugeFace<Drawable>;