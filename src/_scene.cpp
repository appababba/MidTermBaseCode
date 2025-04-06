#include "_scene.h"

#include <windows.h>
#include <GL/glu.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>


// constructor: initializes variables when a _scene object is created
_scene::_scene()
{
    // set the starting screen state
    currentState = LANDING;
    // create the object responsible for loading textures
    texLoader = new _textureLoader();
    // initialize texture ids to 0 (meaning no texture loaded yet)
    landingTextureID = 0;
    // initialize screen dimensions
    dim.x = 0;
    dim.y = 0;
    // initialize more texture ids
    fontTextureID = 0;
    menuBackgroundTextureID = 0;

    // initialize game object pointers to null (nothing created yet)
    player = nullptr;
    background = nullptr;
    gameInputs = nullptr;
    gameTimer = nullptr;

    // initialize game-specific texture ids
    playerTextureID = 0;
    enemyTextureID = 0;
    bulletTextureID = 0;
    backgroundTextureID = 0;

}

// destructor: cleans up memory when the _scene object is destroyed
_scene::~_scene()
{
    // delete the texture loader object
    delete texLoader;
    texLoader = nullptr; // set pointer to null after deleting

    // delete other game objects
    delete player;
    player = nullptr;
    delete background;
    background = nullptr;
    delete gameInputs;
    gameInputs = nullptr;
    delete gameTimer;
    gameTimer = nullptr;

    // loop through the enemies vector and delete each enemy
    for (_enms* enemy : enemies) {
        delete enemy;
    }
    enemies.clear(); // clear the vector itself

    // loop through the bullets vector and delete each bullet
    for (_bullets* bullet : bullets) {
        delete bullet;
    }
    bullets.clear(); // clear the vector
}

