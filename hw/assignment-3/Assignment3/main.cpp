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

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint shading_program;
GLuint nonshading_program;

GLuint sphere_buffer;   /* vertex buffer object id for sphere */
GLuint shadow_buffer;
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
vec4 A(3.0, 1.0, 5.0, 1.0);
vec4 B(-2.0, 1.0, -2.5, 1.0);
vec4 C(2.0, 1.0, -4.0, 1.0);

enum Rolling { PartA, PartB, PartC };
Rolling currPart = PartA;

// rotation matrix M
mat4 M = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};


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

std::vector<point4> sphere_vertices;
std::vector<vec3> sphere_flat_normals;
std::vector<vec3> sphere_smooth_normals;
std::vector<color4> sphere_colors;
int sphere_NumVertices = 0; // redefined in readFile() function
int sphere_NumSmoothNormals = 0;
int sphere_NumFlatNormals = 0;

std::vector<point4> shadow_vertices;
std::vector<color4> shadow_colors;
int shadow_NumVertices = 0;

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point4 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floor_normal[floor_NumVertices];

const int axes_NumVertices = 6;
point4 axes_points[axes_NumVertices]; // positions for all vertices
color4 axes_colors[axes_NumVertices]; // colors for all vertices

// In worldframe 
point4 floor_vertices[4] = {
    point4(5.0f, 0.0f,  8.0f, 1.0f),
    point4(-5.0f, 0.0f, 8.0f, 1.0f),
    point4(-5.0f, 0.0f, -4.0f, 1.0f),
    point4(5.0f, 0.0f, -4.0f, 1.0f),
};

// In worldframe 
point4 axes_vertices[4] = {
    point4(0.0, 0.0, 2.0, 1.0),
    point4(8.0, 0.0, 2.0, 1.0),
    point4(0.0, 8.0, 2.0, 1.0),
    point4(0.0, 0.0, 10.0, 1.0),
};

// RGBA colors
color4 vertex_colors[6] = {
    color4(1.0f, 0.0f, 0.0f, 1.0f),  // red: for x axis
    color4(1.0f, 0.84f, 0.0f, 1.0f),  // yellow: for sphere
    color4(0.0f, 1.0f, 0.0f, 1.0f),  // green: for floor
    color4(0.0f, 0.0f, 1.0f, 1.0f),  // blue: for z axis
    color4(1.0f, 0.0f, 1.0f, 1.0f),  // magenta: for y axis
    color4(0.25f, 0.25f, 0.25f, 0.65f) // for shadow
};


mat4 shadow_matrix(
    12.0, 0.0, 0.0, 0.0,
    14.0, 0.0, 3.0, -1.0,
     0.0, 0.0, 12.0, 0.0,
     0.0, 0.0, 0.0, 12.0
);

// Lighting and Shading variables

// Global ambient light
color4 global_ambient(1.0, 1.0, 1.0, 1.0);

// Directional light
color4 direction_ambient(0.0, 0.0, 0.0, 1.0);
color4 direction_diffuse(0.8, 0.8, 0.8, 1.0);
color4 direction_specular(0.2, 0.2, 0.2, 1.0);

vec4 directional_light_direction(0.1, 0.0, -1.0, 0.0); // in eye frame

// Positional light
color4 position_ambient(0.0f, 0.0f, 0.0f, 1.0f);
color4 position_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
color4 position_specular(1.0f, 1.0f, 1.0f, 1.0f);

point4 positionalLightPosition(-14.0, 12.0, -3.0, 1.0); // In world frame
point4 spotLightFocusPosition(-6.0f, 0.0, -4.5, 1.0); // In world frame
float spotLightExponent = 15.0;
float spotLightCutOffAngle = (20 * M_PI) / 180;

float material_shininess = 125.0;

float const_att = 2.0f;
float linear_att = 0.01f;
float quad_att = 0.001f;

// Material color
color4 floor_ambient(0.2, 0.2, 0.2, 1.0);
color4 floor_diffuse(0.0, 1.0, 0.0, 1.0);
color4 floor_specular(0.0, 0.0, 0.0, 1.0);

color4 sphere_ambient(0.2, 0.2, 0.2, 1.0);
color4 sphere_diffuse(1.0, 0.84, 0.0, 1.0);
color4 sphere_specular(1.0, 0.84, 0.0, 1.0);


/*
    FUNCTIONS
*/

