#version 430

uniform sampler2D samplerPosition;
uniform sampler2D samplerNormal;
uniform sampler2D samplerColor;

in VERTEX
{
    vec2 uv;
}vertex;

layout(location = 0) out vec4 aoMap;

int sampleCount = 8;
int selAngle[8] = int[](2, 4, 3, 7, 6, 5, 0, 1);
int selDist[8]  = int[](3, 2, 4, 7, 1, 6, 5, 0);
float fSelAngle = 6.2831853 / sampleCount;
float fSelDist  = 1.0 / sampleCount;
float fScaler = 1.0;
float fRadius = 3.0;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 colorHigh = vec3(41.0f / 255.0f, 128.0f / 255.0f, 185.0f / 255.0f);
vec3 colorLow = vec3(44.0f / 255.0f, 62.0f / 255.0f, 80.0f / 255.0f);

void main()
{
    vec3 clr = texture(samplerColor, vertex.uv).rgb;
    if(clr.r == 0.0f && clr.g == 0.0f && clr.b == 0.0f)
    {
        aoMap = vec4(colorHigh * vertex.uv.y + colorLow * (1.0f - vertex.uv.y), 1.0);
        return;
    }

    vec2 vSampleVector;
    vec3 vPos = texture(samplerPosition, vertex.uv).xyz;
    vec3 vNormal = texture(samplerNormal, vertex.uv).xyz;
    float occ = 0.0;
    for(int i = 0; i < sampleCount; ++i)
    {
        int n = selAngle[i];
        int m = selDist[i];
        vSampleVector = vec2(cos(fSelAngle * n), sin(fSelAngle * n)) * (fSelDist * (m + 1)) * fScaler / vPos.z;
        float rAngle = 3.14159265 * rand(vPos.yx);
        vSampleVector = mat2(cos(rAngle), -sin(rAngle), sin(rAngle), cos(rAngle)) * vSampleVector;
        vec3 sPos = texture(samplerPosition, vertex.uv + vSampleVector).rgb;
        vec3 sdPos = sPos - vPos;
        float fAbsV = dot(sdPos, sdPos);
        float fRes = 0;
        fRes = max(0.0, 1.0 - sqrt(fAbsV) / fRadius) * max(0.0, dot(sdPos / sqrt(fAbsV), vNormal) - 0.01);
        occ += fScaler * fRes;
    }
    occ = 1 - (occ / sampleCount);
    aoMap = vec4(clr * occ, 1.0);
    /*aoMap = vec4(occ, occ, occ, 1.0f);*/
}
