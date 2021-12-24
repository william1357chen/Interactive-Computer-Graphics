#include "Angel-yjc.h"


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#ifndef FOG_H
#define FOG_H

class Fog {
public:
	color4 fog_color = color4(0.7, 0.7, 0.7, 0.5);
	float linear_start = 0.0;
	float linear_end = 18.0;
	float density = 0.09;

	void Uniform_Setup_Vars(GLuint program, mat4 mv, point4 eye, int fogFlag) {
		glUniform1i(glGetUniformLocation(program, "FogFlag"), fogFlag);
		glUniform1f(glGetUniformLocation(program, "FogLinearStart"), linear_start);
		glUniform1f(glGetUniformLocation(program, "FogLinearEnd"), linear_end);
		glUniform1f(glGetUniformLocation(program, "FogDensity"), density);
		glUniform4fv(glGetUniformLocation(program, "FogColor"), 1, fog_color);
		point4 eye_pos = mv * eye;
		glUniform4fv(glGetUniformLocation(program, "EyePosition"), 1, eye_pos);
	}
};


#endif // !FOG_H