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

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 CookTorranceBRDF(vec3 V, vec3 L, vec3 H, vec3 N, vec3 albedo, float roughness, float metallic, out vec3 F) {
    // Normal distribution function: approximates the amount the surface's microfacets are aligned to the halfway vector,
    // influenced by the roughness of the surface; this is the primary function approximating the microfacets.
    float D = DistributionGGX(N, H, roughness);

    // Geometry function: describes the self-shadowing property of the microfacets. When a surface is relatively rough,
    // the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
    float G = GeometrySmith(N, V, L, roughness);

    // Fresnel equation: The Fresnel equation describes the ratio of surface reflection at different surface angles.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // The actual equation.
    vec3 numerator    = D * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    return numerator / max(denominator, 0.001); // Prevent division by zero.
}

vec3 calculate_directional_lighting(vec3 V, vec3 L, vec3 H, vec3 N, vec3 radiance, vec3 albedo, float roughness, float metallic) {
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // Each analytical light (directional).
    for (int i = 0; i < 1; ++i) {
        vec3 F;
        vec3 specular = CookTorranceBRDF(V, L, H, N, albedo, roughness, metallic, F);

        // Ratios.
        vec3 kS = F;              // Reflection/specular ratio.
        vec3 kD = vec3(1.0) - kS; // Refraction/diffuse ratio.
        kD *= 1.0 - metallic;

        // Outgoing radiance.
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    return Lo;
}

void main() {
    vec3 albedo = albedo_map.enabled ? vec3(texture(albedo_map.map, UV)) : albedo_map.default_value;
    vec3 ao = ao_map.enabled ? vec3(texture(ao_map.map, UV)) : ao_map.default_value;
    float metallic = metallic_map.enabled ? texture(metallic_map.map, UV).b : metallic_map.default_value.b;
    float roughness = roughness_map.enabled ? texture(roughness_map.map, UV).g : roughness_map.default_value.g;
    vec3 tangent_space_normal = normal_map.enabled ? vec3(texture(normal_map.map, UV)) : normal_map.default_value;
    tangent_space_normal = normalize((tangent_space_normal * 2.0) - 1.0) * vec3(1.0, -1.0, 1.0);
    vec3 normal = normalize(TBN * tangent_space_normal.xyz);

    // Convert sRGB textures to linear space.
    albedo = pow(albedo, vec3(2.2));
    albedo = pow(albedo, vec3(2.2));

    // Direct lighting.
    vec3 direct_lighting = vec3(0);

    if (directional_light.enabled) {
        vec3 V = normalize(camera_position - WorldPosition); // vector to camera
        vec3 L = -directional_light.direction; // vector to light
        vec3 H = normalize(V + L); // half-way vector
        vec3 N = normal;
        vec3 irradiance = directional_light.irradiance * clamp(dot(N, L), 0, 1);
        direct_lighting += calculate_directional_lighting(V, L, H, N, irradiance, albedo, roughness, metallic);
    }

    // Ambient lighting.
    vec3 ambient_lighting = vec3(0.2) * albedo * ao;

    // Final lighting.
    vec3 color = direct_lighting + ambient_lighting;

    // Gamma correction.
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    out_color = vec4(color, 1);
}