// initializes opengl settings and loads game assets
GLint _scene::initGL()
{
    // set the background color (white) and depth clearing value
    glClearColor(1.0,1.0,1.0,1.0);
    glClearDepth(1.0);
    // enable depth testing for 3d rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // type of depth test

    // enable alpha blending for transparency effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // standard blend function

    // enable lighting and a light source (light0)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // allow material colors to affect lighting
    glEnable(GL_COLOR_MATERIAL);

    // get the screen width and height
    dim.x = GetSystemMetrics(SM_CXSCREEN);
    dim.y = GetSystemMetrics(SM_CYSCREEN);

    // check if the texture loader was created successfully
    if (!texLoader) {
        MessageBox(NULL, "texture loader object not created", "init error", MB_OK | MB_ICONERROR);
        return false; // return false if initialization fails
    }

    // load the background texture for the menu
    if (!loadMenuBackgroundTexture()) {
        // error message is handled inside the function
        return false;
    }

    // load the texture for the font
    texLoader->loadTexture((char*)"images/retro_deco.png");
    if (texLoader->tex == 0) { // check if loading failed
        MessageBox(NULL, "font texture failed to load", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        fontTextureID = texLoader->tex; // store the texture id
        // set texture parameters (how it scales)
        glBindTexture(GL_TEXTURE_2D, fontTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // smooth scaling up
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // smooth scaling down
        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    }
    // load the help screen image
    texLoader->loadTexture((char*)"images/help.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "help screen texture failed to load", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        helpTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, helpTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load the font data file which describes character positions in the font texture
    if (!loadFontData("images/retro_deco.fnt")) {
        // error message handled inside function
        return false;
    }

    // load the landing page image
    texLoader->loadTexture((char*)"images/landing_page.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "landing page texture failed to load", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        landingTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, landingTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load the player sprite texture
    texLoader->loadTexture((char*)"images/player.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "player.png failed to load", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        playerTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, playerTextureID);
        // use nearest neighbor for pixelated look, repeat for wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load the enemy sprite texture
    texLoader->loadTexture((char*)"images/mon.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "enemy texture failed to load, images/mon.png", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        enemyTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, enemyTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load the bullet sprite texture
    texLoader->loadTexture((char*)"images/b.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "bullet texture failed to load, images/b.png", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        bulletTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, bulletTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load the parallax background texture
    texLoader->loadTexture((char*)"images/prlx.jpg");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "background texture failed to load, images/prlx.jpg", "texture load error", MB_OK | MB_ICONERROR);
        return false;
    } else {
        backgroundTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat for scrolling
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // create the player object
    player = new _player();
    if (player) {
        // call player's initialization functions
        player->playerActions(); // likely sets up animation frames or initial state
        player->initPlayer(4, 2); // initialize player properties (maybe grid position?)
    } else { MessageBox(NULL,"player new failed","mem error",MB_OK); return false; } // check memory allocation

    // create the parallax background object
    background = new _parallax();
    if (background) {
        background->initPrlx(); // initialize background properties
    } else { MessageBox(NULL,"background new failed","mem error",MB_OK); return false; }

    // create the input handler object
    gameInputs = new _inputs();
    if (!gameInputs) { MessageBox(NULL,"input new failed","mem error",MB_OK); return false; }

    // create the timer object
    gameTimer = new _timer();
    if (gameTimer) {
        // timer might start automatically or need a start call later
    } else { MessageBox(NULL,"timer new failed","mem error",MB_OK); return false; }

    // create some enemy objects
    for (int i = 0; i < 2; ++i) { // create 2 enemies
        _enms* enemy = new _enms();
        if (enemy) {
            // set initial position for each enemy
            vec3 enemyPos = { -0.5f + i * 1.0f, 0.65f, -5.0f };
            enemy->placeEnms(enemyPos); // place the enemy
            enemy->isEnmsLive = true; // mark the enemy as active
            enemies.push_back(enemy); // add the enemy to the vector
        } else { MessageBox(NULL,"enemy new failed","mem error",MB_OK); /* continue maybe? */ }
    }

    // create a pool of bullet objects (pre-allocate)
    for (int i = 0; i < MAX_BULLETS; ++i) { // max_bullets needs definition elsewhere
        _bullets* bullet = new _bullets();
        if (bullet) {
            // set initial position far away (off-screen)
            vec3 initialPos = {0, 0, -100};
            bullet->bInit(initialPos); // initialize bullet state (inactive)
            bullets.push_back(bullet); // add bullet to the vector
        } else { MessageBox(NULL,"bullet new failed","mem error",MB_OK); /* continue maybe? */ }
    }

    // if everything loaded and initialized correctly, return true
    return true;
}

// handles window resize events
void _scene::reSize(GLint width, GLint height)
{
    if (height == 0) height = 1; // prevent divide by zero
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height; // calculate aspect ratio
    glViewport(0,0,width,height); // set the rendering area to the new window size
    glMatrixMode(GL_PROJECTION); // switch to projection matrix mode
    glLoadIdentity(); // reset the matrix
    // set up the perspective projection (fov, aspect ratio, near/far clip planes)
    gluPerspective(45.0f,aspectRatio,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW); // switch back to modelview matrix mode
    glLoadIdentity(); // reset the matrix
    // store the new dimensions
    dim.x = width;
    dim.y = height;
}

// sets up an orthographic projection for 2d rendering (ui, overlays)
void _scene::setOrthoProjection(int width, int height)
{
    glMatrixMode(GL_PROJECTION); // switch to projection matrix
    glPushMatrix(); // save the current projection matrix (perspective)
    glLoadIdentity(); // reset it
    // set up a 2d orthographic view (0,0 is top-left, width/height is bottom-right)
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW); // switch to modelview matrix
    glPushMatrix(); // save the current modelview matrix
    glLoadIdentity(); // reset it
}

// restores the perspective projection after drawing 2d elements
void _scene::restorePerspectiveProjection()
{
    glMatrixMode(GL_MODELVIEW); // switch to modelview matrix
    glPopMatrix(); // restore the previous modelview matrix
    glMatrixMode(GL_PROJECTION); // switch to projection matrix
    glPopMatrix(); // restore the previous projection matrix (perspective)
    glMatrixMode(GL_MODELVIEW); // switch back to modelview for 3d rendering
}

// updates game logic based on time passed (deltatime)
void _scene::updateGame(float deltaTime) {
    // only update if the game is in the 'game' state
    if (currentState != GAME) return;

    // update player logic (movement, animation based on input)
    if (player) {
        // placeholder: add player update logic using deltatime here
        player->playerActions(); // likely updates animation frame or state based on actiontrigger
    }

    // update background scrolling
    if (background) {
        // scroll the background left at a defined speed
        background->scroll(true, "left", background->speed * deltaTime); // multiply speed by deltatime for frame-rate independence
    }

    // update enemy logic
    for (_enms* enemy : enemies) {
        if (enemy && enemy->isEnmsLive) {
            // placeholder: add enemy movement, ai, shooting logic here
            // e.g., enemy->update(deltatime);
        }
    }

    // update bullet logic
    for (_bullets* bullet : bullets) {
        if (bullet && bullet->bLive) {
            // placeholder: add bullet movement logic here
            // e.g., bullet->update(deltatime);
        }
    }

    // placeholder: add collision detection logic here (player vs enemy, bullet vs enemy, etc.)

}

// draws the entire scene based on the current state
GLint _scene::drawScene()
{
    // clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // reset the modelview matrix

    // choose what to draw based on the game state
    switch (currentState)
    {
        case LANDING:
            // draw the landing page image
            glPushMatrix(); // save current matrix state
            {
                glDisable(GL_LIGHTING); // disable lighting for 2d image
                glDisable(GL_DEPTH_TEST); // disable depth test for 2d image
                glEnable(GL_TEXTURE_2D); // enable texturing
                setOrthoProjection((int)dim.x, (int)dim.y); // switch to 2d projection

                glColor3f(1.0, 1.0, 1.0); // set color to white (so texture isn't tinted)
                glBindTexture(GL_TEXTURE_2D, landingTextureID); // select the landing page texture

                // draw a quad covering the whole screen
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0, 0.0); glVertex2f(0, 0); // top-left tex coord and vertex
                    glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0); // top-right
                    glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y); // bottom-right
                    glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y); // bottom-left
                glEnd();

                glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
                glDisable(GL_TEXTURE_2D); // disable texturing
                restorePerspectiveProjection(); // switch back to 3d projection
                glEnable(GL_DEPTH_TEST); // re-enable depth test
                // lighting was already disabled, re-enable if needed outside the switch
            }
            glPopMatrix(); // restore matrix state
            break;

        case MENU:
            // call the specific function to draw the menu
            drawMenuScene();
            break;

        case HELP:
            // draw the help screen
            glPushMatrix();
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND); // enable blending if help image has transparency
                glEnable(GL_TEXTURE_2D);
                setOrthoProjection((int)dim.x, (int)dim.y);

                glColor3f(1.0, 1.0, 1.0); // white color
                glBindTexture(GL_TEXTURE_2D, helpTextureID); // bind help texture
                // draw quad for help image
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
                    glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0);
                    glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y);
                    glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);

                // draw help text using the font rendering function
                drawText("press [esc] to return to menu", 50, dim.y - 50, 1.0f, 1.0f, 1.0f); // white text at bottom-left

                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                restorePerspectiveProjection();
                glEnable(GL_DEPTH_TEST);
                 // re-enable lighting if needed outside the switch
            }
            glPopMatrix();
            break;

        case GAME:
            // set a green background color for the game area (will be covered by parallax)
            glClearColor(0.0, 0.4, 0.0, 1.0);
            // ensure buffers are cleared (redundant here, but safe)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // enable opengl features needed for the 3d game scene
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND); // for player/enemy transparency potentially
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor3f(1.0, 1.0, 1.0); // default color to white

            // draw the scrolling background first
            glPushMatrix();
            glDisable(GL_LIGHTING); // disable lighting for the background image
            if (background) {
                // call the background's draw function
                background->drawBackground(backgroundTextureID, dim.x, dim.y);
            }
            glEnable(GL_LIGHTING); // re-enable lighting for other objects
            glPopMatrix();

            // draw the player
            glPushMatrix();
            if (player) {
                // call the player's draw function, passing its texture
                player->drawPlayer(playerTextureID);
            }
            glPopMatrix();

            // draw active enemies
            for (_enms* enemy : enemies) {
                if (enemy && enemy->isEnmsLive) { // only draw if enemy exists and is alive
                    glPushMatrix();
                    // call the enemy's draw function
                    enemy->drawEnms(enemyTextureID);
                    glPopMatrix();
                }
            }

            // draw active bullets
            for (_bullets* bullet : bullets) {
                if (bullet && bullet->bLive) { // only draw if bullet exists and is active
                    glPushMatrix();
                    // call the bullet's draw function
                    bullet->drawBullet(bulletTextureID);
                    glPopMatrix();
                }
            }

            // disable states not needed by default after drawing game elements
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D); // disable textures until needed again
            // lighting and depth test remain enabled usually

            break;

        case PAUSED:
            // draw the pause overlay and text
            glPushMatrix();
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND); // enable blending for the semi-transparent overlay
                glEnable(GL_TEXTURE_2D); // needed for drawtext

                setOrthoProjection((int)dim.x, (int)dim.y);

                // draw a semi-transparent dark quad as the overlay background
                glColor4f(0.0f, 0.0f, 0.0f, 0.75f); // black with 75% opacity
                float popupWidth = 350; float popupHeight = 100;
                float popupX = (dim.x - popupWidth) / 2.0f; // center horizontally
                float popupY = (dim.y - popupHeight) / 2.0f; // center vertically
                glBegin(GL_QUADS);
                    glVertex2f(popupX, popupY);
                    glVertex2f(popupX + popupWidth, popupY);
                    glVertex2f(popupX + popupWidth, popupY + popupHeight);
                    glVertex2f(popupX, popupY + popupHeight);
                glEnd();

                // draw pause menu text over the overlay
                drawText("quit game?", popupX + 50, popupY + 20, 1.0f, 1.0f, 1.0f); // white text
                drawText("yes (enter) / no (esc)", popupX + 50, popupY + 50, 1.0f, 1.0f, 1.0f);

                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                restorePerspectiveProjection();
                glEnable(GL_DEPTH_TEST);
                // re-enable lighting if needed outside the switch
            }
            glPopMatrix();
            break;

        default: // handle unexpected states if necessary
            break;
    }

    return true; // indicate drawing was successful
}

