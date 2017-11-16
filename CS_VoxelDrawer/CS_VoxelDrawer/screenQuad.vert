#version 430

layout (location = 0) in vec3 pos;

out VERTEX
{
    vec2 uv;
}vertex;

void main()
{
    vertex.uv = vec2(0.5 * (pos.x + 1.0), 0.5 * (pos.y + 1.0));
    gl_Position = vec4(pos, 1.0);
}
