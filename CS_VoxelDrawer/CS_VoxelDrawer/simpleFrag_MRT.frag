#version 430

in VERTEX
{
    vec4 position;
    vec3 normal;
    vec4 color;
} vertex;

layout(location = 0) out vec4 worldPosition;
layout(location = 1) out vec4 worldNormal;
layout(location = 2) out vec4 fragColor;

void main()
{
    float diffuse = 0.4f + 0.3f * dot(normalize(vertex.normal), normalize(vec3(1.0f, 2.0f, 0.0f)));

    worldPosition = vertex.position;
    worldNormal = vec4(vertex.normal, 1.0f);
    fragColor = vec4(diffuse * vertex.color.rgb, 1.0f);
}
