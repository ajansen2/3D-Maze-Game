#define GL_SILENCE_DEPRECATION  // Suppress OpenGL deprecation warnings on macOS

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#define M_PI 3.14159265358979323846 // Window doesn't seem to define this
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>

/*
 
 Notes for Mac Users
 
 windows.h is not needed
 conio.h is not needed (for Mac or Windows users)
 
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <GLUT/glut.h>
 #include <math.h>
 
 Your program must use (also works with Windows)
 
 int main(int argc, char** argv)
 {
    glutInit( &argc, argv );
 
 
 Comment out the #define M_PI 3.14159265358979323846 since it is already present
 
 
 When compiling you need to include the appropriate frameworks:
 
 clang++ Circle.cpp -framework GLUT -framework OpenGL -o Circle -Wno-deprecated-declarations
 
 The -Wno-deprecation-declarations keep from displaying warnings that GLUT is outdated.
 
 
*/



// Maze representation
// 0 for open path, 1 for wall, 2 for target box
int maze[20][20] = {
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 2, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 2, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 2, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1},
    {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Player attributes
int playerX = 1, playerZ = 1;
float playerRotY = 0.0f;
float dx = 0, dz = 1;  // Direction in which player is looking
bool canPassThroughWalls = false;
bool isBlueBoxActive = false;   // True when the blue box has been fired and is still active
int blueBoxX, blueBoxZ;         // Current position of the blue box
float blueBoxHeight = 0.0f;     // Y position (to simulate bouncing)
int blueBoxTTL;                 // Time-To-Live for the blue box
float blueBoxDx, blueBoxDz;     // Direction in which blue box is moving
float amplitude = 1.0f;         // Amplitude for bouncing
bool isWireframe = false;       // Toggle for wireframe mode
bool wallsColorToggle = false;  // Toggle to give a visual cue when walls can be passed through
int redBoxCount = 0;            // Initialize the redBoxCount
float bounceTime = 0.0f;         // Time variable for controlling bounce frequency
float blueBoxVelY = 0.1f; // Vertical velocity of the blue box
const float GRAVITY = -0.005f; // gravity pulling the blue box down
// Additional variables for the sine wave based bouncing
float blueBoxSinePos = 0.0f; // The current position in the sine wave
const float BOUNCE_SPEED = 0.1f; // The speed at which the blue box bounces

// Additional missing variables
GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[] = {50.0f};
float angle = 0.0f;  // Rotation angle for target boxes


// Initialization function
void init() {
    glEnable(GL_DEPTH_TEST);

    // Lighting setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets color to black
    glDisable(GL_CULL_FACE);


    // Count the number of target boxes during initialization
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (maze[i][j] == 2) {
                redBoxCount++;
            }
        }
    }

}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set the camera to follow the player with a slight offset in the y-direction for a first-person view
    gluLookAt(playerX, 0.5, playerZ, playerX + dx, 0.5, playerZ + dz, 0, 1, 0);
    
    // Lighting setup
    GLfloat light_pos[] = {playerX + 2.0f, 3.0f, playerZ + 2.0f, 1.0f};  // light source relative to player position
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

    glShadeModel(GL_SMOOTH);
    

    glEnable(GL_DEPTH_TEST);

    
    glPushMatrix(); //Push matrix for the maze
    // Draw the maze
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            glPushMatrix();
            glTranslatef(i, 0.5, j);  // Translate to the appropriate location
        
       switch (maze[i][j]) {
            case 1:  // Wall
                glColor3f(0.8, 0.8, 0.8);  // Light gray for walls
                glutSolidCube(1.0f);
                break;
            case 2:  // Target box
                glColor3f(1.0, 0.0, 0.0);  // Red for target boxes
                glScalef(1.0f, 0.5f, 0.5f); // This line scales the box
                glRotatef(angle, 0, 1, 0);  // Rotate the target box
                glutSolidCube(1.0f);
                break;
            case 3:  // Green wall (game completed state)
               glColor3f(0.0, 1.0, 0.0);  // Green
               glutSolidCube(1.0f);
                break;
           case 4: // Green targetbox if hit
               glColor3f(0.0, 1.0, 0.0); //Green
               glScalef(1.0f, 0.5f, 0.5f); // This line scales the box
               glRotatef(angle, 0, 1 , 0); //Rotate the green
               glutSolidCube(1.0f);
            default:
                break;  // No drawing for open paths
            }
            glPopMatrix(); // Reset transformations for the next block
        }
    }
    glPopMatrix(); // Poop matrix for the maze
            
    if (isBlueBoxActive) {
        glPushMatrix(); // Push matrix for the blue box
        glColor3f(0.0, 0.0, 1.0);  // Blue color for the fired box
        glTranslatef(blueBoxX, blueBoxHeight, blueBoxZ);
        glutSolidCube(0.5f);      // Half the size of a normal block
        glPopMatrix(); //Pop matrix for the blue box

        // Check for collisions with target boxes
        if (maze[blueBoxX][blueBoxZ] == 2) {
            maze[blueBoxX][blueBoxZ] = 4;  // Remove the target box (or change its color)
            isBlueBoxActive = false;       // Destroy the blue box
            redBoxCount--;                 // Decrement the count of red boxes

        // Visual cue when walls can be passed through
        if (canPassThroughWalls) {
            wallsColorToggle = !wallsColorToggle;
            if (wallsColorToggle) {
                glColor3f(1.0, 1.0, 0.0);  // Change to yellow for indication
            } else {
                glColor3f(1.0, 0.0, 0.0);  // Regular red
            }
        }
            // Check if all target boxes are hit
            if (redBoxCount == 0) {
                // Change maze walls to green
                for (int i = 0; i < 20; i++) {
                    for (int j = 0; j < 20; j++) {
                        if (maze[i][j] == 1) {
                            maze[i][j] = 3;  // Represent green walls with the value '3'
                        }
                    }
                }
            }
        }
    

        // Check for collisions with walls
        if (maze[blueBoxX][blueBoxZ] == 1) {
            isBlueBoxActive = false;  // Destroy the blue box
        }
    }
    glutSwapBuffers();
}
// Reshape Function
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width / (GLfloat)height, 0.1f, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool isColliding(int x1, int z1, int x2, int z2) {
    // Assuming the cube size is 1. Adjust if different.
    float halfSize = 0.5f;
    return (x1 + halfSize > x2 - halfSize) &&
           (x1 - halfSize < x2 + halfSize) &&
           (z1 + halfSize > z2 - halfSize) &&
           (z1 - halfSize < z2 + halfSize);
}