/*
    readFile(): Read the file name provided by the user and
    setup an array of vertices for the sphere and shadow, as well as calculating
    the normals for flat shading and also smooth shading.
*/
void readFile() {
    std::cout << "Please enter the input file name (sphere.256 or sphere.1024): ";
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
    float x[3], y[3], z[3];
    ifs >> numOfTriangles;
    vec3 normal;
  
    for (size_t i = 0; i < numOfTriangles; i++) {
        ifs >> numOfVertices;
        for (size_t j = 0; j < numOfVertices; j++) {
            ifs >> x[j] >> y[j] >> z[j];
            sphere_vertices.push_back(point4(x[j], y[j], z[j], 1.0f));
            color4 yellow = vertex_colors[1];
            sphere_colors.push_back(yellow);

            sphere_smooth_normals.push_back(normalize(vec3(x[j], y[j], z[j])));

            color4 black = vertex_colors[5];
            shadow_vertices.push_back(point4(x[j], y[j], z[j], 1.0f));
            shadow_colors.push_back(black);

        }
        // Calculate normals for each vertex
        vec4 vec1(x[0], y[0], z[0], 0.0);
        vec4 vec2(x[1], y[1], z[1], 0.0);
        vec4 vec3(x[2], y[2], z[2], 0.0);
        vec4 u = vec3 - vec1;
        vec4 v = vec2 - vec1;
        normal = normalize(cross(u, v));
        //std::cout << normal << std::endl;
        sphere_flat_normals.push_back(normal);
        sphere_flat_normals.push_back(normal);
        sphere_flat_normals.push_back(normal);
    }
    sphere_NumVertices = sphere_vertices.size();
    shadow_NumVertices = shadow_vertices.size();
    sphere_NumFlatNormals = sphere_flat_normals.size();
    sphere_NumSmoothNormals = sphere_smooth_normals.size();

    // std::cout << "Number of Vertices " << sphere_NumVertices << std::endl;
    std::cout << "Read File and Built Vertices Array" << std::endl;
    std::cout << "Size of vector" << sphere_NumVertices << std::endl;
    ifs.close();
}

