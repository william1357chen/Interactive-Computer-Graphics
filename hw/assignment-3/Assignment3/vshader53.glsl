/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;

// Global Ambient Light
uniform vec4 GlobalAmbientProduct;

// Direction Light Variables
uniform vec4 DirectionAmbientProduct, DirectionDiffuseProduct, DirectionSpecularProduct;
uniform vec4 LightDirection;

uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 Normal_Matrix;

// Positional Light (Point Source or Spotlight) Variables
uniform int PointSourceFlag, SpotLightFlag; // the flag that is 1 will be used 
uniform vec4 PositionAmbientProduct, PositionDiffuseProduct, PositionSpecularProduct;
uniform vec4 PositionalLightPosition;   // Must be in Eye Frame
uniform vec4 SpotLightFocusPosition; // only used if SpotLightFlag is 1. In eye frame
uniform float CutOffAngle; // only used if SpotLightFlag is 1. In radians
uniform float SpotLightExponent;
uniform float Shininess;
uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation


void main()
{
    // Add global ambient light
    color = GlobalAmbientProduct;

    // Add directional light
    // Transform vertex position into eye coordinates
    vec3 pos = (model_view * vPosition).xyz;
	
    vec3 L = -normalize( LightDirection.xyz );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize(Normal_Matrix * vNormal);

// YJC Note: N must use the one pointing *toward* the viewer
//     ==> If (N dot E) < 0 then N must be changed to -N
//
    if ( dot(N, E) < 0 ) N = -N;
/*--- To Do: Compute attenuation for Directional Light ---*/
    float attenuation = 1.0; 

 // Compute terms in the illumination equation
    vec4 ambient = DirectionAmbientProduct;

    float d = max( dot(L, N), 0.0 );
    vec4  diffuse = d * DirectionDiffuseProduct;

    float s = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = s * DirectionSpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	    specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    color += attenuation * (ambient + diffuse + specular);

    /*--- To Do: Compute attenuation for Directional Light ---*/

    float distance = length( PositionalLightPosition.xyz - pos);
    attenuation = 1.0 / (ConstAtt + LinearAtt * distance + QuadAtt * pow(distance, 2));

    if (SpotLightFlag == 1){
        float spotlight_attenuation;
        L = normalize( PositionalLightPosition.xyz - pos );
        vec3 LF = normalize( SpotLightFocusPosition.xyz - PositionalLightPosition.xyz );

        if (dot(LF, -L) < cos(CutOffAngle))
            spotlight_attenuation = 0;
        else
            spotlight_attenuation = pow( dot(LF, -L), SpotLightExponent);

        attenuation = attenuation * spotlight_attenuation;
    }

    N = normalize(Normal_Matrix * vNormal);
    if ( dot(N, E) < 0 ) N = -N;

    L = normalize( PositionalLightPosition.xyz - pos );
    E = normalize( -pos );
    H = normalize( L + E );

    // Compute terms in the illumination equation
    ambient = PositionAmbientProduct;

    d = max( dot(L, N), 0.0 );
    diffuse = d * PositionDiffuseProduct;

    s = pow( max(dot(N, H), 0.0), Shininess );
    specular = s * PositionSpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	    specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    if (SpotLightFlag == 1 || PointSourceFlag == 1)
        color += attenuation * (ambient + diffuse + specular);

    gl_Position = projection * model_view * vPosition;
}
