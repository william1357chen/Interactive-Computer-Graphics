#include "Angel-yjc.h"
#include "Globals.h"
#include "Object.h"
#include <iostream>
#include <string>
#include <vector>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class Axes : public Object {

public:
	GLuint buffer;
	static const int num_vertices = 6;
	point4 points[num_vertices]; // positions for all vertices
	color4 colors[num_vertices]; // colors for all vertices

	point4 vertices[4] = {
		point4(0.0, 0.0, 2.0, 1.0),
		point4(8.0, 0.0, 2.0, 1.0),
		point4(0.0, 8.0, 2.0, 1.0),
		point4(0.0, 0.0, 10.0, 1.0),
	};

	void setup() {
		colors[0] = vertex_colors[0]; points[0] = vertices[0];
		colors[1] = vertex_colors[0]; points[1] = vertices[1];

		colors[2] = vertex_colors[4]; points[2] = vertices[0];
		colors[3] = vertex_colors[4]; points[3] = vertices[2];

		colors[4] = vertex_colors[3]; points[4] = vertices[0];
		colors[5] = vertex_colors[3]; points[5] = vertices[3];
	}
	void init() {
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
			colors);
	}

	void draw() {
		mat4  mv = LookAt(eye, at, VUP);
		mat4  ModelView;
		glUseProgram(nonshading_program); // Use the nonshading program
		model_view = glGetUniformLocation(nonshading_program, "model_view");

		fog.Uniform_Setup_Vars(nonshading_program, mv, eye, fogFlag);

		ModelView = mv;
		glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		drawObj(nonshading_program, buffer, 2, num_vertices);  // draw the axes
	}
};