#include "Angel-yjc.h"
#include "Globals.h"
#include "Object.h"
#include <iostream>
#include <string>
#include <vector>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class Firework : public Object {

public:

	GLuint buffer;
	static const int num_vertices = 300;
	vec3 velocites[num_vertices]; // positions for all vertices
	color4 colors[num_vertices]; // colors for all vertices

	float Tmax = 10.0 / 0.001;

	void setup() {
		for (int i = 0; i < num_vertices; i++) {
			float x = 2.0 * ((rand() % 256) / 256.0 - 0.5);
			float z = 2.0 * ((rand() % 256) / 256.0 - 0.5);
			float y = 1.2 * 2.0 * ((rand() % 256) / 256.0);
			velocites[i] = vec3(x, y, z);

			/*std::cout << velocites[i] << "at index : " << i << std::endl;*/

			float r = (rand() % 256) / 256.0;
			float g = (rand() % 256) / 256.0;
			float b = (rand() % 256) / 256.0;
			colors[i] = color4(r, g, b, 1.0);
			//std::cout << colors[i] << "at index : " << i << std::endl;
		}
	}
	void init() {
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(velocites) + sizeof(colors),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(velocites), velocites);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(velocites), sizeof(colors),
			colors);
	}

	void draw() {
		mat4  mv = LookAt(eye, at, VUP);
		mat4  ModelView;
		glUseProgram(firework_program); 
		model_view = glGetUniformLocation(firework_program, "model_view");

		fog.Uniform_Setup_Vars(firework_program, mv, eye, fogFlag);

		if ((T - Tsub) >= Tmax) 
			Tsub = (float) glutGet(GLUT_ELAPSED_TIME);
		T = (float) glutGet(GLUT_ELAPSED_TIME);
		
		//std::cout << T - Tsub << std::endl;
		glUniform1f(glGetUniformLocation(firework_program, "Time"), (T-Tsub));

		ModelView = mv;
		glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		GLuint Velocity = glGetAttribLocation(firework_program, "Velocity");
		glEnableVertexAttribArray(Velocity);
		glVertexAttribPointer(Velocity, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(firework_program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(velocites)));
		glPointSize(3.0);
		glDrawArrays(GL_POINTS, 0, num_vertices);

		glDisableVertexAttribArray(Velocity);
		glDisableVertexAttribArray(vColor);

		
	}
};