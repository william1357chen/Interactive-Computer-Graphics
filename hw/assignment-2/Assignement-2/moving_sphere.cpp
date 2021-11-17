/************************************************************
* Assignment 2: Implementation of a simple rolling sphere animation modeled by a 
  wireframe representation with some UI capabilities

**************************************************************/
#include "Angel-yjc.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>


/* 
    GLOBALS 
*/
typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint sphere_buffer;   /* vertex buffer object id for sphere */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint axes_buffer; /* vertex buffer object id for the axes*/

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 15.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 VRP(7.0f, 3.0f, -10.0f, 1.0f); // initial viewer position
vec4 eye = VRP;               // current viewer position

// Sphere rotation variables
vec4 A (3.0, 1.0, 5.0, 1.0);
vec4 B (-2.0, 1.0, -2.5, 1.0);
vec4 C (2.0, 1.0, -4.0, 1.0);

enum Rolling {PartA, PartB, PartC};
Rolling currPart = PartA;

// rotation matrix M
mat4 M = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by right mouse click
int startFlag = 0; // 1: started rolling; 0: stand still at point A. Toggled by key 'b' or 'B'

std::vector<point3> sphere_vertices;
std::vector<color3> sphere_colors;
int sphere_NumVertices = 0; // redefined in readFile() function


const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

const int axes_NumVertices = 6;
point3 axes_points[axes_NumVertices]; // positions for all vertices
color3 axes_colors[axes_NumVertices]; // colors for all vertices

// In worldframe 
point3 floor_vertices[4] = {
    point3(5.0, 0.0,  8.0),
    point3(-5.0, 0.0, 8.0),
    point3(-5.0, 0.0, -4.0),
    point3(5.0, 0.0,  -4.0),

};

// In worldframe 
point3 axes_vertices[4] = {
    point3(0.0, 0.0, 2.0),
    point3(8.0, 0.0, 2.0),
    point3(0.0, 8.0, 2.0),
    point3(0.0, 0.0, 10.0),
};

// RGBA colors
color3 vertex_colors[5] = {
    color3( 1.0, 0.0, 0.0),  // red: for x axis
    color3( 1.0, 0.84, 0.0),  // yellow: for sphere
    color3( 0.0, 1.0, 0.0),  // green: for floor
    color3( 0.0, 0.0, 1.0),  // blue: for z axis
    color3( 1.0, 0.0, 1.0),  // magenta: for y axis
};

/*
    FUNCTIONS
*/

/*
    readFile(): Part A of assignment 2. Read the file name provided by the user and 
    setup an array of vertices for the sphere, stored in sphere_points, and update
    sphere_NumVertices.
*/
void readFile() {
    std::cout << "Please enter the input file name (sphere.8 or sphere.128): ";
    std::string filename;
    std::cin >> filename;
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "There is a problem opening this file\n";
        exit(1);
    }
    //std::cout << "Valid " << std::endl;
    int numOfTriangles;
    int numOfVertices;
    float x, y, z;
    ifs >> numOfTriangles;
    //std::cout << "Num of Triangle " << numOfTriangles << std::endl;
    for (size_t i = 0; i < numOfTriangles; i++) {
        ifs >> numOfVertices;
        //std::cout << "Num of Vertices " << numOfVertices << std::endl;
        for (size_t j = 0; j < numOfVertices; j++) {
            ifs >> x >> y >> z;
            //std::cout << "x, y, z = " << x << y << z << std::endl;
            sphere_vertices.push_back(point3(x, y, z));
            color3 yellow = vertex_colors[1];
            sphere_colors.push_back(yellow);
        }
    }
    sphere_NumVertices = sphere_vertices.size();
    std::cout << "Number of Vertices " << sphere_NumVertices  << std::endl;
    std::cout << "Read File and Built Vertices Array" << std::endl;
    ifs.close();
}

//-------------------------------
// generate 2 triangles: 6 vertices and 1 color
void floor()
{
    floor_colors[0] = vertex_colors[2]; floor_points[0] = floor_vertices[0];
    floor_colors[1] = vertex_colors[2]; floor_points[1] = floor_vertices[1];
    floor_colors[2] = vertex_colors[2]; floor_points[2] = floor_vertices[2];

    floor_colors[3] = vertex_colors[2]; floor_points[3] = floor_vertices[2];
    floor_colors[4] = vertex_colors[2]; floor_points[4] = floor_vertices[3];
    floor_colors[5] = vertex_colors[2]; floor_points[5] = floor_vertices[0];
}
//-------------------------------
// generate 3 lines: 4 vertices and 3 color
void axes() {
    // x axis
    axes_colors[0] = vertex_colors[0]; axes_points[0] = axes_vertices[0];
    axes_colors[1] = vertex_colors[0]; axes_points[1] = axes_vertices[1];

    axes_colors[2] = vertex_colors[4]; axes_points[2] = axes_vertices[0];
    axes_colors[3] = vertex_colors[4]; axes_points[3] = axes_vertices[2];

    axes_colors[4] = vertex_colors[3]; axes_points[4] = axes_vertices[0];
    axes_colors[5] = vertex_colors[3]; axes_points[5] = axes_vertices[3];
}

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{

 // Create and initialize a vertex buffer object for sphere, to be used in display()
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);

    glBufferData(GL_ARRAY_BUFFER, 
                    sizeof(point3)* sphere_NumVertices + sizeof(color3)* sphere_NumVertices,
		            NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    sizeof(point3) * sphere_NumVertices, &sphere_vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 
                    sizeof(point3) * sphere_NumVertices,
                    sizeof(color3) * sphere_NumVertices,
                    &sphere_colors[0]);

    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);

    axes();
    // Create and initialize a vertex buffer object for the axes, to be used in display()
    glGenBuffers(1, &axes_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, axes_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_points) + sizeof(axes_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axes_points), axes_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(axes_points), sizeof(axes_colors),
        axes_colors);

 // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader42.glsl", "fshader42.glsl");
    
    glEnable( GL_DEPTH_TEST );
    glClearColor(0.529, 0.807, 0.92, 0.0);
    glLineWidth(2.0);
}


