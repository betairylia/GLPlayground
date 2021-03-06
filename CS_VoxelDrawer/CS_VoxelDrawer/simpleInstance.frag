#version 430

in VERTEX
{
    vec4 position;
    vec3 normal;
    vec4 color;
} vertex;

out vec4 frag_colour;

void main()
{
    if(vertex.color.a == 0.0f)
    {
        discard;
    }

    float diffuse = 0.4f + 0.3f * dot(normalize(vertex.normal), normalize(vec3(1.0f, 2.0f, 0.0f)));

    frag_colour = vec4(diffuse * vertex.color.rgb, 1.0);
}
