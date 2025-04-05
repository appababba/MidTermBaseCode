#include "_scene.h"
#include <windows.h>
#include <GL/glu.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

_scene::_scene()
{
    currentState = LANDING; // start at the landing page
    texLoader = new _textureLoader(); // create the texture loader instance
    landingTextureID = 0;      // initialize texture ID to 0
    dim.x = 0;                 // initialize dimensions
    dim.y = 0;
    fontTextureID = 0;
}

_scene::~_scene()
{
    delete texLoader; // delete texture loader instance
    texLoader = nullptr;//null danging pointers
}


GLint _scene::initGL()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    dim.x = GetSystemMetrics(SM_CXSCREEN);
    dim.y = GetSystemMetrics(SM_CYSCREEN);

    // --- Ensure Texture Loader Exists (ONCE) ---
    if (!texLoader) {
        MessageBox(NULL, "Texture loader object not created", "Init error", MB_OK | MB_ICONERROR);
        return false;
    }

    // --- Load Font Texture & Set Params ---
    texLoader->loadTexture((char*)"images/retro_deco.png"); // 1. Load
    fontTextureID = texLoader->tex;                      // 2. Get ID

    if (fontTextureID == 0) {                            // 3. Check ID
        MessageBox(NULL, "Font Texture failed to load", "Texture load error", MB_OK | MB_ICONERROR);
        // Consider: return false;
    } else {                                             // 4. Set Params (if ID valid)
        glBindTexture(GL_TEXTURE_2D, fontTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use linear filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use linear filter
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind
    }

    // --- Load Landing Page Texture & Set Params ---
    texLoader->loadTexture((char*)"images/landing_page.png"); // 1. Load
    landingTextureID = texLoader->tex;                     // 2. Get ID

    if (landingTextureID == 0) {                           // 3. Check ID
        MessageBox(NULL, "Landing page texture failed to load", "Texture load error", MB_OK | MB_ICONERROR);
        // Consider: return false;
    } else {                                            // 4. Set Params (optional for landing page)
         glBindTexture(GL_TEXTURE_2D, landingTextureID);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glBindTexture(GL_TEXTURE_2D, 0); // Unbind
    }

    // --- Load Font Data ---
    if (!loadFontData("images/retro_deco.fnt")) {
        MessageBox(NULL, "ERROR: Failed to load font data!", "Font Data Error", MB_OK | MB_ICONERROR);
       return false;
    }

    return true; // Success
}

void _scene::reSize(GLint width, GLint height)
{
    if (height == 0) height = 1;
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    // keep track of the resize window
    glViewport(0,0,width,height); // adjusting the viewport
    glMatrixMode(GL_PROJECTION);  // perspective projection settings
    glLoadIdentity();             // identity matrix
    gluPerspective(45.0f,aspectRatio,0.1f,100.0f); // projection settings

    glMatrixMode(GL_MODELVIEW);   // camera and model settings
    glLoadIdentity();             // identity matrix

    dim.x = width;
    dim.y = height;
}

void _scene::setOrthoProjection(int width, int height)
{
    glMatrixMode(GL_PROJECTION);        // select projection matrix
    glPushMatrix();                     // save current projection matrix (likely perspective)
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, width, height, 0);    // set ortho mode
    glMatrixMode(GL_MODELVIEW);         // select modelview matrix
}

//brings back saved projection
void _scene::restorePerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION); // choose projection matrix
    glPopMatrix();              // get old projection matrix
    glMatrixMode(GL_MODELVIEW); // select modelview matrix
}

