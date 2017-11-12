#version 430

in VERTEX
{
    vec3 normal;
} vertex;

out vec4 frag_colour;

void main()
{
    float diffuse = 0.4f + 0.3f * dot(normalize(vertex.normal), normalize(vec3(1.0f, 2.0f, 0.0f)));
    vec3 color = vec3(0.5, 0.8, 1.0);

    frag_colour = vec4(diffuse * color.xyz, 1.0);
}
