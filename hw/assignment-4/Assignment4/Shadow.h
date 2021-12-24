#include "Angel-yjc.h"
#include "Globals.h"
#include "Object.h"
#include "Sphere.h"
#include <iostream>
#include <string>
#include <vector>

class Shadow : public Object {
public:
	GLuint buffer;
	std::vector<point4> vertices;
	std::vector<color4> colors;
	int num_vertices = 0;

	mat4 shadow_matrix = mat4(
		12.0, 0.0, 0.0, 0.0,
		14.0, 0.0, 3.0, -1.0,
		0.0, 0.0, 12.0, 0.0,
		0.0, 0.0, 0.0, 12.0
	);

	void setup(const Sphere& sphere) {
		for (int i = 0; i < sphere.num_vertices; i++) {
			vertices.push_back(sphere.vertices[i]);
			color4 black = vertex_colors[5];
			colors.push_back(black);
		}
		num_vertices = vertices.size();
	}

	void init() {
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		glBufferData(GL_ARRAY_BUFFER,
			sizeof(point4) * num_vertices + sizeof(color4) * num_vertices,
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			sizeof(point4) * num_vertices, &vertices[0]);
		glBufferSubData(GL_ARRAY_BUFFER,
			sizeof(point4) * num_vertices,
			sizeof(color4) * num_vertices,
			&colors[0]);
	}

	void draw(mat4 ModelView) {
		mat4  mv = LookAt(eye, at, VUP);
		glUseProgram(nonshading_program); // Use the nonshading program
		model_view = glGetUniformLocation(nonshading_program, "model_view");
		if (solidOrWireFlag != 0)
			glUniform1i(glGetUniformLocation(nonshading_program, "LatticeFlag"), latticeFlag);

		fog.Uniform_Setup_Vars(nonshading_program, mv, eye, fogFlag);

		glUniformMatrix4fv(model_view, 1, GL_TRUE, ModelView); // GL_TRUE: matrix is row-major
		if (solidOrWireFlag == 0) // wire frame 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else if (solidOrWireFlag == 1) // solid sphere
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawObj(nonshading_program, buffer, 3, num_vertices);  // draw the shadow
		glUniform1i(glGetUniformLocation(nonshading_program, "LatticeFlag"), 0);
	}
};