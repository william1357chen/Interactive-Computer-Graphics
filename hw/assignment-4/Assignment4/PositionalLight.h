#include "Angel-yjc.h"
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#ifndef POSITION_LIGHT
#define POSITION_LIGHT

class PositionalLight{
public:
	point4 lightFocus = point4(-6.0f, 0.0, -4.5, 1.0); // In world frame

    color4 ambient = color4(0.0f, 0.0f, 0.0f, 1.0f);
    color4 diffuse = color4(1.0f, 1.0f, 1.0f, 1.0f);
    color4 specular = color4(1.0f, 1.0f, 1.0f, 1.0f);

    point4 lightPos = color4(-14.0, 12.0, -3.0, 1.0); // In world frame

	float spotLightExponent = 15.0;
	float spotLightCutOffAngle = (20 * M_PI) / 180;

	float shininess = 125.0;
	float const_att = 2.0f;
	float linear_att = 0.01f;
	float quad_att = 0.001f;

    PositionalLight() : lightFocus(-6.0f, 0.0, -4.5, 1.0),
        ambient(0.0f, 0.0f, 0.0f, 1.0f),
        diffuse(1.0f, 1.0f, 1.0f, 1.0f),
        specular(1.0f, 1.0f, 1.0f, 1.0f),
        lightPos(-14.0, 12.0, -3.0, 1.0) {}


    void SetUp_Uniform_Vars(GLuint program, mat4 mv, 
        color4 material_ambient, color4 material_diffuse, color4 material_specular,
        int spotLightFlag, int pointSourceFlag)
    {

        color4 ambient_product = ambient * material_ambient;
        color4 diffuse_product = diffuse * material_diffuse;
        color4 specular_product = specular * material_specular;

        glUniform4fv(glGetUniformLocation(program, "PositionAmbientProduct"),
            1, ambient_product);
        glUniform4fv(glGetUniformLocation(program, "PositionDiffuseProduct"),
            1, diffuse_product);
        glUniform4fv(glGetUniformLocation(program, "PositionSpecularProduct"),
            1, specular_product);

        vec4 light_position_eyeFrame = mv * lightPos;
        glUniform4fv(glGetUniformLocation(program, "PositionalLightPosition"),
            1, light_position_eyeFrame);

        glUniform1f(glGetUniformLocation(program, "ConstAtt"),
            const_att);
        glUniform1f(glGetUniformLocation(program, "LinearAtt"),
            linear_att);
        glUniform1f(glGetUniformLocation(program, "QuadAtt"),
            quad_att);

        vec4 focus_position_eyeFrame = mv * lightFocus;
        glUniform4fv(glGetUniformLocation(program, "SpotLightFocusPosition"),
            1, focus_position_eyeFrame);

        glUniform1f(glGetUniformLocation(program, "CutOffAngle"),
            spotLightCutOffAngle);

        glUniform1f(glGetUniformLocation(program, "SpotLightExponent"),
            spotLightExponent);

        glUniform1i(glGetUniformLocation(program, "SpotLightFlag"),
            spotLightFlag);

        glUniform1i(glGetUniformLocation(program, "PointSourceFlag"),
            pointSourceFlag);

        glUniform1f(glGetUniformLocation(program, "Shininess"),
            shininess);
    }
};

#endif