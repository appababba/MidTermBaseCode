#ifndef _PARALLAX_H
#define _PARALLAX_H

#include<_common.h>
// #include<_textureloader.h> // No longer needed here if removed below
#include<_timer.h>
#include<string> // Include string for the scroll function parameter

using namespace std; // Add if 'string' is not recognized otherwise

class _parallax
{
    public:
        _parallax();
        virtual ~_parallax();

        
        _timer *tmr = new _timer();

        // Added textureID parameter
        void drawBackground(GLuint textureID, float width, float height);

        //  Takes no parameters now
        void initPrlx();

        void scroll(bool, string, float); // Changed from std::string to string

        float xMax,xMin,yMax,yMin;
        float speed;

    protected:

    private:
};

#endif // _PARALLAX_H