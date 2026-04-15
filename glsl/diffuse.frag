#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#include "push.glsl"

layout(binding = 0) uniform sampler2D textures[];

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 outColor;

float getFogFactor(float d)
{
    const float FogMax = 80.0;
    const float FogMin = 0.0;

    if (d >= FogMax) return 1;
    if (d <= FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main() {
    vec4 albedo = texture(textures[push.texture_id], in_uv);

    float fogDistance = gl_FragCoord.z / gl_FragCoord.w;
    float fogAmount = getFogFactor(fogDistance);
    vec4 fogColor = vec4(vec3(0.25), 1.0);

    outColor = mix(albedo, fogColor, fogAmount);
    // outColor = vec4(in_uv, 0.0, 1.0);
}
