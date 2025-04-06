#include "_scene.h" // Already includes all necessary headers now

#include <windows.h>
#include <GL/glu.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>  // Ensure vector is included


_scene::_scene()
{
    currentState = LANDING; // start at the landing page
    texLoader = new _textureLoader(); // create the texture loader instance
    landingTextureID = 0;     // initialize texture ID to 0
    dim.x = 0;                  // initialize dimensions
    dim.y = 0;
    fontTextureID = 0;
    menuBackgroundTextureID = 0; // Initialize menu background ID

    // <<< NEW >>> Initialize new pointers to nullptr
    player = nullptr;
    background = nullptr;
    gameInputs = nullptr;
    gameTimer = nullptr; // Consider if a global timer or per-object timers are better
    // lights = nullptr;
    // soundManager = nullptr;
    // collisionChecker = nullptr;

    playerTextureID = 0;
    enemyTextureID = 0;
    bulletTextureID = 0;
    backgroundTextureID = 0;

}

_scene::~_scene()
{
    delete texLoader; // delete texture loader instance
    texLoader = nullptr;

    // <<< NEW >>> Clean up dynamically allocated objects
    delete player;
    player = nullptr;
    delete background;
    background = nullptr;
    delete gameInputs;
    gameInputs = nullptr;
    delete gameTimer;
    gameTimer = nullptr;
    // delete lights; lights = nullptr;
    // delete soundManager; soundManager = nullptr; // Remember to call soundEngine->drop() first if using irrKlang
    // delete collisionChecker; collisionChecker = nullptr;

    // Clean up enemies vector (if holding pointers)
    for (_enms* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    // Clean up bullets vector (if holding pointers)
    for (_bullets* bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
}


GLint _scene::initGL()
{
    // --- Basic GL Setup ---
    glClearColor(1.0,1.0,1.0,1.0); // Default clear color
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING); // Enable lighting globally
    glEnable(GL_LIGHT0);   // Enable light source 0 (configure in lightsetting class or here)
    glEnable(GL_COLOR_MATERIAL); // Allows glColor to affect material colors

    dim.x = GetSystemMetrics(SM_CXSCREEN);
    dim.y = GetSystemMetrics(SM_CYSCREEN);

    // --- Texture Loader Check ---
    if (!texLoader) {
        MessageBox(NULL, "Texture loader object not created", "Init error", MB_OK | MB_ICONERROR);
        return false;
    }

    // --- Load UI Textures & Font ---
    // Menu Background Texture (Load First - background for other UI)
    if (!loadMenuBackgroundTexture()) {
         // Message shown in function
         return false; // Exit if essential background fails
    }
    // Font Texture
    texLoader->loadTexture((char*)"images/retro_deco.png");
    if (texLoader->tex == 0) {
         MessageBox(NULL, "Font Texture failed to load", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if font texture fails
    } else {
        fontTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, fontTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Font Data
    if (!loadFontData("images/retro_deco.fnt")) {
        // Error message shown in function
        return false; // Exit if font data fails
    }
    // Landing Page Texture
    texLoader->loadTexture((char*)"images/landing_page.png");
    if (texLoader->tex == 0) {
         MessageBox(NULL, "Landing page texture failed to load", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if landing page fails
    } else {
         landingTextureID = texLoader->tex;
         glBindTexture(GL_TEXTURE_2D, landingTextureID);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glBindTexture(GL_TEXTURE_2D, 0);
     }


    // --- <<< NEW >>> Load Game Textures ---
    // Player Texture
    texLoader->loadTexture((char*)"images/player.png");
    if (texLoader->tex == 0) { // Make sure path is correct
         MessageBox(NULL, "player.png failed to load", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if player texture fails
    } else {
        playerTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, playerTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Example: Use NEAREST for pixel art
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Example for sprite sheets
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Enemy Texture
    texLoader->loadTexture((char*)"images/mon.png");
    if (texLoader->tex == 0) { // Make sure path is correct
         MessageBox(NULL, "Enemy texture failed to load, images/mon.png", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if enemy texture fails
    } else {
        enemyTextureID = texLoader->tex;
         glBindTexture(GL_TEXTURE_2D, enemyTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Bullet Texture
    texLoader->loadTexture((char*)"images/b.png");
    if (texLoader->tex == 0) { // Make sure path is correct
         MessageBox(NULL, "Bullet texture failed to load, images/b.png", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if bullet texture fails
    } else {
        bulletTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, bulletTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
     // Background Texture
    texLoader->loadTexture((char*)"images/prlx.jpg");
    if (texLoader->tex == 0) { // Make sure path is correct
         MessageBox(NULL, "Background texture failed to load, images/prlx.jpg", "Texture load error", MB_OK | MB_ICONERROR);
         return false; // Exit if background texture fails
    } else {
        backgroundTextureID = texLoader->tex;
        glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // --- <<< NEW >>> Initialize Game Objects ---
    player = new _player();
    if (player) {
        // Initialize player - Check _player.h for the correct parameters for initPlayer

        // Example: Assuming player sprite sheet has 4 columns, 2 rows
        player->playerActions();
        player->initPlayer(4, 2); // <<< ADJUST FRAMES IF NEEDED >>>
    } else { MessageBox(NULL,"Player new failed","Mem Error",MB_OK); return false; /* Handle allocation error */ }

    background = new _parallax();
    if (background) {
        // Parallax init doesn't take texture ID anymore based on your provided code
        background->initPrlx();
    } else { MessageBox(NULL,"Background new failed","Mem Error",MB_OK); return false; /* Handle allocation error */ }

    gameInputs = new _inputs();
    if (!gameInputs) { MessageBox(NULL,"Input new failed","Mem Error",MB_OK); return false; /* Handle allocation error */ }

    gameTimer = new _timer();
     if (gameTimer) {
        // Check _timer.h for the correct start function
        // Assuming it's start()
        // gameTimer->start(); // Start the timer
     } else { MessageBox(NULL,"Timer new failed","Mem Error",MB_OK); return false; /* Handle allocation error */ }

    // Initialize Enemies (Example: Create 2 enemies)
    for (int i = 0; i < 2; ++i) {
        _enms* enemy = new _enms();
        if (enemy) {
             // Check _enms.h/cpp for init function - Assuming initEnms takes frames like player
             // enemy->initEnms(num_enemy_frames_x, num_enemy_frames_y); // <<< ADD ENEMY INIT >>>

             // Place the enemy
             vec3 enemyPos = { -0.5f + i * 1.0f, 0.65f, -5.0f }; // Example positions
             enemy->placeEnms(enemyPos); // Assuming placeEnms sets position
             enemy->isEnmsLive = true; // Make it alive
             enemies.push_back(enemy);
        } else { MessageBox(NULL,"Enemy new failed","Mem Error",MB_OK); /* Handle allocation error */ }
    }

    // Initialize Bullets (Create a pool)
     for (int i = 0; i < MAX_BULLETS; ++i) {
         _bullets* bullet = new _bullets();
         if (bullet) {
             vec3 initialPos = {0, 0, -100}; // Start off-screen
             bullet->bInit(initialPos); // Initialize it (sets bLive to false)
             bullets.push_back(bullet);
         } else { MessageBox(NULL,"Bullet new failed","Mem Error",MB_OK); /* Handle allocation error */ }
     }

    // Initialize Sound, Lights, Collision systems if you have classes for them
    // soundManager = new _sounds();
    // soundManager->initSounds(); // Initialize irrKlang etc.
    // lights = new _lightsetting();
    // lights->initLight();


    return true; // Success
}

void _scene::reSize(GLint width, GLint height)
{
    if (height == 0) height = 1;
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,aspectRatio,0.1f,100.0f); // Use perspective for GAME state
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    dim.x = width;
    dim.y = height;
}

// --- Projection Helpers --- (Keep these as they are)
void _scene::setOrthoProjection(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0); // Top-left origin for UI
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Also push modelview
    glLoadIdentity();
}

void _scene::restorePerspectiveProjection()
{
    glMatrixMode(GL_MODELVIEW); // Pop modelview first
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


// --- <<< NEW >>> Game Update Logic ---
// <<< TODO: Implement actual update logic >>>
void _scene::updateGame(float deltaTime) { // deltaTime parameter might be needed if gameTimer isn't used internally by objects
    if (currentState != GAME) return; // Only update if in GAME state

    // --- Update Timer (Calculate deltaTime if not passed in) ---
    // float deltaTime = gameTimer->getDeltaTime(); // Or however your timer provides elapsed time

    // --- Process Input for Player Actions ---
    // Example using the global keys[] array from main.cpp (simple approach)
    // Or use gameInputs class methods if it tracks key states
    if (player) {
         // Check your _player class for movement/action methods
         // Example:
         // if (keys[VK_LEFT]) { player->actionTrigger = _player::LEFTWALK; }
         // else if (keys[VK_RIGHT]) { player->actionTrigger = _player::RIGHTWALK; }
         // else { player->actionTrigger = _player::STAND; } // Default to STAND if no movement key pressed
         // Note: Shooting is handled in winMsg WM_KEYDOWN for VK_SPACE
    }

    // --- Update Game Objects ---
    // Call the action/update methods for each object
    // These methods should handle movement, animation frame updates, AI, etc.
    if (player) {
        player->playerActions(); // Call player's update logic (handles animation based on actionTrigger, moves position)
    }

    if (background) {
        // Check your _parallax class for its update/scroll method
        // Example: Auto-scroll left
        background->scroll(true, "left", background->speed); // Use member speed or pass deltaTime based speed
    }

    for (_enms* enemy : enemies) {
        if (enemy && enemy->isEnmsLive) {
             // Check your _enms class for its update method
             // enemy->actions(); // Or enemy->update(deltaTime);
        }
    }

    for (_bullets* bullet : bullets) {
        if (bullet && bullet->bLive) {
             // Check your _bullets class for its update method
             // This should move the bullet
             // bullet->bUpdate(); // Or bullet->update(deltaTime);
        }
    }

    // --- Collision Detection ---
    // <<< USER ACTION REQUIRED >>> Fix warnings in _collisionckeck.cpp & Implement
    /*
    if (collisionChecker) { // Assuming a collision checker class exists and is initialized
        // Bullet-Enemy Collisions
        for (_bullets* bullet : bullets) {
            if (!bullet || !bullet->bLive) continue;
            for (_enms* enemy : enemies) {
                if (!enemy || !enemy->isEnmsLive) continue;

                // Replace getBounds() with actual methods in your classes to get collision shapes/positions
                // if (collisionChecker->isCollision(bullet->getBounds(), enemy->getBounds())) {
                //     bullet->bLive = false; // Deactivate bullet
                //     enemy->isEnmsLive = false; // Deactivate enemy
                //     // soundManager->playSound("explosion.wav"); // Play sound
                //     // Increase score, etc.
                // }
            }
        }

        // Player-Enemy Collisions
        for (_enms* enemy : enemies) {
             if (!enemy || !enemy->isEnmsLive || !player) continue;
            // if (collisionChecker->isCollision(player->getBounds(), enemy->getBounds())) {
            //     // Handle player hit: decrease health, game over, play sound etc.
            //     // soundManager->playSound("player_hit.wav");
            //     // player->takeDamage();
            //     // if (player->isDead()) { currentState = GAME_OVER; }
            // }
        }
        // Add other collision checks (Player vs Bullets?)
    }
    */

    // --- Cleanup Inactive Objects (Optional) ---
    // Bullets are reused via bLive flag, no cleanup needed if using pool
    // Enemies could be removed, but depends on game design (spawning new ones?)
    // enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
    //    [](const _enms* e){ if (!e->isEnmsLive) { delete e; return true; } return false; }
    // ), enemies.end());

}


GLint _scene::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer
    glLoadIdentity(); // reset modelview matrix


    switch (currentState)
    {
        case LANDING:
            glPushMatrix();
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_TEXTURE_2D);
                setOrthoProjection((int)dim.x, (int)dim.y); // Switch to 2D ortho view
                glColor3f(1.0, 1.0, 1.0);
                glBindTexture(GL_TEXTURE_2D, landingTextureID);
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
                    glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0);
                    glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y);
                    glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0); // Unbind
                glDisable(GL_TEXTURE_2D);
                restorePerspectiveProjection(); // Go back to 3D projection mode
                glEnable(GL_DEPTH_TEST);
            }
            glPopMatrix();
            break;

        case MENU:
            drawMenuScene(); // Use the function that draws background and black text
            break;

        case HELP:
            glPushMatrix();
            {
                glClearColor(0.0, 0.0, 0.8, 1.0); // Blue background
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glEnable(GL_TEXTURE_2D); // << Keep Enabled for drawText
                setOrthoProjection((int)dim.x, (int)dim.y);

                // Draw using white text on blue background
                drawText("How To Play", 100, 100, 1.0f, 1.0f, 1.0f);
                drawText("Move: Arrow Keys (Placeholder)", 100, 150, 1.0f, 1.0f, 1.0f);
                drawText("Shoot: Spacebar (Placeholder)", 100, 180, 1.0f, 1.0f, 1.0f);
                drawText("Press [M] to return to Menu", 100, dim.y - 100, 1.0f, 1.0f, 1.0f);

                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                restorePerspectiveProjection();
                glEnable(GL_DEPTH_TEST);
                glClearColor(1.0, 1.0, 1.0, 1.0); // Reset clear color
            }
            glPopMatrix();
            break;

        case GAME:
            glClearColor(0.0, 0.4, 0.0, 1.0); // Set desired background color
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with the color

            // --- Set up 3D View & States ---
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING); // Make sure lighting is on for 3D objects if needed
            glEnable(GL_TEXTURE_2D); // Enable textures for sprites
            glEnable(GL_BLEND); // Enable blending for transparency
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor3f(1.0, 1.0, 1.0); // Set default color modulation to white

            // --- Draw Background (Parallax) ---
            glPushMatrix();
            glDisable(GL_LIGHTING); // Background likely doesn't need lighting
            if (background) {
                // Use the function name from _parallax.h/cpp
                background->drawBackground(backgroundTextureID, dim.x, dim.y); // <<< EDITED
            }
            glEnable(GL_LIGHTING); // Re-enable if subsequent objects need it
            glPopMatrix();


            // --- Draw Player ---
            glPushMatrix();
             if (player) {
                 // Use the function name from _player.h/cpp
                 player->drawPlayer(playerTextureID); // <<< EDITED
             }
            glPopMatrix();

            // --- Draw Enemies ---  // (This was already working)
            for (_enms* enemy : enemies) {
                 if (enemy && enemy->isEnmsLive) {
                     glPushMatrix();
                    // Check _enms.h/cpp for draw function name
                    enemy->drawEnms(enemyTextureID); // <<< Ensure this is correct
                     glPopMatrix();
                 }
            }

             // --- Draw Bullets ---
            for (_bullets* bullet : bullets) {
                 if (bullet && bullet->bLive) { // Only draw if bullet is active
                     glPushMatrix();
                    // Check _bullets.h/cpp for draw function name
                    bullet->drawBullet(bulletTextureID); // <<< EDITED (Assuming name)
                     glPopMatrix();
                 }
            }

            // --- Clean up States ---
            // Don't disable texture/blend if UI elements (like score) need them later
            // Or, draw game elements first, then switch to ortho for UI
             glDisable(GL_BLEND);      // Disable blend after drawing transparent objects
             glDisable(GL_TEXTURE_2D); // Disable textures if no more textured objects
             // Keep Depth Test & Lighting enabled if needed for subsequent 3D draws or default state

            break; // End GAME case

        case PAUSED:
            // Draw GAME scene dimmed/unchanged (optional)
            //glClearColor(0.0, 0.4, 0.0, 1.0); // Match game background
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // <<< Consider drawing the actual game scene here first for a better pause effect >>>
            // <<< You would need to call the drawing logic from case GAME again here >>>
            // <<< BUT without calling updateGame() >>>

            // Draw the overlay popup menu (using ortho)
            glPushMatrix();
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST); // Draw popup on top
                glEnable(GL_BLEND);
                glEnable(GL_TEXTURE_2D); // Needed for drawText

                setOrthoProjection((int)dim.x, (int)dim.y);

                // Draw semi-transparent background quad for the popup
                glColor4f(0.0f, 0.0f, 0.0f, 0.75f); // Dark semi-transparent grey
                float popupWidth = 350; float popupHeight = 100;
                float popupX = (dim.x - popupWidth) / 2.0f;
                float popupY = (dim.y - popupHeight) / 2.0f;
                glBegin(GL_QUADS);
                    glVertex2f(popupX, popupY);
                    glVertex2f(popupX + popupWidth, popupY);
                    glVertex2f(popupX + popupWidth, popupY + popupHeight);
                    glVertex2f(popupX, popupY + popupHeight);
                glEnd();

                // Draw the confirmation text (using white)
                drawText("Quit Game?", popupX + 50, popupY + 20, 1.0f, 1.0f, 1.0f);
                drawText("Yes (Enter) / No (P)", popupX + 50, popupY + 50, 1.0f, 1.0f, 1.0f);

                // Clean up states for ortho overlay
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                restorePerspectiveProjection(); // Switch back to 3D view setup
                glEnable(GL_DEPTH_TEST); // Re-enable depth test for safety
            }
            glPopMatrix();
            break;


        // Add a default case for safety
        default:
            break;

    } // End switch(currentState)

    return true;
} // End drawScene

int _scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Pass input to the game input handler if in GAME state
    if (currentState == GAME && gameInputs) {
         // Let gameInputs handle mouse/keyboard specifically for the game
         // Or handle directly as below
         switch(uMsg) {
             case WM_KEYDOWN:
                  // Handle game actions like shooting
                 if (wParam == VK_SPACE) {
                     // Find an inactive bullet and fire it
                     for (_bullets* bullet : bullets) {
                         if (bullet && !bullet->bLive) {
                              // Get player's current position (ensure player pointer is valid)
                             vec3 playerPos = {0,0,0}; // Default
                             if(player) {
                                 // Assuming player->plPos holds the current position
                                 playerPos = player->plPos;
                                 // Potentially offset bullet start position slightly
                                 // playerPos.y += 0.1f; // Example offset
                             }
                             bullet->bReset(playerPos); // Reset bullet to player pos, sets state to SHOOT
                             bullet->bActions();        // Sets bLive = true in SHOOT state

                             // soundManager->playSound("shoot.wav"); // Play sound if sound manager exists
                             break; // Only fire one bullet per press
                         }
                     }
                 } else if (wParam == 'P') { // Pause Key
                     currentState = PAUSED;
                 } else {
                     // Handle player movement keys - Set actionTrigger in player
                     if (player) {
                         switch (wParam) {
                             case VK_LEFT:  player->actionTrigger = _player::LEFTWALK; break;
                             case VK_RIGHT: player->actionTrigger = _player::RIGHTWALK; break;
                             // Add UP/DOWN/JUMP/ATTACK triggers if needed
                             // case VK_UP: player->actionTrigger = _player::JUMP; break;
                         }
                     }
                     // Alternatively, pass key to gameInputs:
                     // gameInputs->keyDown(wParam);
                 }
                 return 0; // Indicate message was handled

             case WM_KEYUP:
                 // Handle stopping movement when key is released
                 if (player) {
                     switch (wParam) {
                         case VK_LEFT:
                             // If currently moving left, stop (go to STAND)
                             if (player->actionTrigger == _player::LEFTWALK) {
                                 player->actionTrigger = _player::STAND;
                             }
                             break;
                         case VK_RIGHT:
                             // If currently moving right, stop (go to STAND)
                             if (player->actionTrigger == _player::RIGHTWALK) {
                                 player->actionTrigger = _player::STAND;
                             }
                             break;
                         // Add other key up handlers if needed
                     }
                 }
                  // Alternatively, pass key to gameInputs:
                  // gameInputs->keyUp(wParam);
                 return 0; // Indicate message was handled

             case WM_MOUSEMOVE:
                 // Example: Aiming?
                 // gameInputs->mouseMove(player, LOWORD(lParam), HIWORD(lParam));
                 return 0;
             // Add other mouse messages if needed (WM_LBUTTONDOWN etc for game actions)

         }
    }

    // --- Handle UI State Transitions (mostly unchanged) ---
    switch (uMsg)
    {
        case WM_KEYDOWN:
             // State transitions based on current state
             if (currentState == LANDING) {
                 if (wParam == VK_RETURN) { currentState = MENU; }
             }
             else if (currentState == MENU) {
                 switch (wParam) {
                     case 'N': currentState = GAME; break;
                     case 'H': currentState = HELP; break;
                     case 'E': PostQuitMessage(0); break;
                     case VK_ESCAPE: currentState = LANDING; break; // Or PostQuitMessage(0);
                 }
             }
             else if (currentState == HELP) {
                 if (wParam == 'M' || wParam == VK_ESCAPE) {
                     currentState = MENU;
                 }
             }
             else if (currentState == GAME) {
                 // Pause ('P') handled in the game input section above
                 // Exit via ESC handled in main loop
             }
             else if (currentState == PAUSED) {
                 if (wParam == VK_RETURN) { // Yes to Quit
                      MessageBoxA(NULL, "DEBUG: Exiting via PAUSED Enter key", "Exit Trace", MB_OK);
                     PostQuitMessage(0);
                 } else if (wParam == 'P' || wParam == VK_ESCAPE) { // No to Quit (Resume)
                     currentState = GAME;
                 }
             }
             return 0; // Handled Keydown for UI state changes

        case WM_KEYUP:
             // If not handled by gameInputs or state changes above, break
            break;

        case WM_LBUTTONDOWN: // Mouse clicks for UI state changes
        {
             if (currentState == LANDING) {
                 currentState = MENU;
             }
             else if (currentState == MENU) {
                // Use the synchronized coordinates from previous fix
                int mouseX = LOWORD(lParam); int mouseY = HIWORD(lParam);
                POINT clickPoint = { mouseX, mouseY };
                float menuX = dim.x / 2.0f - 75.0f;
                float menuY_Start = dim.y / 2.0f - 50.0f;
                float menuY_Spacing = 60;
                float buttonWidth = 250; // Adjusted width
                float buttonHeight = 50; // Adjusted height
                RECT newGameRect = {(long)menuX, (long)menuY_Start, (long)(menuX + buttonWidth), (long)(menuY_Start + buttonHeight)};
                RECT helpRect = {(long)menuX, (long)(menuY_Start + menuY_Spacing), (long)(menuX + buttonWidth), (long)(menuY_Start + menuY_Spacing + buttonHeight)};


                if (PtInRect(&newGameRect, clickPoint)) { currentState = GAME; }
                else if (PtInRect(&helpRect, clickPoint)) { currentState = HELP; }
                // else if (PtInRect(&exitRect, clickPoint)) { PostQuitMessage(0); } // Exit click is optional

            }
            // Add mouse handling for GAME state here if needed (e.g., mouse aiming/shooting)
            // else if (currentState == GAME) { ... }

            return 0; // Handled LButtonDown for UI
        }

        default:
            break; // Let DefWindowProc handle other messages
    }

    // For messages not explicitly handled above, let DefWindowProc take over.
     return DefWindowProc(hWnd, uMsg, wParam, lParam); // Important for default processing
}


// --- Font Loading/Drawing --- (Keep these as they are)
bool _scene::loadFontData(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
         std::cerr << "Error: Could not open font data file: " << filename << std::endl;
         MessageBox(NULL, "ERROR: Could not open .fnt file!", "Font Data Error", MB_OK | MB_ICONERROR);
         return false;
    }
    std::string line; std::string key;
    fontDataMap.clear(); // Clear previous data
    fontTextureWidth = 0; fontTextureHeight = 0; fontLineHeight = 0; fontBaseHeight = 0;

    while (std::getline(file, line)) {
         std::stringstream ss(line);
         ss >> key;
         if (key == "common") {
             std::string valuePair;
             while (ss >> valuePair) {
                 size_t equalsPos = valuePair.find('=');
                 if (equalsPos != std::string::npos) {
                     std::string currentKey = valuePair.substr(0, equalsPos);
                     std::string currentValue = valuePair.substr(equalsPos + 1);
                     try {
                         if (currentKey == "base") fontBaseHeight = std::stof(currentValue);
                         else if (currentKey == "lineHeight") fontLineHeight = std::stof(currentValue);
                         else if (currentKey == "scaleW") fontTextureWidth = std::stof(currentValue);
                         else if (currentKey == "scaleH") fontTextureHeight = std::stof(currentValue);
                     } catch (...) { /* Handle exceptions */ std::cerr << "Font parse error (common)\n"; }
                 }
             }
         } else if (key == "char") {
             CharData charData; std::string valuePair;
             while (ss >> valuePair) {
                 size_t equalsPos = valuePair.find('=');
                 if (equalsPos != std::string::npos) {
                      std::string currentKey = valuePair.substr(0, equalsPos);
                      std::string currentValue = valuePair.substr(equalsPos + 1);
                      try {
                         if (currentKey == "id") charData.id = std::stoi(currentValue);
                         else if (currentKey == "x") charData.x = std::stof(currentValue);
                         else if (currentKey == "y") charData.y = std::stof(currentValue);
                         else if (currentKey == "width") charData.width = std::stof(currentValue);
                         else if (currentKey == "height") charData.height = std::stof(currentValue);
                         else if (currentKey == "xoffset") charData.xoffset = std::stof(currentValue);
                         else if (currentKey == "yoffset") charData.yoffset = std::stof(currentValue);
                         else if (currentKey == "xadvance") charData.xadvance = std::stof(currentValue);
                     } catch (...) { /* Handle exceptions */ std::cerr << "Font parse error (char)\n"; }
                 }
             }
             if (charData.id != 0 /*&& fontDataMap.find(charData.id) == fontDataMap.end()*/) { // Allow overwrite?
                 fontDataMap[charData.id] = charData;
             }
         }
    }
    file.close();
    if (fontTextureWidth == 0 || fontTextureHeight == 0 || fontDataMap.empty()) {
         MessageBox(NULL, "Warning: Font data might be missing or invalid (.fnt parsing).", "Font Data Warning", MB_OK | MB_ICONWARNING);
         // Consider returning false here if font is absolutely essential
    }
    return true;
}

