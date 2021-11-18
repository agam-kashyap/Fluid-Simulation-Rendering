#version 330 core

in vec4 viewPos;
in vec4 FragPos;

uniform mat4 projection;
uniform float particleRadius;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 particleColor;
uniform vec3 cameraViewPos;

// out vec4 FragColor;


float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec3 N; //Normal to the sphere

    N.xy = 2.0 * gl_PointCoord - 1.0;
    if (dot(N.xy, N.xy) > 1.0) {
        discard;
    }
    N.z = sqrt(1.0 - dot(N.xy, N.xy));

    // calculate depth
    vec4 pixelPos = vec4(viewPos.xyz + N*(particleRadius), 1.0);
    vec4 clipSpacePos = projection * pixelPos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w) * 0.5f + 0.5f;

    // vec3 norm = normalize(N);
    // vec3 lightDir = normalize(lightPos - FragPos.xyz);
    // vec3 diffuse = lightColor * max(dot(norm, lightDir), 0.0);

    // FragColor = vec4(diffuse*particleColor, 1.0f);

    // float depth = LinearizeDepth(gl_FragCoord.z);
    // gl_FragDepth = depth;
}