// handles windows messages (keyboard, mouse input)
int _scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // handle input specifically for the 'game' state first
    if (currentState == GAME && gameInputs) { // check state and if input handler exists
        switch(uMsg) { // check the type of message
            case WM_KEYDOWN: // key pressed down
                // check which key was pressed (wparam)
                if (wParam == VK_ESCAPE) { // escape key
                    currentState = PAUSED; // change state to paused
                    return 0; // message handled
                }

                if (wParam == VK_SPACE) { // space bar
                    // find an inactive bullet to fire
                    for (_bullets* bullet : bullets) {
                        if (bullet && !bullet->bLive) { // if bullet exists and is not already active
                            vec3 playerPos = {0,0,0}; // default position
                            if(player) {
                                playerPos = player->plPos; // get current player position
                            }
                            // reset the bullet to the player's position and make it active
                            bullet->bReset(playerPos);
                            bullet->bActions(); // start bullet movement/animation
                            break; // only fire one bullet per key press
                        }
                    }
                } else if (wParam == 'P') { // 'p' key
                    currentState = PAUSED; // pause the game
                } else {
                    // handle player movement keys
                    if (player) {
                        switch (wParam) {
                            case VK_LEFT: player->actionTrigger = _player::LEFTWALK; break; // set player action state
                            case VK_RIGHT: player->actionTrigger = _player::RIGHTWALK; break;
                            // add up/down if needed
                        }
                    }
                }
                return 0; // message handled

            case WM_KEYUP: // key released
                if (player) {
                    switch (wParam) {
                        case VK_LEFT:
                            // if the player was walking left, set action back to standing
                            if (player->actionTrigger == _player::LEFTWALK) {
                                player->actionTrigger = _player::STAND;
                            }
                            break;
                        case VK_RIGHT:
                            // if the player was walking right, set action back to standing
                            if (player->actionTrigger == _player::RIGHTWALK) {
                                player->actionTrigger = _player::STAND;
                            }
                            break;
                        // add up/down if needed
                    }
                }
                return 0; // message handled

            case WM_MOUSEMOVE:
                // handle mouse movement during the game if needed (e.g., aiming)
                return 0; // currently unused

            // add other game-specific input messages here (e.g., mouse clicks)
        }
    }

    // handle input for non-game states (menus, landing page, etc.)
    switch (uMsg)
    {
        case WM_KEYDOWN:
            // handle key presses based on the current state
            if (currentState == LANDING) {
                if (wParam == VK_RETURN) { currentState = MENU; } // enter key -> go to menu
            }
            else if (currentState == MENU) {
                switch (wParam) {
                    case 'N': currentState = GAME; break; // 'n' -> start new game
                    case 'H': currentState = HELP; break; // 'h' -> show help
                    case 'E': PostQuitMessage(0); break; // 'e' -> exit application
                    case VK_ESCAPE: currentState = LANDING; break; // escape -> back to landing
                }
            }
            else if (currentState == HELP) {
                if (wParam == 'M' || wParam == VK_ESCAPE) { // 'm' or escape
                    currentState = MENU; // go back to menu
                }
            }
            else if (currentState == GAME) {
                // game keydown is handled above, this is fallback (shouldn't be reached usually)
            }
            else if (currentState == PAUSED) {
                if (wParam == VK_RETURN) { // enter key
                    PostQuitMessage(0); // confirm quit
                } else if (wParam == 'P' || wParam == VK_ESCAPE) { // 'p' or escape
                    currentState = GAME; // resume game
                }
            }
            return 0; // message handled

        case WM_KEYUP:
            // handle key releases for non-game states if needed
            break;

        case WM_LBUTTONDOWN: // left mouse button pressed
        {
            if (currentState == LANDING) {
                // click anywhere on landing screen goes to menu
                currentState = MENU;
            }
            else if (currentState == MENU) {
                // check if the click was on a menu "button" (defined by text position)
                int mouseX = LOWORD(lParam); // get click x coordinate
                int mouseY = HIWORD(lParam); // get click y coordinate
                POINT clickPoint = { mouseX, mouseY }; // store as a point

                // define the approximate rectangular areas for the menu text options
                float menuX = dim.x / 2.0f - 75.0f; // horizontal start of text
                float menuY_Start = dim.y / 2.0f - 50.0f; // vertical start of first option
                float menuY_Spacing = 60; // vertical space between options
                float buttonWidth = 250; // approximate width of the clickable area
                float buttonHeight = 50; // approximate height

                // create rect structures for collision detection
                RECT newGameRect = {(long)menuX, (long)menuY_Start, (long)(menuX + buttonWidth), (long)(menuY_Start + buttonHeight)};
                RECT helpRect = {(long)menuX, (long)(menuY_Start + menuY_Spacing), (long)(menuX + buttonWidth), (long)(menuY_Start + menuY_Spacing + buttonHeight)};
                // add exit rect if needed

                // check if the click point is inside any button rect
                if (PtInRect(&newGameRect, clickPoint)) { currentState = GAME; }
                else if (PtInRect(&helpRect, clickPoint)) { currentState = HELP; }
                // add exit check if needed: else if (ptinrect(&exitrect, clickpoint)) { postquitmessage(0); }
            }
            // add click handling for other states if needed (e.g., paused menu)
            return 0; // message handled
        }

        default: // for messages not handled above
            break;
    }

    // pass unhandled messages to the default windows procedure
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// loads font data from a .fnt file generated by tools like bmfont
bool _scene::loadFontData(const char* filename) {
    std::ifstream file(filename); // open the file
    if (!file.is_open()) { // check if file opened successfully
        std::cerr << "error: could not open font data file: " << filename << std::endl;
        MessageBox(NULL, "error: could not open .fnt file!", "font data error", MB_OK | MB_ICONERROR);
        return false; // return error
    }
    std::string line; std::string key; // variables for parsing
    fontDataMap.clear(); // clear any existing font data
    // reset font metrics
    fontTextureWidth = 0; fontTextureHeight = 0; fontLineHeight = 0; fontBaseHeight = 0;

    // read the file line by line
    while (std::getline(file, line)) {
        std::stringstream ss(line); // use stringstream to easily extract parts
        ss >> key; // read the first word (key) of the line

        if (key == "common") { // information about the font texture itself
            std::string valuePair;
            while (ss >> valuePair) { // read key=value pairs
                size_t equalsPos = valuePair.find('=');
                if (equalsPos != std::string::npos) {
                    std::string currentKey = valuePair.substr(0, equalsPos);
                    std::string currentValue = valuePair.substr(equalsPos + 1);
                    try { // use try-catch for safety when converting string to number
                        if (currentKey == "base") fontBaseHeight = std::stof(currentValue);
                        else if (currentKey == "lineHeight") fontLineHeight = std::stof(currentValue);
                        else if (currentKey == "scaleW") fontTextureWidth = std::stof(currentValue); // texture width
                        else if (currentKey == "scaleH") fontTextureHeight = std::stof(currentValue); // texture height
                    } catch (...) { std::cerr << "font parse error (common)\n"; }
                }
            }
        } else if (key == "char") { // information about a specific character
            CharData charData; // struct to hold data for one character
            std::string valuePair;
            while (ss >> valuePair) { // read key=value pairs
                size_t equalsPos = valuePair.find('=');
                if (equalsPos != std::string::npos) {
                    std::string currentKey = valuePair.substr(0, equalsPos);
                    std::string currentValue = valuePair.substr(equalsPos + 1);
                    try { // use try-catch for string to number conversion
                        if (currentKey == "id") charData.id = std::stoi(currentValue); // ascii id
                        else if (currentKey == "x") charData.x = std::stof(currentValue); // position in texture (top-left x)
                        else if (currentKey == "y") charData.y = std::stof(currentValue); // position in texture (top-left y)
                        else if (currentKey == "width") charData.width = std::stof(currentValue); // width in texture
                        else if (currentKey == "height") charData.height = std::stof(currentValue); // height in texture
                        else if (currentKey == "xoffset") charData.xoffset = std::stof(currentValue); // offset when drawing on screen (x)
                        else if (currentKey == "yoffset") charData.yoffset = std::stof(currentValue); // offset when drawing on screen (y)
                        else if (currentKey == "xadvance") charData.xadvance = std::stof(currentValue); // how much to move cursor after drawing
                    } catch (...) { std::cerr << "font parse error (char)\n"; }
                }
            }
            // store the parsed character data in the map, using the character id as the key
            if (charData.id != 0 ) { // make sure id is valid before storing
                fontDataMap[charData.id] = charData;
            }
        }
        // add parsing for "kerning" lines if needed
    }
    file.close(); // close the file

    // check if essential data was loaded
    if (fontTextureWidth == 0 || fontTextureHeight == 0 || fontDataMap.empty()) {
        MessageBox(NULL, "warning: font data might be missing or invalid (.fnt parsing).", "font data warning", MB_OK | MB_ICONWARNING);
        // might still be usable if only some chars are missing, but drawing might fail
    }
    return true; // loading finished (possibly with warnings)
}