void _scene::drawText(std::string text, float screenX, float screenY, float r, float g, float b) {
     if (fontTextureID == 0 || fontDataMap.empty() || fontTextureWidth == 0 || fontTextureHeight == 0) {
         return; // Can't draw if font texture or data isn't loaded
     }

    // Set up OpenGL states specifically for text rendering WITHIN this function
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureID); // Bind the font texture
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Ortho projection should be set by the caller (drawMenuScene, HELP, PAUSED cases)

    glColor3f(r, g, b); // Set text color

    float currentX = screenX;

    glBegin(GL_QUADS);
    for (char &c : text) {
         // Use .at() for bounds checking if needed, or .count()/.find()
         auto it = fontDataMap.find(c);
         if (it != fontDataMap.end()) { // Check if character exists in map
             CharData cd = it->second; // Get data using iterator
             float x1 = currentX + cd.xoffset;
             float y1 = screenY + cd.yoffset; // Y offset from baseline
             float x2 = x1 + cd.width;
             float y2 = y1 + cd.height;
             float u1 = cd.x / fontTextureWidth;
             float v1 = cd.y / fontTextureHeight;
             float u2 = (cd.x + cd.width) / fontTextureWidth;
             float v2 = (cd.y + cd.height) / fontTextureHeight;

             glTexCoord2f(u1, v1); glVertex2f(x1, y1); // Top-Left UV, Top-Left Pos
             glTexCoord2f(u2, v1); glVertex2f(x2, y1); // Top-Right UV, Top-Right Pos
             glTexCoord2f(u2, v2); glVertex2f(x2, y2); // Bottom-Right UV, Bottom-Right Pos
             glTexCoord2f(u1, v2); glVertex2f(x1, y2); // Bottom-Left UV, Bottom-Left Pos

             currentX += cd.xadvance;
         } else {
             // Handle character not found (e.g., draw '?' or just advance by space width)
             auto space_it = fontDataMap.find(' ');
             if (space_it != fontDataMap.end()) {
                 currentX += space_it->second.xadvance;
             }
             // else { currentX += default_advance_width; } // Use a default advance if space not found
         }
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the font texture
    // Don't disable texture/blend here, let the calling state do it.
}


