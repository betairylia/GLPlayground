#version 430

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;

uniform ivec2 windowSizeDiv2;

out VERTEX
{
    vec2 uv;
} vertex;

void main()
{
    // Output position of the vertex, in clip space
    // map [0..800][0..600] to [-1..1][-1..1]
    vec2 vertexPosition_homoneneousSpace = position - windowSizeDiv2; // [0..800][0..600] -> [-400..400][-300..300]
    vertexPosition_homoneneousSpace /= windowSizeDiv2;
    gl_Position =  vec4(vertexPosition_homoneneousSpace, 0, 1);

    // UV of the vertex. No special space for this one.
    vertex.uv = texcoord;
}
