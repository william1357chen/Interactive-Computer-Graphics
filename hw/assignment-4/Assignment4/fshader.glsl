

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in vec4 color;
in vec4 fPosition;
in vec2 texCoord;
in vec2 latticeTexCoord;
uniform int SphereTextureFlag;
uniform int FloorTextureFlag;
uniform int LatticeFlag;

out vec4 fColor;



// Texture variables
uniform sampler2D texture_2D;
uniform sampler1D texture_1D;

// Fog variables
uniform int FogFlag;
uniform float FogLinearStart;
uniform float FogLinearEnd;
uniform float FogDensity;
uniform vec4 FogColor;
uniform vec4 EyePosition;

vec4 fogBlend(vec4 prevColor){
    float z = length(fPosition);
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
    if (LatticeFlag == 1){
        if (fract(4 * latticeTexCoord[0]) < 0.35 && fract(4 * latticeTexCoord[1]) < 0.35)
            discard;
    }
    
    if (FloorTextureFlag == 1)
        fColor = color * texture(texture_2D, texCoord);
    else if (SphereTextureFlag == 1){
        fColor = color * texture(texture_1D, texCoord[0]);
    }
    else if (SphereTextureFlag == 2){
        vec4 tempColor = texture(texture_2D, texCoord);
        if (tempColor.r == 0)
            tempColor = vec4(0.9, 0.1, 0.1, 1.0);
        fColor = color * tempColor;
    }
    else
       fColor = color;

    vec4 fog_blend = fogBlend(fColor);
    float alpha = color.a;
    fColor = vec4(fog_blend.r, fog_blend.g, fog_blend.b, alpha);
} 

