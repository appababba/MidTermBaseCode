#include "_player.h"
#include <GL/gl.h> // Make sure OpenGL header is included

_player::_player()
{
    //ctor
    // Initialize position, scale, etc. here or in initPlayer
    plPos.x = 0.0f;
    plPos.y = -0.65f; // Default Y from old init
    plPos.z = -2.0f;  // Default Z from old init

    plScl.x = 0.25f; // Example scale - adjust as needed
    plScl.y = 0.25f;
    plScl.z = 1.0f; // Scale Z usually 1 for 2D sprites

    actionTrigger = STAND; // Start in STAND state
    xMin = yMin = xMax = yMax = 0.0f; // Initialize texture coords
    framesX = 1; // Default frame count
    framesY = 1; // Default frame count

    // Initialize Quad Vertices (local space)
    vert[0].x = -1.0; vert[0].y = -1.0; vert[0].z = 0.0; // Bottom-Left
    vert[1].x =  1.0; vert[1].y = -1.0; vert[1].z = 0.0; // Bottom-Right
    vert[2].x =  1.0; vert[2].y =  1.0; vert[2].z = 0.0; // Top-Right
    vert[3].x = -1.0; vert[3].y =  1.0; vert[3].z = 0.0; // Top-Left
}

_player::~_player()
{
    //dtor
    delete pTmer; // Delete timer if created with new
    pTmer = nullptr;
}

// <<< MODIFIED >>> Signature matches _player.h (no filename)
void _player::initPlayer(int xfrm, int yfrm)
{
    // get frame count
    framesX = xfrm;
    if (framesX <= 0) framesX = 1; // Prevent division by zero
    framesY = yfrm;
    if (framesY <= 0) framesY = 1; // Prevent division by zero

    actionTrigger = STAND; // set action to STAND initially

    //setup initial Texture Coords for STAND (assuming frame 0, 0)
    xMin = 0.0f;
    xMax = 1.0f / (float)framesX;
    yMin = 0.0f;                    // Assuming frame 0 is at the top row in sprite sheet
    yMax = 1.0f / (float)framesY;   // Use yMax consistently for bottom coord


}

void _player::drawPlayer(GLuint textureID)
{
    // Set color (optional, useful for effects or debugging)
    glColor3f(1.0, 1.0, 1.0);

    // Bind the texture using the passed ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    // <<< REMOVED >>> pTex->textureBinder();

    glPushMatrix();
        glTranslatef(plPos.x, plPos.y, plPos.z);
        glScalef(plScl.x, plScl.y, plScl.z);

        glBegin(GL_QUADS);
            // Define UV coords based on current animation frame (calculated in playerActions)
            glTexCoord2f(xMin, yMax); // Bottom-Left UV
            glVertex3f(vert[0].x, vert[0].y, vert[0].z); // Bottom-Left Pos

            glTexCoord2f(xMax, yMax); // Bottom-Right UV
            glVertex3f(vert[1].x, vert[1].y, vert[1].z); // Bottom-Right Pos

            glTexCoord2f(xMax, yMin); // Top-Right UV
            glVertex3f(vert[2].x, vert[2].y, vert[2].z); // Top-Right Pos

            glTexCoord2f(xMin, yMin); // Top-Left UV
            glVertex3f(vert[3].x, vert[3].y, vert[3].z); // Top-Left Pos
        glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

void _player::playerActions()
{
    // This function should update player state, position, AND texture coordinates (xMin, xMax, yMin, yMax) for animation
    float frameWidth = 1.0f / (float)framesX;
    float frameHeight = 1.0f / (float)framesY;

    switch(actionTrigger)
    {
        case STAND:
            xMin = 0.0f; // First frame
            xMax = frameWidth;
            yMin = 0.0f; // Assuming top row is standing anim
            yMax = frameHeight;
            // No movement for STAND
            break;

        case LEFTWALK:
            // Move player position
            plPos.x -= 0.01f; // <<< Adjust speed as needed

            // Update UVs based on timer
            if (pTmer && pTmer->getTicks() > 70) // Check timer
            {
                xMin += frameWidth; // Move to next frame
                xMax += frameWidth;
                // Assuming left walk animation is on the second row (index 1)
                yMin = frameHeight * 1.0f;
                yMax = frameHeight * 2.0f;

                // Wrap animation frame
                if (xMax > 1.0f) {
                    xMin = 0.0f;
                    xMax = frameWidth;
                }
                pTmer->reset(); // Reset timer for next frame
            }
            break;

        case RIGHTWALK:
             // Move player position
            plPos.x += 0.01f; // <<< Adjust speed as needed

            // Update UVs based on timer
            if (pTmer && pTmer->getTicks() > 70) // Check timer
            {
                xMin += frameWidth; // Move to next frame
                xMax += frameWidth;
                yMin = frameHeight * 0.0f;
                yMax = frameHeight * 1.0f;

                // Wrap animation frame
                if (xMax > 1.0f) {
                    xMin = 0.0f;
                    xMax = frameWidth;
                }
                pTmer->reset(); // Reset timer for next frame
            }
            break;

    }
}
