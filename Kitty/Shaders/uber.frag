#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "Bits/structs.frag"

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVec;
layout(location = 4) in vec4 fragWorldPos;
layout(location = 5) in vec4 fragMaterial; // x = Specular strength, y = Shininess, z = Ambient, w = Light reception
layout(location = 6) in vec4 worldAmbient;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    bool usePhong;
    uint numLights;
} settings;

layout (set = 3, binding = 0) uniform LightsUBO {
	lightSource lights[128];
} ubo;

#include "Bits/phong.frag"

void main() {
    vec4 finalColor = texture(texSampler, fragTexCoord);

    if (settings.usePhong == true)
    {
        finalColor *= Phong();
    }

    outColor = finalColor;
}
