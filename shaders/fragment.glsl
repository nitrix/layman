#version 400 core

in vec2 pass_texture_coords;
in vec3 normal_in_model_space;
in vec3 to_light_vector;
in vec3 to_camera_vector;

uniform sampler2D texture_albedo_sampler;
uniform sampler2D texture_normal_map_sampler;
uniform sampler2D texture_roughness_map_sampler;
uniform sampler2D texture_glow_map_sampler;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

out vec4 out_Color;

void main(void) {
    // Normal from the normal map
    vec3 N = texture(texture_normal_map_sampler, pass_texture_coords).rgb;
    N = N * 2 - 1;

    // Constants
    // vec3 N = normalize(normal_in_model_space); // Normal
    vec3 Li = normalize(to_light_vector); // Light inverse
    vec3 Ei = normalize(to_camera_vector); // Eye inverse
    vec3 L = -Li; // Light
    vec3 R = normalize(reflect(L, N)); // Reflected light

    // Ambient lighting
    vec3 ambient = material_ambient * light_ambient;

    // Diffuse lighting
    float diffuseBrightness = clamp(dot(Li, N), 0, 1);
    vec3 diffuse = (diffuseBrightness * material_diffuse) * light_diffuse;

    // Specular lighting
    float specularAngle = clamp(dot(R, Ei), 0, 1);
    float specularBrightness = pow(specularAngle, material_shininess);
    vec3 specular = (specularBrightness * material_specular) * light_specular;

    // Roughness from the roughness map
    vec3 roughness = texture(texture_roughness_map_sampler, pass_texture_coords).rgb;
    specular *= 1 - roughness;

    // Glow map
    vec3 glow = texture(texture_glow_map_sampler, pass_texture_coords).rgb;
    if (glow.r > 0.5) {
        diffuse = vec3(1.0);
    }

    // Phong output (ambiant + diffuse + specular)
    out_Color = texture(texture_albedo_sampler, pass_texture_coords) *
        (vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0));

    // out_Color = vec4(roughness, 1.0);
}
