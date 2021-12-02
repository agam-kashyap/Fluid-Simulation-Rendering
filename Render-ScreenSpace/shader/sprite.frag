#version 330 core

in vec4 viewPos;
in vec4 FragPos;

uniform mat4 projection;
uniform float particleRadius;

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
}