#version 330 core

#define PI 3.1415926535897932384626433832795

struct DirectionalLight {
    bool enabled;
    vec3 direction;
    vec3 irradiance;
};

/*
struct PointLight {
    bool enabled;
    vec3 position;
    vec3 irradiance;
    float const_atten;
    float linear_atten;
    float quad_atten;
};
*/

struct MapInfo {
    bool enabled;
    vec3 default_value;
    sampler2D map;
};

uniform DirectionalLight directional_light;
//uniform PointLight point_lights[4];
uniform MapInfo albedo_map;
uniform MapInfo metallic_map;
uniform MapInfo roughness_map;
uniform MapInfo normal_map;
uniform MapInfo ao_map;
uniform MapInfo environment_map;
uniform vec3 camera_position;

in vec2 UV;
in vec3 WorldPosition;
in mat3 TBN;

out vec4 out_color;

float clamped_cosine(vec3 a, vec3 b) {
    return min(max(dot(a, b), 0.0), 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 pbr_directional(vec3 albedo, float roughness, vec3 N, vec3 incoming_irradiance, vec3 reversed_direction, float metallic) {
    vec3 V = normalize(camera_position - WorldPosition); // eye vector
    vec3 L = -directional_light.direction; // to light vector
    vec3 H = normalize(V + L); // half-way vector

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 radiance = incoming_irradiance;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
    vec3 albedo = albedo_map.enabled ? vec3(texture(albedo_map.map, UV)) : albedo_map.default_value;
    float metallic = metallic_map.enabled ? texture(metallic_map.map, UV).b : metallic_map.default_value.b;
    float roughness = roughness_map.enabled ? texture(roughness_map.map, UV).g : roughness_map.default_value.g;
    vec3 ao = ao_map.enabled ? vec3(texture(ao_map.map, UV)) : ao_map.default_value;
    vec3 tangent_space_normal = normal_map.enabled ? vec3(texture(normal_map.map, UV)) : normal_map.default_value;
    tangent_space_normal = normalize((tangent_space_normal * 2.0) - 1.0) * vec3(1.0, -1.0, 1.0);
    vec3 normal = normalize(TBN * tangent_space_normal.xyz);

    albedo = pow(albedo, vec3(2.2));

    vec3 Lo = vec3(0);

    if (directional_light.enabled) {
        vec3 reversed_direction = -directional_light.direction;
        vec3 incoming_irradiance = directional_light.irradiance * clamped_cosine(normal, reversed_direction);
        Lo += pbr_directional(albedo, roughness, normal, incoming_irradiance, reversed_direction, metallic);
    }

    vec3 color = vec3(0.03) * albedo * ao + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    out_color = vec4(color, 1.0);
}