#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 blockPos;
layout (location = 3) in ivec4 blockProp;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_group;

uniform vec4 colorLow;
uniform vec4 colorHigh;

out VERTEX
{
    vec3 normal;
    vec4 color;
} vertex;

void main()
{
    mat4 mv = view * model_group;
    gl_Position = proj * mv * (position + blockPos);
    vertex.normal = mat3(model_group) * normal;
    vertex.color = colorLow * (float(blockProp.x) / 32.0f) + colorHigh * (float(32-blockProp.x) / 32.0f);
}
