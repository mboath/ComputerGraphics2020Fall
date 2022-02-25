#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec2 offset;
uniform int reflection;

out vec2 TexCoord;

void main()
{
    if (reflection == 0)
        gl_Position = projection * view * vec4(position.x, position.y, position.z, 1.0f);
    else
        gl_Position = projection * view * vec4(position.x, -position.y, position.z, 1.0f);
    TexCoord = vec2(texCoord.x - offset.x, 1.0f - texCoord.y + offset.y);
}
