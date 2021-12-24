#include "Angel-yjc.h"
#include "Globals.h"
#include "Object.h"
#include <iostream>
#include <string>
#include <vector>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class Floor : public Object {
public:
    GLuint buffer;
    point4 vertices[4] = {
        point4(5.0f, 0.0f,  8.0f, 1.0f), // a
        point4(-5.0f, 0.0f, 8.0f, 1.0f), // b
        point4(-5.0f, 0.0f, -4.0f, 1.0f), // c
        point4(5.0f, 0.0f, -4.0f, 1.0f), // d
    };

    vec2 quad_texCoord[6] = {
      vec2(1.0, 0.0),  // for a
      vec2(0.0, 0.0),  // for b
      vec2(0.0, 1.0),  // for c

      vec2(0.0, 1.0),  // for c
      vec2(1.0, 1.0),  // for d
      vec2(1.0, 0.0),  // for a 
    };

    static const int num_vertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
    point4 points[num_vertices]; // positions for all vertices
    color4 colors[num_vertices]; // colors for all vertices
    vec3 normals[num_vertices];

    color4 ambient = color4(0.2, 0.2, 0.2, 1.0);
    color4 diffuse = color4(0.0, 1.0, 0.0, 1.0);
    color4 specular = color4(0.0, 0.0, 0.0, 1.0);

    void setup() {
            vec4 u = vertices[1] - vertices[0];
            vec4 v = vertices[3] - vertices[0];
            vec3 normal = normalize(cross(v, u));

            colors[0] = vertex_colors[2]; points[0] = vertices[0]; normals[0] = normal;
            colors[1] = vertex_colors[2]; points[1] = vertices[1]; normals[1] = normal;
            colors[2] = vertex_colors[2]; points[2] = vertices[2]; normals[2] = normal;

            colors[3] = vertex_colors[2]; points[3] = vertices[2]; normals[3] = normal;
            colors[4] = vertex_colors[2]; points[4] = vertices[3]; normals[4] = normal;
            colors[5] = vertex_colors[2]; points[5] = vertices[0]; normals[5] = normal;
    }

    void init() {
        // Create and initialize a vertex buffer object for floor, to be used in display()
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(quad_texCoord) + 
            sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(quad_texCoord), quad_texCoord);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(quad_texCoord), 
            sizeof(colors), colors);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(quad_texCoord)
            + sizeof(colors), sizeof(normals), normals);
    }

    void draw() {
        mat4  ModelView;
        mat4  rolling_transform;
        mat3  normal_matrix;
        mat4  mv = LookAt(eye, at, VUP);
        glUseProgram(program);
        model_view = glGetUniformLocation(program, "model_view");
        ModelView = mv;
        model_view = glGetUniformLocation(program, "model_view");
        glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

        fog.Uniform_Setup_Vars(program, mv, eye, fogFlag);
        direction_light.SetUp_Uniform_Vars(program, ambient, diffuse, specular);
        position_light.SetUp_Uniform_Vars(program, mv, 
            ambient, diffuse, specular,
            spotLightFlag, pointSourceFlag);
        //std::cout<< "textureflagfloor: " << floorTextureFlag << std::endl;
        // Pass on the value of texture_app_flag to the fragment shader
        glUniform1i(glGetUniformLocation(program, "FloorTextureFlag"),
            floorTextureFlag);
        glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(ModelView, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
            1, GL_TRUE, normal_matrix);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        #if 1 // Draw floor
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        GLuint vPosition = glGetAttribLocation(program, "vPosition");
        glEnableVertexAttribArray(vPosition);
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(0));

        GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
        glEnableVertexAttribArray(vTexCoord);
        glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point4) * num_vertices));

        GLuint vNormal = glGetAttribLocation(program, "vNormal");
        glEnableVertexAttribArray(vNormal);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point4) * num_vertices + +sizeof(vec2) * num_vertices
                + sizeof(color4) * num_vertices));

        GLuint vColor = glGetAttribLocation(program, "vColor");
        glEnableVertexAttribArray(vColor);
        glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(vec2) * num_vertices + sizeof(point4) * num_vertices));

        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

        glDisableVertexAttribArray(vPosition);
        glDisableVertexAttribArray(vColor);
        glDisableVertexAttribArray(vTexCoord);
        glDisableVertexAttribArray(vNormal);
        #endif
        // reset texture flag
        glUniform1i(glGetUniformLocation(program, "FloorTextureFlag"),
            0);
    }
};