vec4 Phong()
{
    vec3 ambient = fragColor * fragMaterial.z;
    vec3 totalLighting = worldAmbient.xyz + ambient;
    float attenuation;

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(fragViewVec);

    for (int i = 0; i < settings.numLights; i++)
    {
        vec4 lightOffset = ubo.lights[i].position - fragWorldPos;
        vec3 L = normalize(lightOffset).xyz;
        vec3 R = reflect(L, N);

        float distance = length(lightOffset);
        attenuation = 1.0 / (ubo.lights[i].attenuation.x
                           + ubo.lights[i].attenuation.y * distance
                           + ubo.lights[i].attenuation.z * distance * distance);

        vec3 diffuse = attenuation * max(dot(N, L), 0.0) * fragColor * ubo.lights[i].color.xyz;

        vec3 specular = attenuation
                        * pow(max(dot(R, V), 0.0), fragMaterial.y)
                        * ubo.lights[i].color.xyz
                        * vec3(fragMaterial.x);

        totalLighting += vec3(diffuse + specular);
    }

    return vec4(totalLighting, 1.0);// * (light.color * fragMaterial.w);
}