void _scene::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer
    glLoadIdentity(); // reset  modelview matrix

    // draw based on the current state
    switch (currentState)
    {
        case LANDING:
            //draw Landing Page
            glPushMatrix(); // save current modelview matrix state
            {
                // set up rendering states 2D textured quad
                glDisable(GL_LIGHTING);   // turn off lighting for 2D UI
                glDisable(GL_DEPTH_TEST); // turn off depth testing for 2D UI
                glEnable(GL_TEXTURE_2D);  // make sure texturing is enabled

                setOrthoProjection((int)dim.x, (int)dim.y); //switch to 2D ortho view

                glColor3f(1.0, 1.0, 1.0); // set color to white to show texture correctly
                glBindTexture(GL_TEXTURE_2D, landingTextureID); //activate landing page texture

                // draw rectangle covering whole screen
                // texture coordinates map image corners
                // vertex coordinates map screen corners
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
                    glTexCoord2f(1.0, 0.0); glVertex2f(dim.x, 0);
                    glTexCoord2f(1.0, 1.0); glVertex2f(dim.x, dim.y);
                    glTexCoord2f(0.0, 1.0); glVertex2f(0, dim.y);
                glEnd();

                restorePerspectiveProjection(); // go back to 3D projection mode

                // reenable states needed for other parts of the game
                glEnable(GL_DEPTH_TEST);
                // glEnable(GL_LIGHTING); // only if needed
                glDisable(GL_TEXTURE_2D); // disable textures unless needed by next state
            }
            glPopMatrix(); // bring back modelview matrix state
            break;

        case MENU:
            { // Use braces for scope within the case

                // 1. Set up OpenGL state for 2D drawing
                glDisable(GL_LIGHTING);   // Turn off lighting
                glDisable(GL_DEPTH_TEST); // Turn off depth testing
                glEnable(GL_BLEND);       // Make sure alpha blending is on
                glEnable(GL_TEXTURE_2D);  // Make sure texturing is on

                // 2. Set Orthographic Projection (2D View)
                setOrthoProjection((int)dim.x, (int)dim.y);


                float menuX = 350; // Example X position
                float menuY_Start = 200; // Example starting Y position
                float menuY_Spacing = 50; // Example spacing between items

                drawText("New Game", menuX, menuY_Start);
                drawText("Help", menuX, menuY_Start + menuY_Spacing);
                drawText("Exit: Press E or esc", menuX, menuY_Start + 2 * menuY_Spacing);

                // 4. Restore Perspective Projection and OpenGL states
                restorePerspectiveProjection(); // Switch back to 3D view

                glEnable(GL_DEPTH_TEST); // Re-enable depth testing
                // glEnable(GL_LIGHTING); // Re-enable lighting if needed for other states
                glDisable(GL_TEXTURE_2D); // Disable textures unless the next state needs them
            }
            break; // --- End of MENU case ---

        case HELP:
            glClearColor(0.0, 0.0, 0.8, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(1.0, 1.0, 1.0, 1.0);
                // 1. Set up OpenGL state for 2D drawing
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glEnable(GL_TEXTURE_2D); // << Enable texture for font drawing

                // 2. Set Orthographic Projection (2D View)
                setOrthoProjection((int)dim.x, (int)dim.y);

                // 3. Draw the Help Text using your drawText function
                //    Adjust X, Y coordinates as needed.
                //    (You might want a different text color/size later)
                drawText("How To Play", 100, 100); // Example Title
                drawText("Move: Arrow Keys (Placeholder)", 100, 150); // Example instruction
                drawText("Shoot: Spacebar (Placeholder)", 100, 180); // Example instruction
                // ... Add more instructions as needed ...

                drawText("Press [M] to return to Menu", 100, dim.y - 100); // Instructions near bottom

                // 4. Restore Perspective Projection and OpenGL states
                restorePerspectiveProjection();

                glEnable(GL_DEPTH_TEST);
                // glEnable(GL_LIGHTING);
                glDisable(GL_TEXTURE_2D); // Disable textures
            break;

        case GAME:
            glClearColor(0.0, 0.4, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // glClearColor(1.0, 1.0, 1.0, 1.0); // Reset default

            // --- EXAMPLE: Draw a simple rotating cube ---
            glEnable(GL_LIGHTING); // Make sure lighting is on if needed
            glEnable(GL_COLOR_MATERIAL); // Simple way to color without full materials

            glPushMatrix(); // Save matrix state

            // Position the cube
            glTranslatef(0.0f, 0.0f, -6.0f); // Move it back so we can see it

            // Optional: Add rotation (you'd need a member variable like 'angle' and update it)
            // static float angle = 0.0f;
            // glRotatef(angle, 1.0f, 1.0f, 0.0f); // Rotate on X and Y axes
            // angle += 0.5f; // Increment angle for next frame (need an update function)

            // Draw the cube
            glBegin(GL_QUADS);
                // Front Face (Red)
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f,  1.0f);
                glVertex3f( 1.0f, -1.0f,  1.0f);
                glVertex3f( 1.0f,  1.0f,  1.0f);
                glVertex3f(-1.0f,  1.0f,  1.0f);
                // Back Face (Green)
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f,  1.0f, -1.0f);
                glVertex3f( 1.0f,  1.0f, -1.0f);
                glVertex3f( 1.0f, -1.0f, -1.0f);
                // Top Face (Blue)
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex3f(-1.0f,  1.0f, -1.0f);
                glVertex3f(-1.0f,  1.0f,  1.0f);
                glVertex3f( 1.0f,  1.0f,  1.0f);
                glVertex3f( 1.0f,  1.0f, -1.0f);
                // Bottom Face (Yellow)
                glColor3f(1.0f, 1.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f( 1.0f, -1.0f, -1.0f);
                glVertex3f( 1.0f, -1.0f,  1.0f);
                glVertex3f(-1.0f, -1.0f,  1.0f);
                // Right face (Magenta)
                glColor3f(1.0f, 0.0f, 1.0f);
                glVertex3f( 1.0f, -1.0f, -1.0f);
                glVertex3f( 1.0f,  1.0f, -1.0f);
                glVertex3f( 1.0f,  1.0f,  1.0f);
                glVertex3f( 1.0f, -1.0f,  1.0f);
                // Left Face (Cyan)
                glColor3f(0.0f, 1.0f, 1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f,  1.0f);
                glVertex3f(-1.0f,  1.0f,  1.0f);
                glVertex3f(-1.0f,  1.0f, -1.0f);
            glEnd();

            glPopMatrix(); // Restore matrix state
            // --- END EXAMPLE ---

            break; // End GAME case

         case PAUSED:
             glClearColor(0.0, 0.4, 0.0, 1.0); // Match game background
             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
             // glClearColor(1.0, 1.0, 1.0, 1.0); // Reset default


             // 2. Draw the overlay popup menu
             glPushMatrix(); // Save the current modelview matrix state
             {
                 // Set up for 2D drawing over the scene
                 glDisable(GL_LIGHTING);
                 glDisable(GL_DEPTH_TEST); // Draw popup on top of everything
                 glEnable(GL_BLEND);
                 glEnable(GL_TEXTURE_2D); // Needed for drawText

                 setOrthoProjection((int)dim.x, (int)dim.y); // Switch to 2D view
                 glLoadIdentity();                          // Reset modelview matrix for 2D

                 // Optional: Draw a semi-transparent background quad for the popup
                 //glColor4f(0.0f, 0.0f, 0.0f, 0.75f); // Dark semi-transparent grey
                 glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

                 float popupWidth = 350;  // Adjust size as needed
                 float popupHeight = 100; // Adjust size as needed
                 float popupX = (dim.x - popupWidth) / 2.0f; // Center horizontally
                 float popupY = (dim.y - popupHeight) / 2.0f; // Center vertically
                 glBegin(GL_QUADS);
                     glVertex2f(popupX, popupY);
                     glVertex2f(popupX + popupWidth, popupY);
                     glVertex2f(popupX + popupWidth, popupY + popupHeight);
                     glVertex2f(popupX, popupY + popupHeight);
                 glEnd();


                 // Draw the confirmation text (adjust text color/position)
                  glColor3f(1.0, 1.0, 1.0); // Example: White text

                 drawText("Quit Game?", popupX + 50, popupY + 20); // Adjust position
                 drawText("Yes (Enter) / No (P)", popupX + 50, popupY + 50); // Adjust position

                 restorePerspectiveProjection(); // Switch back to 3D view

                 // Restore states needed for 3D rendering (in case game loop continues)
                 glEnable(GL_DEPTH_TEST);
                 // Re-enable lighting only if the GAME state uses it
                 // glEnable(GL_LIGHTING);
                 glDisable(GL_TEXTURE_2D); // Usually disable after 2D UI drawing
             }
             glPopMatrix(); // Restore the original modelview matrix state
             break;


        // Add a default case for safety
        default:
             break;

    } // End switch(currentState)
} // End drawScene

