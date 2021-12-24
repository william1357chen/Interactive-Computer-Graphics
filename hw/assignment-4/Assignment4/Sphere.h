#include "Angel-yjc.h"
#include "Globals.h"
#include "Object.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#ifndef SPHERE_H
#define SPHERE_H

class Sphere : public Object{
public:
    GLuint buffer;
	std::vector<point4> vertices;
	std::vector<vec3> flat_normals;
	std::vector<vec3> smooth_normals;
	std::vector<color4> colors;
	int num_vertices = 0; // redefined in readFile() function
	int num_smooth_normals = 0;
	int num_flat_normals = 0;

	color4 ambient = color4(0.2f, 0.2f, 0.2f, 1.0f);
	color4 diffuse = color4(1.0f, 0.84f, 0.0f, 1.0f);
	color4 specular = color4(1.0f, 0.84f, 0.0f, 1.0f);

    // Sphere rotation variables
    vec4 A = vec4(3.0, 1.0, 5.0, 1.0);
    vec4 B = vec4(-2.0, 1.0, -2.5, 1.0);
    vec4 C = vec4(2.0, 1.0, -4.0, 1.0);

    mat4 M = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    enum Rolling { PartA, PartB, PartC };
    Rolling currPart = PartA;

	
	void setup(const std::string& filename) {
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
                vertices.push_back(point4(x[j], y[j], z[j], 1.0f));
                color4 yellow = vertex_colors[1];
                colors.push_back(yellow);

                smooth_normals.push_back(normalize(vec3(x[j], y[j], z[j])));

     /*           color4 black = vertex_colors[5];
                shadow_vertices.push_back(point4(x[j], y[j], z[j], 1.0f));
                shadow_colors.push_back(black);*/

            }
            // Calculate normals for each vertex
            vec4 vec1(x[0], y[0], z[0], 0.0);
            vec4 vec2(x[1], y[1], z[1], 0.0);
            vec4 vec3(x[2], y[2], z[2], 0.0);
            vec4 u = vec3 - vec1;
            vec4 v = vec2 - vec1;
            normal = normalize(cross(u, v));
            //std::cout << normal << std::endl;
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);
        }
        num_vertices = vertices.size();
        //shadow_NumVertices = shadow_vertices.size();
        num_flat_normals = flat_normals.size();
        num_smooth_normals = smooth_normals.size();

        // std::cout << "Number of Vertices " << sphere_NumVertices << std::endl;
        std::cout << "Read File and Built Vertices Array" << std::endl;
        std::cout << "Size of vector" << num_vertices << std::endl;
        ifs.close();
	}

    void init() {
        // Create and initialize a vertex buffer object for sphere, to be used in display()
        // store position, color, flat normal, and smooth normal
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glBufferData(GL_ARRAY_BUFFER,
            sizeof(point4) * num_vertices
            + sizeof(color4) * num_vertices
            + sizeof(vec3) * num_flat_normals
            + sizeof(vec3) * num_smooth_normals,
            NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
            sizeof(point4) * num_vertices, &vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
            sizeof(point4) * num_vertices,
            sizeof(color4) * num_vertices,
            &colors[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * num_vertices
            + sizeof(color4) * num_vertices,
            sizeof(vec3) * num_flat_normals, &flat_normals[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * num_vertices
            + sizeof(color4) * num_vertices + sizeof(vec3) * num_flat_normals,
            sizeof(vec3) * num_smooth_normals, &smooth_normals[0]);
    }

    void drawSphere(mat4 ModelView) {
        mat4  mv = LookAt(eye, at, VUP);
        mat3  normal_matrix;
        // Draw sphere. If it is wire frame do not use shading and lighting

        if (solidOrWireFlag == 1) {
            glUseProgram(program);
            model_view = glGetUniformLocation(program, "model_view");

            glUniform1i(glGetUniformLocation(program, "VerOrSlantFlag"), verOrSlantFlag);
            glUniform1i(glGetUniformLocation(program, "ObjectOrEyeFlag"), objectOrEyeFlag);
            glUniform1i(glGetUniformLocation(program, "SphereTextureFlag"), sphereTextureFlag);
            glUniform1i(glGetUniformLocation(program, "SphereTextureFlag"), sphereTextureFlag);
            glUniform1i(glGetUniformLocation(program, "UprightOrTiltFlag"), uprightOrTiltFlag);
            glUniform1i(glGetUniformLocation(program, "LatticeFlag"), latticeFlag);
            glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
            glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);

            fog.Uniform_Setup_Vars(program, mv, eye, fogFlag);
            direction_light.SetUp_Uniform_Vars(program, ambient, diffuse, specular);
            position_light.SetUp_Uniform_Vars(program, mv, ambient, 
                diffuse, specular, spotLightFlag, pointSourceFlag);

            glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

            // Set up the Normal Matrix from the model-view matrix
            normal_matrix = NormalMatrix(ModelView, 1);
            glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
                1, GL_TRUE, normal_matrix);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            drawObj(program, buffer, 3, num_vertices);  // draw the sphere

            glUniform1i(glGetUniformLocation(program, "SphereTextureFlag"), 0);
            glUniform1i(glGetUniformLocation(program, "LatticeFlag"), 0);
        }
        else if (solidOrWireFlag == 0) {
            glUseProgram(nonshading_program);
            model_view = glGetUniformLocation(nonshading_program, "model_view");
            fog.Uniform_Setup_Vars(nonshading_program, mv, eye, fogFlag);

            glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            drawObj(nonshading_program, buffer, 3, num_vertices);  // draw the sphere
        }
    }
    mat4 computeRolling() {
        mat4  rolling_transform;
        mat3  normal_matrix;
        vec3 d;
        vec3 Y0(0.0, 1.0, 0.0);
        vec3 moving_direction;
        vec3 translate;
        vec3 rotate_axis;
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
        }
        else {
            rolling_transform = Translate(translate.x, translate.y, translate.z);
        }
        return rolling_transform;
    }
};
#endif // !SPHERE_H