//-------------------------------
// generate 2 triangles: 6 vertices and 1 color as well as their normals
void floor()
{
    vec4 u = floor_vertices[1] - floor_vertices[0];
    vec4 v = floor_vertices[3] - floor_vertices[0];
    vec3 normal = normalize(cross(v, u));

    floor_colors[0] = vertex_colors[2]; floor_points[0] = floor_vertices[0]; floor_normal[0] = normal;
    floor_colors[1] = vertex_colors[2]; floor_points[1] = floor_vertices[1]; floor_normal[1] = normal;
    floor_colors[2] = vertex_colors[2]; floor_points[2] = floor_vertices[2]; floor_normal[2] = normal;

    floor_colors[3] = vertex_colors[2]; floor_points[3] = floor_vertices[2]; floor_normal[3] = normal;
    floor_colors[4] = vertex_colors[2]; floor_points[4] = floor_vertices[3]; floor_normal[4] = normal;
    floor_colors[5] = vertex_colors[2]; floor_points[5] = floor_vertices[0]; floor_normal[5] = normal;
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
    // store position, color, flat normal, and smooth normal
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(point4) * sphere_NumVertices
        + sizeof(color4) * sphere_NumVertices
        + sizeof(vec3) * sphere_NumFlatNormals
        +sizeof(vec3) * sphere_NumSmoothNormals,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(point4) * sphere_NumVertices, &sphere_vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point4) * sphere_NumVertices,
        sizeof(color4) * sphere_NumVertices,
        &sphere_colors[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * sphere_NumVertices
        + sizeof(color4) * sphere_NumVertices,
        sizeof(vec3) * sphere_NumFlatNormals, &sphere_flat_normals[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * sphere_NumVertices
        + sizeof(color4) * sphere_NumVertices + sizeof(vec3) * sphere_NumFlatNormals,
        sizeof(vec3) * sphere_NumSmoothNormals, &sphere_smooth_normals[0]);


    // Create vertex buffer object for shadow
    glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(point4) * shadow_NumVertices + sizeof(color4) * shadow_NumVertices,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(point4) * shadow_NumVertices, &shadow_vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point4) * shadow_NumVertices,
        sizeof(color4) * shadow_NumVertices,
        &shadow_colors[0]);


    floor();
    // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normal),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
        floor_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors), sizeof(floor_normal),
        floor_normal);

    axes();
    // Create and initialize a vertex buffer object for the axes, to be used in display()
    glGenBuffers(1, &axes_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, axes_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_points) + sizeof(axes_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axes_points), axes_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(axes_points), sizeof(axes_colors),
        axes_colors);

    // Load shaders and create two shader program (to be used in display())
    nonshading_program = InitShader("vshader42.glsl", "fshader42.glsl");
    shading_program = InitShader("vshader53.glsl", "fshader53.glsl");
    // By default use non shading program
    program = nonshading_program;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.807, 0.92, 0.0);
    glLineWidth(2.0);
}
// Setup the Lighting Uniform Variables for Global Ambient Light and Directional Light, depending on the material attributes (either sphere or floor)
void SetUp_DirectionalLighting_Uniform_Vars(color4 material_ambient, color4 material_diffuse, color4 material_specular)
{
    color4 global_ambient_product = global_ambient * material_ambient;
    glUniform4fv(glGetUniformLocation(shading_program, "GlobalAmbientProduct"),
        1, global_ambient_product);

    color4 ambient_product = direction_ambient * material_ambient;
    color4 diffuse_product = direction_diffuse * material_diffuse;
    color4 specular_product = direction_specular * material_specular;

    glUniform4fv(glGetUniformLocation(shading_program, "DirectionAmbientProduct"),
        1, ambient_product);
    glUniform4fv(glGetUniformLocation(shading_program, "DirectionDiffuseProduct"),
        1, diffuse_product);
    glUniform4fv(glGetUniformLocation(shading_program, "DirectionSpecularProduct"),
        1, specular_product);

    glUniform4fv(glGetUniformLocation(shading_program, "LightDirection"),
        1, directional_light_direction);

    glUniform1f(glGetUniformLocation(shading_program, "Shininess"),
        material_shininess);
}
// Setup the Lighting Uniform Variables for Positional Light (point source or spotlight), depending on the material attributes (either sphere or floor)
void SetUp_PositionalLighting_Uniform_Vars(mat4 mv, color4 material_ambient, color4 material_diffuse, color4 material_specular)
{

    color4 ambient_product = position_ambient * material_ambient;
    color4 diffuse_product = position_diffuse * material_diffuse;
    color4 specular_product = position_specular * material_specular;

    glUniform4fv(glGetUniformLocation(shading_program, "PositionAmbientProduct"),
        1, ambient_product);
    glUniform4fv(glGetUniformLocation(shading_program, "PositionDiffuseProduct"),
        1, diffuse_product);
    glUniform4fv(glGetUniformLocation(shading_program, "PositionSpecularProduct"),
        1, specular_product);

    vec4 light_position_eyeFrame = mv * positionalLightPosition;
    glUniform4fv(glGetUniformLocation(shading_program, "PositionalLightPosition"),
        1, light_position_eyeFrame);

    glUniform1f(glGetUniformLocation(shading_program, "ConstAtt"),
        const_att);
    glUniform1f(glGetUniformLocation(shading_program, "LinearAtt"),
        linear_att);
    glUniform1f(glGetUniformLocation(shading_program, "QuadAtt"),
        quad_att);

    vec4 focus_position_eyeFrame = mv * spotLightFocusPosition;
    glUniform4fv(glGetUniformLocation(shading_program, "SpotLightFocusPosition"),
        1, focus_position_eyeFrame);

    glUniform1f(glGetUniformLocation(shading_program, "CutOffAngle"),
        spotLightCutOffAngle);

    glUniform1f(glGetUniformLocation(shading_program, "SpotLightExponent"),
        spotLightExponent);

    glUniform1i(glGetUniformLocation(shading_program, "SpotLightFlag"),
        spotLightFlag);

    glUniform1i(glGetUniformLocation(shading_program, "PointSourceFlag"),
        pointSourceFlag);
}