//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices. Shape of 2 vertices is a line, and
//  Shape of 3 vertices is a triangle
void drawObj(GLuint buffer, int shape, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point3) * num_vertices) ); 
      // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    if (shape == 2)
        glDrawArrays(GL_LINES, 0, num_vertices);
    if (shape == 3)
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}
//----------------------------------------------------------------------------
void display( void )
{
  GLuint  model_view;  // model-view matrix uniform shader variable location
  GLuint  projection;  // projection matrix uniform shader variable location

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );

/*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to VRP and updated by keyboard()

    vec4    VPN(-7.0f, -3.0f, 10.0f, 0.0f);
    vec4    at(0.0f, 0.0f, 0.0f, 1.0f);
    vec4    VUP(0.0f, 1.0f, 0.0f, 0.0f);

    mat4  mv = LookAt(eye, at, VUP); 

    vec3 d;
    vec3 Y0(0.0, 1.0, 0.0);
    vec3 moving_direction;
    vec3 translate;
    vec3 rotate_axis;

    // if b is not pressed it is a stand still
    if (startFlag == 0)
        translate = vec3(A.x, A.y, A.z);
    else {
        // else we calculation rotation and translation for seperate rolling segments
        switch (currPart)
        {
        case PartA:
            moving_direction = vec3(B.x - A.x, B.y - A.y, B.z - A.z);
            d = ((angle * 2.0 * M_PI) / 360.0) * normalize(moving_direction);
            translate = vec3(A.x + d.x, A.y + d.y, A.z + d.z);
            rotate_axis = cross(Y0, moving_direction);
            if (translate.x <= B.x and translate.z <= B.z) {
                currPart = PartB;
                M = Rotate(angle, rotate_axis.x, rotate_axis.y, rotate_axis.z) * M;
                angle = 0.0f;
            }
            break;
        case PartB:
            moving_direction = vec3(C.x - B.x, C.y - B.y, C.z - B.z);
            d = ((angle * 2.0 * M_PI) / 360.0) * normalize(moving_direction);
            translate = vec3(B.x + d.x, B.y + d.y, B.z + d.z);
            rotate_axis = cross(Y0, moving_direction);
            if (translate.x >= C.x and translate.z <= C.z) {
                currPart = PartC;
                M = Rotate(angle, rotate_axis.x, rotate_axis.y, rotate_axis.z) * M;
                angle = 0.0f;
            }

            break;
        case PartC:
            moving_direction = vec3(A.x - C.x, A.y - C.y, A.z - C.z);
            d = ((angle * 2.0 * M_PI) / 360.0) * normalize(moving_direction);
            translate = vec3(C.x + d.x, C.y + d.y, C.z + d.z);
            rotate_axis = cross(Y0, moving_direction);
            if (translate.x >= A.x and translate.z >= A.z) {
                currPart = PartA;
                M = Rotate(angle, rotate_axis.x, rotate_axis.y, rotate_axis.z) * M;
                angle = 0.0f;
            }
            break;
        default:
            break;
        }
    }
    // transform sphere
    if (startFlag == 1)
        mv = mv * Translate(translate.x, translate.y, translate.z) * Rotate(angle, rotate_axis.x, rotate_axis.y, rotate_axis.z) * M;
    else
        mv = mv * Translate(translate.x, translate.y, translate.z);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(sphere_buffer, 3, sphere_NumVertices);  // draw the sphere

/*----- Set up the Mode-View matrix for the floor -----*/

    // floor already in world frame so no need to do instance transformation
    mv = LookAt(eye, at, VUP);

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    drawObj(floor_buffer, 3, floor_NumVertices);  // draw the floor

    /*----- Set up the Mode-View matrix for the axes line -----*/
    // axes already in world frame so no need to do instance transformation
    mv = LookAt(eye, at, VUP);

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawObj(axes_buffer, 2, axes_NumVertices);  // draw the axes

    glutSwapBuffers();
}
//---------------------------------------------------------------------------
// Handles the animation by incrementing angle
void idle (void)
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
    switch(key) {
    case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

    case 'b': case 'B': 
        startFlag = 1; animationFlag = 1; break;
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
// Handling the menu for part e
void menu(int id) {
    switch (id)
    {
    case 0:
        // default view, so restart everything
        animationFlag = 0;
        eye = VRP;
        break;
    case 1:
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main( int argc, char **argv )
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
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
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

    int menu_ID;
    menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Default View Point ", 0);
    glutAddMenuEntry(" Quit ", 1);
    glutAttachMenu(GLUT_LEFT_BUTTON);

    init();
    glutMainLoop();
    return 0;
}
