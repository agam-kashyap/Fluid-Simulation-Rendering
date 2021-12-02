#version 330 core
layout (location = 0) in vec3 position;
// layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float particleRadius;
uniform float heightOfNearPlane;

out vec4 viewPos;
out vec4 FragPos;

// out vec2 Tex;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    viewPos = view * model * vec4(position, 1.0f);
    FragPos = model * vec4(position, 1.0f);

    gl_PointSize = heightOfNearPlane * (particleRadius/gl_Position.w);  //https://gamedev.stackexchange.com/questions/54391/scaling-point-sprites-with-distance
}