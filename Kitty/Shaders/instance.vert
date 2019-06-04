#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "Bits/structs.frag"

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 worldAmbient;
} ubo;

layout (set = 2, binding = 0) uniform DynamicUBO {
	mat4 matrix;
	vec4 material;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 instancePos;
layout(location = 5) in vec3 instanceRot;
layout(location = 6) in float instanceScale;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragViewVec;
layout(location = 4) out vec4 fragWorldPos;
layout(location = 5) out vec4 fragMaterial;
layout(location = 6) out vec4 worldAmbient;

void main() {
    vec4 worldPos = model.matrix * vec4((inPosition * instanceScale) + instancePos, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(model.matrix) * inNormal;
    fragViewVec = (ubo.view * worldPos).xyz;
    fragWorldPos = worldPos;
    fragMaterial = model.material;
    worldAmbient = ubo.worldAmbient;
}

