#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 blockPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_group;

out VERTEX
{
    vec3 normal;
} vertex;

void main()
{
    mat4 mv = view * model_group;
    gl_Position = proj * mv * (position + blockPos);
    vertex.normal = mat3(model_group) * normal;
}