// draws text on the screen using the loaded bitmap font
void _scene::drawText(std::string text, float screenX, float screenY, float r, float g, float b) {
    // check if font texture and data are loaded and valid
    if (fontTextureID == 0 || fontDataMap.empty() || fontTextureWidth == 0 || fontTextureHeight == 0) {
        return; // cannot draw text without font resources
    }

    // setup opengl state for drawing textured quads with transparency
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureID); // bind the font atlas texture
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use alpha blending

    glColor3f(r, g, b); // set the text color

    float currentX = screenX; // tracks the horizontal position for the next character

    // begin drawing quads (one quad per character)
    glBegin(GL_QUADS);
    for (char &c : text) { // loop through each character in the input string
        // find the character data in the map
        auto it = fontDataMap.find(c);
        if (it != fontDataMap.end()) { // if character data is found
            CharData cd = it->second; // get the character data struct

            // calculate screen coordinates for the quad vertices
            float x1 = currentX + cd.xoffset; // top-left x
            float y1 = screenY + cd.yoffset; // top-left y
            float x2 = x1 + cd.width; // bottom-right x
            float y2 = y1 + cd.height; // bottom-right y

            // calculate texture coordinates (uv) based on character position in the atlas
            float u1 = cd.x / fontTextureWidth; // top-left u
            float v1 = cd.y / fontTextureHeight; // top-left v
            float u2 = (cd.x + cd.width) / fontTextureWidth; // bottom-right u
            float v2 = (cd.y + cd.height) / fontTextureHeight; // bottom-right v

            // define the quad vertices and their corresponding texture coordinates
            glTexCoord2f(u1, v1); glVertex2f(x1, y1); // top-left
            glTexCoord2f(u2, v1); glVertex2f(x2, y1); // top-right
            glTexCoord2f(u2, v2); glVertex2f(x2, y2); // bottom-right
            glTexCoord2f(u1, v2); glVertex2f(x1, y2); // bottom-left

            // move the cursor position for the next character
            currentX += cd.xadvance;
        } else {
            // character not found in font data (e.g., unsupported character)
            // optional: advance by a default amount (like space width) or draw a placeholder
            auto space_it = fontDataMap.find(' '); // find data for space character
            if (space_it != fontDataMap.end()) {
                currentX += space_it->second.xadvance; // advance by space width
            }
            // else: currentx remains unchanged, characters might overlap
        }
    }
    glEnd(); // finish drawing quads

    glBindTexture(GL_TEXTURE_2D, 0); // unbind the font texture
    // disable blend/texture if they were specifically enabled only for text
    // glDisable(GL_TEXTURE_2D);
    // glDisable(GL_BLEND);
}

