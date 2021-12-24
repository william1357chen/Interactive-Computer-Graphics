/************************************************************
*  Final Project: A rolling sphere with lighting, effects, etc.
**************************************************************/
#include "Angel-yjc.h"
#include "Sphere.h"
#include "Shadow.h"
#include "Textmap.h"
#include "Floor.h"
#include "Axes.h"
#include "Firework.h"
#include "Globals.h"
#include "Menu.h"
#include <iostream>
#include <string>
#include <vector>

/*
    GLOBALS
*/
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

Sphere sphere;
Floor ground;
Axes axes;
Shadow shadow;
Menu menu;
Firework firework;

std::string filename;

static GLuint checkTexName;
static GLuint stripeTexName;

/*
    FUNCTIONS
*/

void readFile() {
    std::cout << "Please enter the input file name (sphere.256 or sphere.1024): ";
    std::cin >> filename;
}

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
    /*--- Create and Initialize a checkerboard object ---*/
    image_set_up();
    glGenTextures(1, &checkTexName);
    glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_2D, checkTexName); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

    /*--- Create and Initialize a stripe object ---*/
    glGenTextures(1, &stripeTexName);
    glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1 
    glBindTexture(GL_TEXTURE_1D, stripeTexName); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth,
        0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);

    sphere.setup(filename);
    sphere.init();

    shadow.setup(sphere);
    shadow.init();

    ground.setup();
    ground.init();

    axes.setup();
    axes.init();

    firework.setup();
    firework.init();

    // Load shaders and create two shader program (to be used in display())
    nonshading_program = InitShader("vshader.glsl", "fshader.glsl");
    shading_program = InitShader("vshader_lighting.glsl", "fshader_lighting.glsl");
    firework_program = InitShader("vshader_firework.glsl", "fshader_firework.glsl");
    // By default use non shading program
    program = nonshading_program;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.807, 0.92, 0.0);
    glLineWidth(2.0);
}


//----------------------------------------------------------------------------
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shading_program); // Use the shader program
    projection = glGetUniformLocation(shading_program, "projection");
    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    glUseProgram(nonshading_program); // Use the shader program
    projection = glGetUniformLocation(nonshading_program, "projection");
    /*---  Set up and pass on Projection matrix to the shader ---*/
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    glUseProgram(firework_program); // Use the shader program
    projection = glGetUniformLocation(firework_program, "projection");
    /*---  Set up and pass on Projection matrix to the shader ---*/
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to VRP and updated by keyboard()


    mat4  mv = LookAt(eye, at, VUP);
    mat4  ModelView;
    mat4  rolling_transform;
    mat3  normal_matrix;

    // transform and draw sphere
    rolling_transform = sphere.computeRolling();
    ModelView = mv * rolling_transform;
    sphere.drawSphere(ModelView);

/*----- Set up the Mode-View matrix for the floor and shadow -----*/

    // if need to draw shadow
    if (shadowFlag == 1) {
        // disable writing to z buffer and draw ground and only draw to frame buffer
        glDepthMask(GL_FALSE);
        // floor already in world frame so no need to do instance transformation
        ground.draw();
        /*----- Set up the Mode-View matrix for the shadow -----*/

        // Draw semi-transparent shadow by enabling blending with the ground
        // Do not enable z buffer yet
        if (shadowBlendFlag == 1) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // over operation
        }
        else {
            // enable writing to z buffer and draw shadow to both buffers
            glDepthMask(GL_TRUE);
        }
        ModelView = mv * shadow.shadow_matrix * rolling_transform;
        shadow.draw(ModelView);

        glDepthMask(GL_TRUE); // enable z buffer for both cases
        glDisable(GL_BLEND); // disable blending after drawing shadow for both cases

        // disable writing to frame buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        ground.draw();
        // enable frame buffer 
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else { // if don't need to draw shadow
        ground.draw();
    }

    /*----- Set up the Mode-View matrix for the axes line -----*/
    // axes already in world frame so no need to do instance transformation
    axes.draw();

    if (fireworkFlag)
        firework.draw();

    glutSwapBuffers();
}
//---------------------------------------------------------------------------
// Handles the animation by incrementing angle
void idle(void)
{
    // angle += 0.03f;
    if (startFlag == 1 and animationFlag == 1)
        angle += 0.5f;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
// Handling the keybaord functions for part e
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'X': eye[0] += 1.0; break;
    case 'x': eye[0] -= 1.0; break;
    case 'Y': eye[1] += 1.0; break;
    case 'y': eye[1] -= 1.0; break;
    case 'Z': eye[2] += 1.0; break;
    case 'z': eye[2] -= 1.0; break;
    case 'v': case 'V': 
        verOrSlantFlag = 0;
        break;
    case 's': case 'S':
        if (sphereTextureFlag == 1 || sphereTextureFlag == 2) {
            verOrSlantFlag = 1;
        }
        break;
    case 'o': case 'O':
        objectOrEyeFlag = 0;
        break;
    case 'e': case 'E':
        if (sphereTextureFlag == 1 || sphereTextureFlag == 2) {
            objectOrEyeFlag = 1;
        }
        break;
    case 'u': case 'U':
        uprightOrTiltFlag = 0;
        break;
    case 't': case 'T':
        if (latticeFlag == 1) {
            uprightOrTiltFlag = 1;
        }
        break;
    case 'l': case 'L':
        latticeFlag = 1 - latticeFlag;
        break;

    case 'b': case 'B':
        startFlag = 1; animationFlag = 1; break;
    }
    // Make sure the shadow is not drawn if we are under the floor
    if (eye[1] < 0.0 && tempShadowFlag == 0) {
        tempShadowFlag = shadowFlag;
        shadowFlag = 0;
    } 
    else if (eye[1] > 0.0 && tempShadowFlag == 1) {
        // Make sure to restore shadow if we were under the floor
        shadowFlag = tempShadowFlag;
        tempShadowFlag = 0;
    }

    glutPostRedisplay();
}
//----------------------------------------------------------------------------
// Handling the mouse functions for part e
void mouse(int button, int state, int x, int y) {
    switch (state) {
    case GLUT_DOWN:
        switch (button)
        {
        case GLUT_LEFT_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            animationFlag = 1 - animationFlag;
            break;
        default:
            break;
        }
        break;
    case GLUT_UP:
        break;
    }
    glutPostRedisplay();

}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat)width / (GLfloat)height;
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    readFile();

    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    glutCreateWindow("Rolling Sphere");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
        exit(1);
    }
#endif
    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
    std::cout << "Setup Successful" << std::endl;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    menu.setupMenu();
    init();
    glutMainLoop();
    return 0;
}