int _scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_KEYDOWN:
            if (currentState == LANDING)
            {
                if (wParam == VK_RETURN)
                {
                    currentState = MENU;
                }
            }

            else if (currentState == MENU)
            {
                switch (wParam)
                {
                    case 'N':
                        currentState = GAME;
                        break;
                    case 'H':
                        currentState = HELP;
                        break;
                    case 'E':
                        MessageBoxA(NULL, "DEBUG: Exiting via MENU 'E' key", "Exit Trace", MB_OK); // ADD THIS
                        PostQuitMessage(0);
                        break;
                    case VK_ESCAPE:
                        currentState = LANDING;
                        break;
                }
            }

            else if (currentState == HELP)
            {
             //   MessageBoxA(NULL, "WM_KEYDOWN: State is HELP.", "Debug Step 1", MB_OK | MB_ICONINFORMATION);

                if (wParam == 'M')
                {
                   // MessageBoxA(NULL, "WM_KEYDOWN: HELP detected VK_ESCAPE!", "Debug Step 2", MB_OK | MB_ICONINFORMATION);

                    currentState = MENU;

                   // MessageBoxA(NULL, "WM_KEYDOWN: State set to MENU.", "Debug Step 3", MB_OK | MB_ICONINFORMATION);
                }
            }

            else if (currentState == GAME)
            {
                if (wParam == 'P')
                {
                    currentState = PAUSED;
                }
            }

            else if (currentState == PAUSED)
            {
                if (wParam == VK_RETURN)
                {
                    MessageBoxA(NULL, "DEBUG: Exiting via PAUSED Enter key", "Exit Trace", MB_OK);
                    PostQuitMessage(0);
                }
                else if (wParam == 'P')
                {
                    currentState = GAME;
                }
            }

            return 0;

        case WM_KEYUP:
            break;

        case WM_LBUTTONDOWN:
        {
            if (currentState == LANDING)
            {
                currentState = MENU;
            }
            else if (currentState == MENU)
            {
                //MessageBoxA(NULL, "DEBUG: Exiting via MENU Exit Button", "Exit Trace", MB_OK); // ADD THIS
                //PostQuitMessage(0);
                int mouseX = LOWORD(lParam);
                int mouseY = HIWORD(lParam);
                POINT clickPoint = { mouseX, mouseY };

                float menuX = 350;
                float menuY_Start = 200;
                float menuY_Spacing = 50;
                float buttonWidth = 150;
                float buttonHeight = 40;

                RECT newGameRect = {
                    (long)menuX,
                    (long)menuY_Start,
                    (long)(menuX + buttonWidth),
                    (long)(menuY_Start + buttonHeight)
                };

                RECT helpRect = {
                    (long)menuX,
                    (long)(menuY_Start + menuY_Spacing),
                    (long)(menuX + buttonWidth),
                    (long)(menuY_Start + menuY_Spacing + buttonHeight)
                };

                RECT exitRect = {
                    (long)menuX,
                    (long)(menuY_Start + 2 * menuY_Spacing),
                    (long)(menuX + buttonWidth),
                    (long)(menuY_Start + 2 * menuY_Spacing + buttonHeight)
                };

                if (PtInRect(&newGameRect, clickPoint))
                {
                    currentState = GAME;
                }
                else if (PtInRect(&helpRect, clickPoint))
                {
                    currentState = HELP;
                }
                else if (PtInRect(&exitRect, clickPoint))
                {
                    PostQuitMessage(0);
                }
            }
            break;
        }

        default:
            break;
    }

    return 0;
}
























