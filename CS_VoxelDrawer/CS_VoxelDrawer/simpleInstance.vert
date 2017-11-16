#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 blockPos;
layout (location = 3) in ivec4 blockProp;

/*blockProp.x: block height for coloring
blockProp.y: faces of block needs to be rendered*/

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
    /*Is this face need to be rendered?*/
    if( ((normal.x < -0.5f) && ((blockProp.y & 1 )== 0)) ||
        ((normal.y < -0.5f) && ((blockProp.y & 2 )== 0)) ||
        ((normal.z < -0.5f) && ((blockProp.y & 4 )== 0)) ||
        ((normal.x >  0.5f) && ((blockProp.y & 8 )== 0)) ||
        ((normal.y >  0.5f) && ((blockProp.y & 16)== 0)) ||
        ((normal.z >  0.5f) && ((blockProp.y & 32)== 0)) )
    {
        gl_Position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        vertex.position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        vertex.normal = vec3(0.0f, 0.0f, 0.0f);
        vertex.color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        mat4 mv = view * model_group;
        gl_Position = proj * mv * (position + blockPos);
        vertex.position = model_group * (position + blockPos);
        vertex.normal = mat3(model_group) * normal;
        vertex.color = colorLow * (float(blockProp.x) / 32.0f) + colorHigh * (float(32-blockProp.x) / 32.0f);
    }
}
