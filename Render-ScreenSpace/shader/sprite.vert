#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 viewPos;
out vec2 Tex;

void main()
{
    gl_Position = projection * view * model * vec4(vec3(position, 0.0f), 1.0f);
    viewPos = view * model * vec4(vec3(position, 0.0f), 1.0f);
    Tex = texCoord;
}