bool _scene::loadFontData(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open font data file: " << filename << std::endl;
        MessageBox(NULL, "ERROR: Could not open .fnt file!", "Font Data Error", MB_OK | MB_ICONERROR);
        return false;
    }

    std::string line;
    std::string key;
    std::string valueStr; // Store value part as string initially

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> key; // Read the first word (the key)

        if (key == "common") {
            // Parse base, lineHeight, scaleW, scaleH
            std::string valuePair;
            while (ss >> valuePair) { // Read key=value pairs
                 size_t equalsPos = valuePair.find('=');
                 if (equalsPos != std::string::npos) {
                     std::string currentKey = valuePair.substr(0, equalsPos);
                     std::string currentValue = valuePair.substr(equalsPos + 1);
                     try {
                        if (currentKey == "base") fontBaseHeight = std::stof(currentValue);
                        else if (currentKey == "lineHeight") fontLineHeight = std::stof(currentValue);
                        else if (currentKey == "scaleW") fontTextureWidth = std::stof(currentValue);
                        else if (currentKey == "scaleH") fontTextureHeight = std::stof(currentValue);
                     } catch (const std::invalid_argument& ia) {
                         std::cerr << "Warning: Invalid number format in common line: " << currentValue << std::endl;
                     } catch (const std::out_of_range& oor) {
                         std::cerr << "Warning: Number out of range in common line: " << currentValue << std::endl;
                     }
                 }
            }
        } else if (key == "char") {
            CharData charData;
            std::string valuePair;
            while (ss >> valuePair) { // Read key=value pairs like id=32 x=10 ...
                size_t equalsPos = valuePair.find('=');
                if (equalsPos != std::string::npos) {
                    std::string currentKey = valuePair.substr(0, equalsPos);
                    std::string currentValue = valuePair.substr(equalsPos + 1);
                    try {
                        // Convert value string to appropriate type
                        if (currentKey == "id") charData.id = std::stoi(currentValue);
                        else if (currentKey == "x") charData.x = std::stof(currentValue);
                        else if (currentKey == "y") charData.y = std::stof(currentValue);
                        else if (currentKey == "width") charData.width = std::stof(currentValue);
                        else if (currentKey == "height") charData.height = std::stof(currentValue);
                        else if (currentKey == "xoffset") charData.xoffset = std::stof(currentValue);
                        else if (currentKey == "yoffset") charData.yoffset = std::stof(currentValue);
                        else if (currentKey == "xadvance") charData.xadvance = std::stof(currentValue);
                        // Add page, chnl if needed
                    } catch (const std::invalid_argument& ia) {
                         std::cerr << "Warning: Invalid number format in char line for key " << currentKey << ": " << currentValue << std::endl;
                    } catch (const std::out_of_range& oor) {
                         std::cerr << "Warning: Number out of range in char line for key " << currentKey << ": " << currentValue << std::endl;
                    }
                }
            }
            // Store the parsed data in the map
            if (charData.id != 0) { // Make sure we got a valid ID
               fontDataMap[charData.id] = charData;
            }
        }
        // Ignore other line types like "info", "page", "kernings" for now
    }

    file.close();

    // Basic check if data seems valid
    if (fontTextureWidth == 0 || fontTextureHeight == 0 || fontDataMap.empty()) {
         MessageBox(NULL, "Warning: Font data might be missing or invalid (.fnt parsing).", "Font Data Warning", MB_OK | MB_ICONWARNING);
         // return false; // Optionally fail if critical data is missing
    }

    return true;
}

