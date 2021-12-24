

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in vec4 color;
in vec4 fPosition;

out vec4 fColor;

uniform mat4 model_view;

// Fog variables
uniform int FogFlag;
uniform float FogLinearStart;
uniform float FogLinearEnd;
uniform float FogDensity;
uniform vec4 FogColor;
uniform vec4 EyePosition;

vec4 fogBlend(vec4 prevColor){
    float z = length(model_view * fPosition);
    float f;
    if (FogFlag == 0)
        f = 1.0;
    else if (FogFlag == 1)
        f = (FogLinearEnd - z) / (FogLinearEnd - FogLinearStart);
    else if (FogFlag == 2)
        f = exp(-(FogDensity * z));
    else if (FogFlag ==  3)
        f = exp(-pow(FogDensity * z, 2));

    f = clamp(f, 0, 1);

    fColor = mix(FogColor, prevColor, f);
    
    vec4 fog_blend = mix(FogColor, prevColor, f);
    return fog_blend;
}

void main() 
{
    if (fPosition.y < 0.1)
        discard;

    fColor = color;
    vec4 fog_blend = fogBlend(fColor);
    float alpha = color.a;
    fColor = vec4(fog_blend.r, fog_blend.g, fog_blend.b, alpha);
} 

