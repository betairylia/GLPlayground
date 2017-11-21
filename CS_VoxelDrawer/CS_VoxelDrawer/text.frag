#version 430

uniform sampler2D textTexture;

in VERTEX
{
    vec2 uv;
} vertex;

out vec4 outColor;

void main()
{
    outColor = texture( textTexture, vertex.uv );
    /*outColor = vec4(1, 1, 1, 1);*/
}
