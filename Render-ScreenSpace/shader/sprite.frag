#version 330 core

in vec4 viewPos;
in vec2 Tex;


uniform mat4 projection;

out vec4 FragColor;

void main()
{
    vec3 normal;
    // normal.xy = gl_PointCoord * 2.0 - 1.0;
    normal.xy = Tex* 2.0 -1.0;

    float r2 = dot(normal.xy, normal.xy);

    if(r2 > 1.0)
    {
        discard;
    }

    normal.z = sqrt(1.0 - r2);

    vec4 pixelPos = vec4(viewPos.xyz + normal*(1), 1.0);
    vec4 clipSpacePos = projection * pixelPos;

    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w) * 0.5f + 0.5f;
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}