#ifndef _SCENE_H
#define _SCENE_H

#include <windows.h>
#include <GL/gl.h>
#include <vector> 
#include <string> 
#include <map>    


#include "_textureLoader.h"
#include "_common.h" // Assuming vec2, vec3 are here
#include "_timer.h"
#include "_player.h"
#include "_enms.h"
#include "_bullets.h"
#include "_parallax.h"
#include "_inputs.h"
// #include "_collisioncheck.h" 
// #include "_sounds.h"      
// #include "_lightsetting.h" 


// Enum for different game states
enum GameState {
    LANDING,
    MENU,
    HELP,
    GAME,
    PAUSED
    // Add other states like GAME_OVER if needed
};

// Structure to hold character data from .fnt file
struct CharData {
    int id = 0;
    float x = 0, y = 0;
    float width = 0, height = 0;
    float xoffset = 0, yoffset = 0;
    float xadvance = 0;
    // Add page, chnl if needed
};


class _scene
{
    public:
        _scene();
        virtual ~_scene();

        GLint initGL();       // Initialize OpenGL
        GLint drawScene();    // Draw scene (calls specific draw functions based on state)
        void reSize(GLint, GLint); // Handle window resize
        GLuint menuBackgroundTextureID;
        int winMsg(HWND, UINT, WPARAM, LPARAM); // Handle window messages 

     
        void updateGame(float deltaTime);

    protected:

    private:
        GameState currentState;     // Current game state
        _textureLoader* texLoader;  // Pointer to the texture loader
        vec2 dim;                   // Window dimensions

        // --- Texture IDs ---
        GLuint landingTextureID;    // ID for landing page texture
        GLuint fontTextureID;       // ID for the font texture

        GLuint playerTextureID;
        GLuint enemyTextureID;
        GLuint bulletTextureID;
        GLuint backgroundTextureID;
        GLuint helpTextureID;

        // --- Font Rendering Data ---
        std::map<int, CharData> fontDataMap; // Map character IDs to their data
        float fontTextureWidth = 0;
        float fontTextureHeight = 0;
        float fontLineHeight = 0;
        float fontBaseHeight = 0;
        bool loadFontData(const char* filename); // Helper to load .fnt
        void drawText(std::string text, float screenX, float screenY, float r, float g, float b);

        // --- Projection Helpers ---
        void setOrthoProjection(int width, int height);
        void restorePerspectiveProjection();

        // --- Game Objects --- // <<< NEW >>>
        _player* player = nullptr;          // The player character
        _parallax* background = nullptr;    // The scrolling background
        _inputs* gameInputs = nullptr;      // Input handler specifically for game state
        _timer* gameTimer = nullptr;        // A timer for game updates/animations 
        // _lightsetting* lights = nullptr; // 
        // _sounds* soundManager = nullptr; // 
        // _collisioncheck* collisionChecker = nullptr; // Example

        // Collections for multiple enemies/bullets
        std::vector<_enms*> enemies;
        std::vector<_bullets*> bullets;
        
        const int MAX_BULLETS = 20; // Example limit

        bool loadMenuBackgroundTexture();
        void drawMenuScene();

};

#endif // _SCENE_H