// Timer function
void timer(int value) {
    angle += 1.0f;
    if (angle > 360.0f) {
        angle -= 360.0f;
    }
    if (isBlueBoxActive) {
        // Move the blue box forward
         blueBoxX += blueBoxDx;
         blueBoxZ += blueBoxDz;

         // Update the sine position
         blueBoxSinePos += BOUNCE_SPEED;

         // Calculate the bouncing height using the sine function
         blueBoxHeight = 0.25f + amplitude * std::abs(sin(blueBoxSinePos));

         // If we've completed a full oscillation (which is approximately 6.28 in radians or 2*PI), reduce the amplitude
         if (blueBoxSinePos > 6.28f) {
             blueBoxSinePos = 0.0f; // Reset sine position
             amplitude *= 0.7f; // Reduce the amplitude by 30% with each bounce

             // If the amplitude is too small, deactivate the blue box
             if (amplitude < 0.05f) {
                 isBlueBoxActive = false;
             }
         }
        blueBoxTTL--; // Decrement the TTL
        if (blueBoxTTL <= 0) {
            isBlueBoxActive = false; // Deactivate the blue box when TTL expires
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // 60 FPS approx
}


// Handle arrow keys for movement
void specialKeys(int key, int x, int y) {
    int tmpX = playerX, tmpZ = playerZ;

    switch (key) {
        case GLUT_KEY_UP:
            tmpX += dx;
            tmpZ += dz;
            break;

        case GLUT_KEY_DOWN:
            tmpX -= dx;
            tmpZ -= dz;
            break;

        case GLUT_KEY_LEFT:
            playerRotY -= 5.0f;
            dx = sin(playerRotY * M_PI / 180);
            dz = -cos(playerRotY * M_PI / 180);
            break;

        case GLUT_KEY_RIGHT:
            playerRotY += 5.0f;
            dx = sin(playerRotY * M_PI / 180);
            dz = -cos(playerRotY * M_PI / 180);
            break;
    }

    // Check for walls (if the toggle is off)
    if (!canPassThroughWalls && (tmpX < 0 || tmpX >= 20 || tmpZ < 0 || tmpZ >= 20 || maze[tmpX][tmpZ] == 1)) {
        return;
    }

    playerX = tmpX;
    playerZ = tmpZ;
    printf("Player Position: (%d, %d)\n", playerX, playerZ);
    glutPostRedisplay();
}

// Handle other keyboard inputs like space for firing and 'w' for wall toggle
void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            // Fire the blue box if it's not already active
            if (!isBlueBoxActive) {
                blueBoxX = playerX + 2 * dx;  // Spawn ahead of the player
                blueBoxZ = playerZ + 2 * dz;
                blueBoxDx = dx;
                blueBoxDz = dz;
                blueBoxHeight = 0.5f;       // Start slightly above the ground
                blueBoxTTL = 100;           // Arbitrary TTL value
                blueBoxVelY = 0.1f;        // Initial vertical velocity for a slight upward notion
                blueBoxSinePos = 0.0f;
                amplitude = 1.0f; // Set an initial amplitude
                isBlueBoxActive = true;
                std::cout << "Blue Box Launched!" << std::endl;
            }
            break;
        case 'w':
        case 'W':
            // Toggle wall passing ability
            canPassThroughWalls = !canPassThroughWalls;
            break;
        case 'O':
        case 'o':
            isWireframe = !isWireframe;
            if (isWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
    }
}


int main(int argc, char** argv) {
    // Initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Maze Game");

    init();

    // Callback functions
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboardFunc);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
