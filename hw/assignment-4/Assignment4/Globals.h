#include "Angel-yjc.h"
#include "DirectionalLight.h"
#include "PositionalLight.h"
#include "Fog.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#ifndef GLOBALS
#define GLOBALS

GLuint program;       /* shader program object id */
GLuint shading_program;
GLuint nonshading_program;
GLuint firework_program;

float T = 0;
float Tsub = 0;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection;  // projection matrix uniform shader variable location

DirectionalLight direction_light;
PositionalLight position_light;
Fog fog;

color4 vertex_colors[6] = {
    color4(1.0f, 0.0f, 0.0f, 1.0f),  // red: for x axis
    color4(1.0f, 0.84f, 0.0f, 1.0f),  // yellow: for sphere
    color4(0.0f, 1.0f, 0.0f, 1.0f),  // green: for floor
    color4(0.0f, 0.0f, 1.0f, 1.0f),  // blue: for z axis
    color4(1.0f, 0.0f, 1.0f, 1.0f),  // magenta: for y axis
    color4(0.25f, 0.25f, 0.25f, 0.65f) // for shadow
};

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 15.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 VRP(7.0f, 3.0f, -10.0f, 1.0f); // initial viewer position
vec4 eye = VRP;               // current viewer position
vec4 VPN(-7.0f, -3.0f, 10.0f, 0.0f);
vec4 at(0.0f, 0.0f, 0.0f, 1.0f);
vec4 VUP(0.0f, 1.0f, 0.0f, 0.0f);

// Menu and Input flags
int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by right mouse click
int startFlag = 0; // 1: started rolling; 0: stand still at point A. Toggled by key 'b' or 'B'
int shadowFlag = 0; // 1: show shadow; 0: no shadow. Toggled by menu
int tempShadowFlag = 0;
int solidOrWireFlag = 0; // 1: solid sphere; 0: wire frame sphere. Toggled by menu
int lightFlag = 0; // 1: show lighting; 0: no lighting. Toggled by menu
int shadingFlag = 0; // 1: Smooth shading; 0: Flat shading. Toggled by menu
int spotLightFlag = 0;
int pointSourceFlag = 0;
int shadowBlendFlag = 0; // 0: Opaque shadow 1: Semi transparent shadow

// Texture flag
int floorTextureFlag = 0; // 0: no texture 1: texture
int sphereTextureFlag = 0; // 0: no texture 1: contour line  2: checkerboard
int verOrSlantFlag = 0; // 0: vertical 1: slant
int objectOrEyeFlag = 0; // 0: object 1: eye
int uprightOrTiltFlag = 0; // 0: upright 1: tilted
int latticeFlag = 0; // 0: no lattice effect 1: lattice effect

int fireworkFlag = 0; // 0: no firework 1: firework effect on

int fogFlag = 0; // 0: No Fog, 1: Linear, 2: Exponential 3: Exponential Square

#endif