//----------------------------------------------------------------------------
/* drawObj(buffer, num_vertices) :
   draw the object that is associated with the vertex buffer object "buffer"
   and has "num_vertices" vertices. Shape of 2 vertices is a line, and
  Shape of 3 vertices is a triangle. Depending on whether lighting is enabled,
  use nonshading program or shading program

*/
void drawObj(GLuint inputprogram, GLuint buffer, int shape, int num_vertices)
{
    if (inputprogram == nonshading_program) { // If the object we are drawing does not need shading
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        GLuint vPosition = glGetAttribLocation(inputprogram, "vPosition");
        glEnableVertexAttribArray(vPosition);
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(0));

        GLuint vColor = glGetAttribLocation(inputprogram, "vColor");
        glEnableVertexAttribArray(vColor);
        glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point4) * num_vertices));

        if (shape == 2)
            glDrawArrays(GL_LINES, 0, num_vertices);
        if (shape == 3)
            glDrawArrays(GL_TRIANGLES, 0, num_vertices);

        glDisableVertexAttribArray(vPosition);
        glDisableVertexAttribArray(vColor);
    }
    else if (inputprogram == shading_program) { // If object we are drawing needs shading 
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        GLuint vPosition = glGetAttribLocation(inputprogram, "vPosition");
        glEnableVertexAttribArray(vPosition);
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(0));

        GLuint vNormal;
        if (shadingFlag == 1 && num_vertices != floor_NumVertices) { // smooth shading (only for sphere)
            vNormal = glGetAttribLocation(inputprogram, "vNormal");
            glEnableVertexAttribArray(vNormal);
            glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point4) * num_vertices 
                    + sizeof(color4) * num_vertices + sizeof(vec3) * num_vertices));
        }
        else { // flat shading
            vNormal = glGetAttribLocation(inputprogram, "vNormal");
            glEnableVertexAttribArray(vNormal);
            glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point4) * num_vertices + sizeof(color4) * num_vertices));
        }
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

        glDisableVertexAttribArray(vPosition);
        glDisableVertexAttribArray(vNormal);
    }
}
//----------------------------------------------------------------------------
void display(void)
{
    GLuint  model_view;  // model-view matrix uniform shader variable location
    GLuint  projection;  // projection matrix uniform shader variable location

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

/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to VRP and updated by keyboard()

    vec4    VPN(-7.0f, -3.0f, 10.0f, 0.0f);
    vec4    at(0.0f, 0.0f, 0.0f, 1.0f);
    vec4    VUP(0.0f, 1.0f, 0.0f, 0.0f);

    mat4  mv = LookAt(eye, at, VUP);
    mat4  ModelView;
    mat4  rolling_transform;
    mat3  normal_matrix;
    vec3 d;
    vec3 Y0(0.0, 1.0, 0.0);
    vec3 moving_direction;
    vec3 translate;
    vec3 rotate_axis;

    // if b is not pressed it is a stand still
    if (startFlag == 0)
        translate = vec3(A.x, A.y, A.z);
    else { // else we calculation rotation and translation for seperate rolling segments
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
    if (startFlag == 1) {
        rolling_transform = Translate(translate.x, translate.y, translate.z) * Rotate(angle, rotate_axis.x, rotate_axis.y, rotate_axis.z) * M;
        ModelView = mv * rolling_transform;
    }
    else {
        rolling_transform = Translate(translate.x, translate.y, translate.z);
        ModelView = mv * rolling_transform;
    }
    // Draw sphere. If it is wire frame do not use shading and lighting
    if (solidOrWireFlag == 1) {
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view");

        SetUp_DirectionalLighting_Uniform_Vars(sphere_ambient, sphere_diffuse, sphere_specular);
        SetUp_PositionalLighting_Uniform_Vars(mv, sphere_ambient, sphere_diffuse, sphere_specular);

        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(ModelView, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
            1, GL_TRUE, normal_matrix);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        drawObj(program, sphere_buffer, 3, sphere_NumVertices);  // draw the sphere
    }
    else if (solidOrWireFlag == 0) {
        glUseProgram(nonshading_program);
        model_view = glGetUniformLocation(nonshading_program, "model_view");

        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        drawObj(nonshading_program, sphere_buffer, 3, sphere_NumVertices);  // draw the sphere
    }


// Part (C)
/*----- Set up the Mode-View matrix for the floor and shadow -----*/

    // if need to draw shadow
    if (shadowFlag == 1) {
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view");
        // disable writing to z buffer and draw ground and only draw to frame buffer
        glDepthMask(GL_FALSE);
        // floor already in world frame so no need to do instance transformation
        ModelView = mv;
        model_view = glGetUniformLocation(program, "model_view");
        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        SetUp_DirectionalLighting_Uniform_Vars(floor_ambient, floor_diffuse, floor_specular);
        SetUp_PositionalLighting_Uniform_Vars(mv, floor_ambient, floor_diffuse, floor_specular);

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(ModelView, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
            1, GL_TRUE, normal_matrix);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawObj(program, floor_buffer, 3, floor_NumVertices);  // draw the floor

        /*----- Set up the Mode-View matrix for the shadow -----*/
        // enable writing to z buffer and draw shadow to both buffers
        glUseProgram(nonshading_program); // Use the nonshading program
        model_view = glGetUniformLocation(nonshading_program, "model_view");

        glDepthMask(GL_TRUE);
        ModelView = mv * shadow_matrix * rolling_transform;
        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major
        if (solidOrWireFlag == 0) // wire frame 
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if (solidOrWireFlag == 1) // solid sphere
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawObj(nonshading_program, shadow_buffer, 3, shadow_NumVertices);  // draw the shadow

        glUseProgram(program); // Use the shader program
        model_view = glGetUniformLocation(program, "model_view");

        // disable writing to frame buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        // draw ground to z buffer only
        ModelView = mv;
        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(ModelView, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
            1, GL_TRUE, normal_matrix);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawObj(program, floor_buffer, 3, floor_NumVertices);  // draw the floor
        // enable frame buffer 
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else { // if don't need to draw shadow
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view");
        ModelView = mv;
        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        SetUp_DirectionalLighting_Uniform_Vars(floor_ambient, floor_diffuse, floor_specular);
        SetUp_PositionalLighting_Uniform_Vars(mv, floor_ambient, floor_diffuse, floor_specular);

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(ModelView, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
            1, GL_TRUE, normal_matrix);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawObj(program, floor_buffer, 3, floor_NumVertices);  // draw the floor
        
    }


    /*----- Set up the Mode-View matrix for the axes line -----*/
    // axes already in world frame so no need to do instance transformation
    glUseProgram(nonshading_program); // Use the nonshading program
    model_view = glGetUniformLocation(nonshading_program, "model_view");

    ModelView = mv;

    glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawObj(nonshading_program, axes_buffer, 2, axes_NumVertices);  // draw the axes

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
// Handling the menus

void main_menu(int id) {
    switch (id)
    {
    case 0:
        // default view, so restart everything
        animationFlag = 0;
        eye = VRP;
        // Make sure to restore shadow if we were under the floor
        if (tempShadowFlag == 1) {
            shadowFlag = tempShadowFlag;
            tempShadowFlag = 0;
        }
        break;
    case 1:
        exit(EXIT_SUCCESS);
        break;
    case 2:
        solidOrWireFlag = 0;
    default:
        break;
    }
    glutPostRedisplay();
}

void shadow_menu(int id){
    shadowFlag = id;
    glutPostRedisplay();
}

void light_menu(int id){
    switch (id)
    {
    case 0:
        lightFlag = 0;
        spotLightFlag = 0;
        pointSourceFlag = 0;
        program = nonshading_program;
        break;
    case 1:
        lightFlag = 1;
        program = shading_program;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void shading_menu(int id) {
    shadingFlag = id;
    solidOrWireFlag = 1;
    glutPostRedisplay();
}

void position_light_menu(int id) {
    if (lightFlag == 0) return;
    switch (id)
    {
    case 0:
        spotLightFlag = 1;
        pointSourceFlag = 0;
        break;
    case 1:
        spotLightFlag = 0;
        pointSourceFlag = 1;
        break;
    default:
        break;
    }
}

void setupMenu() {
    int smenu1 = glutCreateMenu(shadow_menu);
    glutSetMenuFont(smenu1, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", 0);
    glutAddMenuEntry(" Yes ", 1);

    int lmenu = glutCreateMenu(light_menu);
    glutSetMenuFont(lmenu, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", 0);
    glutAddMenuEntry(" Yes ", 1);

    int smenu2 = glutCreateMenu(shading_menu);
    glutSetMenuFont(smenu2, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Flat Shading ", 0);
    glutAddMenuEntry(" Smooth Shading ", 1);

    int poslightmenu = glutCreateMenu(position_light_menu);
    glutSetMenuFont(smenu2, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Spot Light ", 0);
    glutAddMenuEntry(" Point Source ", 1);

    int menu_ID;
    menu_ID = glutCreateMenu(main_menu);
    glutSetMenuFont(menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Default View Point ", 0);
    glutAddMenuEntry(" Quit ", 1);
    glutAddSubMenu(" Shadow ", smenu1);
    glutAddSubMenu(" Enable Lighting ", lmenu);
    glutAddMenuEntry(" Wire Frame Sphere ", 2);
    glutAddSubMenu(" Shading ", smenu2);
    glutAddSubMenu(" Light Source ", poslightmenu);
    glutAttachMenu(GLUT_LEFT_BUTTON);
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

    setupMenu();

    init();
    glutMainLoop();
    return 0;
}
