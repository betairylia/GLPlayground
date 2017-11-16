#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in ivec4 blockProp;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_group;

uniform vec4 colorLow;
uniform vec4 colorHigh;

out VERTEX
{
    vec4 position;
    vec3 normal;
    vec4 color;
} vertex;

void main()
{
    mat4 mv = view * model_group;
    gl_Position = proj * mv * (position);
    vertex.position = model_group * position;
    vertex.normal = mat3(model_group) * normal.xyz;
    vertex.color = colorLow * (float(32-blockProp.x) / 32.0f) + colorHigh * (float(blockProp.x) / 32.0f);
    /*vertex.color = vec4(float(blockProp.x), float(blockProp.x) / 32.0f, 0.0f, 1.0f);*/
}
