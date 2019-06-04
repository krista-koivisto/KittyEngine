struct material
{
    float specular;
    float shininess;
    float ambient;
    float lightReception;
};

struct lightSource
{
    vec4 position;
    vec4 color;
    vec4 specular;
    vec4 attenuation; // x = constant, y = linear, z = quadratic
    //float constantAttenuation, linearAttenuation, quadraticAttenuation;
    /*float spotCutoff, spotExponent;
    vec3 spotDirection;*/
};
