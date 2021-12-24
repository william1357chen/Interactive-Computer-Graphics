/***************************
 * File: vshader42.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec4 vColor;
in vec2 vTexCoord;

out vec4 color;
out vec4 fPosition;
out vec2 texCoord;
out vec2 latticeTexCoord;
uniform int SphereTextureFlag;
uniform int FloorTextureFlag;
uniform int VerOrSlantFlag;
uniform int ObjectOrEyeFlag;
uniform int LatticeFlag;
uniform int UprightOrTiltFlag;

uniform mat4 model_view;
uniform mat4 projection;



vec2 computeTexture(){
    vec2 result;
    float s;
    float t;
    if (FloorTextureFlag == 1)
        result = vTexCoord;
    else if (SphereTextureFlag == 1){
        if (ObjectOrEyeFlag == 0){
            if (VerOrSlantFlag == 0)
                s = 2.5 * vPosition.x;
            else if (VerOrSlantFlag == 1)
                s = 1.5 * (vPosition.x + vPosition.y + vPosition.z);
            t = 0;
        }
        else if (ObjectOrEyeFlag == 1){
            vec4 pos = model_view * vPosition;
            if (VerOrSlantFlag == 0)
                s = 2.5 * pos.x;
            else if (VerOrSlantFlag == 1)
                s = 1.5 * (pos.x + pos.y + pos.z);
            t = 0;
        }
        result = vec2(s, t);
    }
    else if (SphereTextureFlag == 2){
        if (ObjectOrEyeFlag == 0){
            if (VerOrSlantFlag == 0){
                s = 0.5 * (vPosition.x + 1); 
                t = 0.5 * (vPosition.y + 1);
            }
            else if (VerOrSlantFlag == 1){
                s = 0.3 * (vPosition.x + vPosition.y + vPosition.z);
                t = 0.3 * (vPosition.x - vPosition.y + vPosition.z);
            }
        }
        else if (ObjectOrEyeFlag == 1){
            vec4 pos = model_view * vPosition;
            if (VerOrSlantFlag == 0){
                s = 0.5 * (pos.x + 1); 
                t = 0.5 * (pos.y + 1);
            }
            else if (VerOrSlantFlag == 1){
                s = 0.3 * (pos.x + pos.y + pos.z);
                t = 0.3 * (pos.x - pos.y + pos.z);
            }
        }
        result = vec2(s, t);
    }
    return result;
}

vec2 computeLattice(){
    float s;
    float t;
    if (UprightOrTiltFlag == 0){
        s = 0.5 * (vPosition.x + 1);
        t = 0.5 * (vPosition.y + 1);
    }
     
    else if (UprightOrTiltFlag == 1){
        s = 0.3 * (vPosition.x + vPosition.y + vPosition.z);
        t = 0.3 * (vPosition.x - vPosition.y + vPosition.z);
    }
    return vec2(s, t);
}

void main() 
{

    gl_Position = projection * model_view * vPosition;

    color = vColor;
    fPosition = model_view * vPosition;

    
    texCoord = computeTexture();
    latticeTexCoord = computeLattice();
} 
