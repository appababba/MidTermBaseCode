#ifndef _SCENE_
#define _SCENE_

#include <windows.h>
#include <GL/gl.h>
#include<_common.h>
#include "_textureloader.h"
#include <string>
#include <map>

enum GameState {
    LANDING,
    MENU,
    HELP,
    GAME,
    PAUSED
};

struct CharData {
    int id = 0;     // Character ID
    float x = 0;    // Position on texture (left)
    float y = 0;    // Position on texture (top)
    float width = 0; // Width on texture
    float height = 0;// Height on texture
    float xoffset = 0; // Offset when drawing on screen (X)
    float yoffset = 0; // Offset when drawing on screen (Y)
    float xadvance = 0;// How much to advance cursor after drawing
};

class _scene
{
    public:
        _scene();             //constructor
        virtual ~_scene();    //destructor

       GLint initGL();        //initialize game objects
       void reSize(GLint width,GLint Height); // resize window
       void  drawScene();     // Render the Final scene

       int winMsg(HWND	hWnd,			// Handle For This Window
                  UINT	uMsg,			// Message For This Window
                  WPARAM	wParam,			// Additional Message Information
                  LPARAM	lParam);

        vec2 dim; // for screen width and height;

    protected:

    private:

        GameState currentState;

        _textureLoader* texLoader; //pointer to texture loader object
        GLuint landingTextureID; // opengl texture id for landing page image
        GLuint fontTextureID;

        void setOrthoProjection(int width, int height); //switch to 2d view

        void restorePerspectiveProjection(); //switch back to 3d view

        std::map<int, CharData> fontDataMap; // Map to store data for each character ID
        float fontBaseHeight = 0; // Stores the 'base' value from the .fnt file
        float fontLineHeight = 0; // Stores the 'lineHeight' value
        float fontTextureWidth = 0; // Stores texture width ('scaleW')

        float fontTextureHeight = 0; // Stores texture height ('scaleH')
        bool loadFontData(const char* filename);
        void drawText(std::string text, float screenX, float screenY);
};

#endif // _SCENE_