// specific function to load the menu background texture
bool _scene::loadMenuBackgroundTexture() {
    texLoader->loadTexture((char*)"images/menu_background.png"); // load the image
    if (texLoader->tex == 0) { // check for loading errors
        MessageBox(NULL, "menu background texture failed to load", "texture load error", MB_OK | MB_ICONERROR);
        return false; // return failure
    }
    menuBackgroundTextureID = texLoader->tex; // store the texture id
    // set texture parameters for the menu background (linear filtering)
    glBindTexture(GL_TEXTURE_2D, menuBackgroundTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    return true; // return success
}

// specific function to draw the menu screen
void _scene::drawMenuScene() {
    glPushMatrix(); // save current opengl state
    {
        // setup for 2d drawing (like landing page)
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND); // allow potential transparency in background or text
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setOrthoProjection((int)dim.x, (int)dim.y); // switch to 2d view

        // draw the menu background image first
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // set color to white, full opacity
        glBindTexture(GL_TEXTURE_2D, menuBackgroundTextureID); // bind the menu background texture
        glBegin(GL_QUADS); // draw a full-screen quad
            glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
            glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0);
            glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y);
            glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

        // calculate positions for menu text items (centered horizontally, spaced vertically)
        float menuX = dim.x / 2.0f - 75.0f; // adjust offset based on text width estimate
        float menuY_Start = dim.y / 2.0f - 50.0f; // starting y position (near center)
        float menuY_Spacing = 60; // vertical distance between menu items

        // draw the menu options using the drawtext function
        drawText("new game: 'n'", menuX, menuY_Start, 0.0f, 0.0f, 0.0f); // black text
        drawText("help: 'h'", menuX, menuY_Start + menuY_Spacing, 0.0f, 0.0f, 0.0f);
        drawText("exit: press e", menuX, menuY_Start + 2 * menuY_Spacing, 0.0f, 0.0f, 0.0f);

        // restore previous opengl state
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        restorePerspectiveProjection(); // switch back to 3d view
        glEnable(GL_DEPTH_TEST);
        // re-enable lighting if needed outside the switch
    }
    glPopMatrix(); // restore saved opengl state
}
