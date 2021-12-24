#include "Angel-yjc.h"
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#ifndef DIRECTION_LIGHT
#define DIRECTION_LIGHT
class DirectionalLight{
public:

    vec4 direction;

    color4 ambient;
    color4 diffuse;
    color4 specular;

    color4 global_ambient;

    float shininess = 125.0;

    DirectionalLight() : ambient(0.0, 0.0, 0.0, 1.0),
        diffuse(0.8, 0.8, 0.8, 1.0),
        specular(0.2, 0.2, 0.2, 1.0),
        global_ambient(1.0, 1.0, 1.0, 1.0),
        direction(0.1, 0.0, -1.0, 0.0) {}


    // Setup the Lighting Uniform Variables for Global Ambient Light and Directional Light, depending on the material attributes (either sphere or floor)
    void SetUp_Uniform_Vars(GLuint program, color4 material_ambient, color4 material_diffuse, color4 material_specular)
    {
        color4 global_ambient_product = global_ambient * material_ambient;
        glUniform4fv(glGetUniformLocation(program, "GlobalAmbientProduct"),
            1, global_ambient_product);

        color4 ambient_product = ambient * material_ambient;
        color4 diffuse_product = diffuse * material_diffuse;
        color4 specular_product = specular * material_specular;

        glUniform4fv(glGetUniformLocation(program, "DirectionAmbientProduct"),
            1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "DirectionDiffuseProduct"),
            1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "DirectionSpecularProduct"),
            1, specular_product);

        glUniform4fv(glGetUniformLocation(program, "LightDirection"),
            1, direction);

        glUniform1f(glGetUniformLocation(program, "Shininess"),
            shininess);
    }

};

#endif