void _scene::drawText(std::string text, float screenX, float screenY) {
    if (fontTextureID == 0 || fontDataMap.empty() || fontTextureWidth == 0 || fontTextureHeight == 0) {
        return; // Can't draw if font texture or data isn't loaded
    }

    // Set up OpenGL states for text rendering
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glEnable(GL_BLEND); // Ensure blending is enabled for transparency in PNG
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDisable(GL_DEPTH_TEST); // Usually disable depth test for UI text
    // glDisable(GL_LIGHTING); // Lighting not needed for text

    // Assume Ortho projection is already set by the calling code (in drawScene)
    // Assume ModelView matrix is reset (glLoadIdentity)

    glColor3f(0.0, 0.0, 0.0); // Set text color (e.g., Black) - CHANGE AS NEEDED

    float currentX = screenX; // Starting X position

    glBegin(GL_QUADS); // Draw characters as quads

    for (char &c : text) { // Loop through each character in the string
        if (fontDataMap.count(c)) { // Check if character data exists
            CharData cd = fontDataMap[c];

            // Calculate screen coordinates for the quad
            float x1 = currentX + cd.xoffset;
            float y1 = screenY + cd.yoffset;
            float x2 = x1 + cd.width;
            float y2 = y1 + cd.height;

            // Calculate texture coordinates (normalize based on atlas size)
            float u1 = cd.x / fontTextureWidth;
            float v1 = cd.y / fontTextureHeight;
            float u2 = (cd.x + cd.width) / fontTextureWidth;
            float v2 = (cd.y + cd.height) / fontTextureHeight;

            // Define quad vertices and texture coordinates
            glTexCoord2f(u1, v1); glVertex2f(x1, y1); // Top-Left
            glTexCoord2f(u2, v1); glVertex2f(x2, y1); // Top-Right
            glTexCoord2f(u2, v2); glVertex2f(x2, y2); // Bottom-Right
            glTexCoord2f(u1, v2); glVertex2f(x1, y2); // Bottom-Left

            // Advance draw position for next character
            currentX += cd.xadvance;
        } else {
            // Character not in map - maybe draw a '?' or just advance
            // currentX += some_default_advance; // Need a default width
        }
    }

    glEnd(); // Finish drawing quads

    glDisable(GL_TEXTURE_2D); // Disable texture after use (optional)
    // Restore other states if needed (depth test, lighting) in the calling function
}
