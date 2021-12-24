#include "Angel-yjc.h"
#include "Globals.h"
#include <iostream>
#include <string>
#include <vector>


#ifndef OBJECT_H
#define OBJECT_H


class Object{
public:

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
            if (shadingFlag == 1 && num_vertices != 6) { // smooth shading (only for sphere)
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
                    BUFFER_OFFSET(sizeof(point4) * num_vertices 
                       + sizeof(color4) * num_vertices));
            }
            glDrawArrays(GL_TRIANGLES, 0, num_vertices);

            glDisableVertexAttribArray(vPosition);
            glDisableVertexAttribArray(vNormal);
        }
    }
};

#endif