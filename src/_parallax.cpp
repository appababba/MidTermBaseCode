#include "_parallax.h"
#include <GL/gl.h> // Make sure OpenGL header is included for glBindTexture etc.

_parallax::_parallax()
{
    //ctor
    xMax = 1.0;
    xMin = 0.0;
    yMax = 1.0;
    yMin = 0.0;
    speed = 0.005;
}

_parallax::~_parallax()
{
    //dtor
    // Make sure you are NOT deleting the 'background' pointer here
    delete tmr; // Delete the timer if created with new
    tmr = nullptr;
}

// <<< MODIFIED >>> Signature matches header, uses textureID parameter
void _parallax::drawBackground(GLuint textureID, float width, float height)
{
    glColor3f(1.0,1.0,1.0); // Set color before binding texture

    // <<< MODIFIED >>> Bind the texture using the passed ID
    glBindTexture(GL_TEXTURE_2D, textureID);



    // Aspect ratio correction might be needed depending on projection setup
    // This calculation (-width/height) assumes a perspective projection centered at 0.
    // Adjust if using ortho or different view setup.
    float aspectRatio = width / height;
    float drawWidth = 10.0f * aspectRatio; // Example: Assume background spans view width of 2*aspectRatio at z=-30
    float drawHeight = 6.0f;             // Example: Assume background spans view height of 2 at z=-30
    float drawZ = -10.0f;
    glBegin(GL_QUADS); // Use GL_QUADS for rectangles
        glTexCoord2f(xMin,yMax);
        glVertex3f(-drawWidth/2.0f, -drawHeight/2.0f, drawZ); // Bottom-Left

        glTexCoord2f(xMax,yMax);
        glVertex3f( drawWidth/2.0f, -drawHeight/2.0f, drawZ); // Bottom-Right

        glTexCoord2f(xMax,yMin);
        glVertex3f( drawWidth/2.0f,  drawHeight/2.0f, drawZ); // Top-Right

        glTexCoord2f(xMin,yMin);
        glVertex3f(-drawWidth/2.0f,  drawHeight/2.0f, drawZ); // Top-Left
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

// <<< MODIFIED >>> Signature matches header, body removed
void _parallax::initPrlx()
{
    // <<< REMOVED >>> background->loadTexture(fileName);
    // No longer loads texture here. Add other init logic if needed.
}

void _parallax::scroll(bool Auto, string dir, float scrollSpeed) // Changed parameter name from speed to scrollSpeed
{
 //  if(Auto) // Keep auto-scroll logic if desired
   {
       if(tmr && tmr->getTicks()>50) // Check if tmr is not null
       {
            // Use scrollSpeed parameter instead of member variable 'speed' if passed in
            float effectiveSpeed = scrollSpeed; // Or keep using member: float effectiveSpeed = speed;

            if(dir=="up")       { yMin += effectiveSpeed; yMax += effectiveSpeed; }
            else if(dir=="down")  { yMin -= effectiveSpeed; yMax -= effectiveSpeed; }
            else if(dir=="right") { xMin += effectiveSpeed; xMax += effectiveSpeed; }
            else if(dir=="left")  { xMin -= effectiveSpeed; xMax -= effectiveSpeed; }

            // Optional: Add wrapping logic for texture coordinates if they go beyond 0.0 or 1.0
            // e.g., if (xMin < -1.0f) { xMin += 1.0f; xMax += 1.0f; }
            //      if (xMin > 1.0f)  { xMin -= 1.0f; xMax -= 1.0f; }
            //      ... similar for yMin/yMax

            tmr->reset();
       }
   }
}
