#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in ivec4 blockProp;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model_group;

uniform int useLODColor;

out VERTEX
{
    vec4 position;
    vec3 normal;
    vec4 color;
} vertex;

vec4 colorBase = vec4(0, 0, 0, 1);
vec4 colorLOD0 = vec4(1, 0, 0, 1);
vec4 colorLOD1 = vec4(1, 1, 0, 1);
vec4 colorLOD2 = vec4(0, 1, 0, 1);
vec4 colorLOD3 = vec4(0, 1, 1, 1);
vec4 colorLOD4 = vec4(0, 0, 1, 1);
vec4 colorLOD5 = vec4(1, 0, 1, 1);

void main()
{
    mat4 mv = view * model_group;
    gl_Position = proj * mv * (position);
    vertex.position = model_group * position;
    vertex.normal = mat3(model_group) * normal.xyz;

    if(useLODColor == 0)
    {
        vertex.color = vec4(float(blockProp.x) / 255.0f, float(blockProp.y) / 255.0f, float(blockProp.z) / 255.0f, 1.0f);
    }
    else
    {
        /*Show LODs*/
        float colorMixture = 1.0f;

        if(position.w > 0.75f)
        {
            vertex.color = colorMixture * colorLOD0 + (1 - colorMixture) * colorBase;
        }
        else if(position.w > 0.45f)
        {
            vertex.color = colorMixture * colorLOD1 + (1 - colorMixture) * colorBase;
        }
        else if(position.w > 0.20f)
        {
            vertex.color = colorMixture * colorLOD2 + (1 - colorMixture) * colorBase;
        }
        else if(position.w > 0.110f)
        {
            vertex.color = colorMixture * colorLOD3 + (1 - colorMixture) * colorBase;
        }
        else if(position.w > 0.0620f)
        {
            vertex.color = colorMixture * colorLOD4 + (1 - colorMixture) * colorBase;
        }
        else if(position.w > 0.03120f)
        {
            vertex.color = colorMixture * colorLOD5 + (1 - colorMixture) * colorBase;
        }
    }
}