// --- Menu Background Loading/Drawing --- (Keep these as they are)
bool _scene::loadMenuBackgroundTexture() {
    texLoader->loadTexture((char*)"images/menu_background.png");
    if (texLoader->tex == 0) {
        MessageBox(NULL, "Menu background texture failed to load", "Texture load error", MB_OK | MB_ICONERROR);
        return false;
    }
    menuBackgroundTextureID = texLoader->tex;
    glBindTexture(GL_TEXTURE_2D, menuBackgroundTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Or GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Or GL_NEAREST
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}


void _scene::drawMenuScene() {
    glPushMatrix();
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setOrthoProjection((int)dim.x, (int)dim.y);

        // Draw the menu background image
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // White, opaque
        glBindTexture(GL_TEXTURE_2D, menuBackgroundTextureID);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
            glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0);
            glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y);
            glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind

        // Draw the menu text using black
        float menuX = dim.x / 2.0f - 75.0f;
        float menuY_Start = dim.y / 2.0f - 50.0f;
        float menuY_Spacing = 60;
        drawText("New Game", menuX, menuY_Start, 0.0f, 0.0f, 0.0f); // Black text
        drawText("Help", menuX, menuY_Start + menuY_Spacing, 0.0f, 0.0f, 0.0f);
        drawText("Exit: Press E or Esc", menuX, menuY_Start + 2 * menuY_Spacing, 0.0f, 0.0f, 0.0f);

        // Clean up GL states set by this function
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        restorePerspectiveProjection();
        glEnable(GL_DEPTH_TEST); // Re-enable depth test for subsequent 3D drawing
    }
    glPopMatrix();
}
