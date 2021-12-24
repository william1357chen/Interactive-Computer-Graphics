

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec3 Velocity;
in  vec4 vColor;

out vec4 color;
out vec4 fPosition;

uniform float Time;

uniform mat4 model_view;
uniform mat4 projection;


void main() 
{
    vec3 init = vec3(0.0, 0.1, 0.0);
    vec4 vPosition;
    vPosition.x = init.x + 0.001 * Velocity.x * Time;
    vPosition.z = init.z + 0.001 * Velocity.z * Time;
    vPosition.y = init.y + 0.001 * Velocity.y * Time + 0.5 * -0.00000049 * Time * Time;
    vPosition.w = 1.0;

    gl_Position = projection * model_view * vPosition;

    color = vColor;
    fPosition = vPosition;

} 
