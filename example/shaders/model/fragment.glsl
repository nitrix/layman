#version 400 core

in vec2 pass_texture_coords;
in vec3 normal_in_model_space;
in vec3 to_light_vector;
in vec3 to_camera_vector;

uniform sampler2D texture_albedo_sampler;
uniform sampler2D texture_normal_map_sampler;
uniform sampler2D texture_roughness_map_sampler;
uniform sampler2D texture_emission_map_sampler;

uniform bool use_normal_map;
uniform bool use_roughness_map;
uniform bool use_emission_map;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

out vec4 out_Color;

void main(void) {
    // Constants
    vec3 N = normalize(normal_in_model_space); // Normal

    // Normal from the normal map
    if (use_normal_map) {
        N = texture(texture_normal_map_sampler, pass_texture_coords).rgb;
        N = N * 2 - 1;
    }

    // Constants
    vec3 Li = normalize(to_light_vector); // Light inverse
    vec3 Ei = normalize(to_camera_vector); // Eye inverse
    vec3 L = -Li; // Light
    vec3 R = normalize(reflect(L, N)); // Reflected light

    // Ambient lighting
    vec3 ambient = material_ambient * light_ambient;

    // Diffuse lighting
    float diffuseBrightness = max(dot(Li, N), 0);
    vec3 diffuse = (diffuseBrightness * material_diffuse) * light_diffuse;

    // Specular lighting
    float specularAngle = max(dot(R, Ei), 0);
    float specularBrightness = pow(specularAngle, material_shininess);
    vec3 specular = (specularBrightness * material_specular) * light_specular;

    // Roughness from the roughness map
    if (use_roughness_map) {
        float roughness = texture(texture_roughness_map_sampler, pass_texture_coords).y;
        specular *= 1 - roughness;
    }

    // Emission map
    if (use_emission_map) {
        vec3 emission = texture(texture_emission_map_sampler, pass_texture_coords).rgb;
        if (emission.r > 0) {
            diffuse = vec3(emission.r);
        }
    }

    // Phong output (ambiant + diffuse + specular)
    vec4 albedo = texture(texture_albedo_sampler, pass_texture_coords);
    out_Color = albedo * vec4(ambient + diffuse + specular, 1.0);
}
