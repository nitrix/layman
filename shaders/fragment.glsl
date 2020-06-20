#version 400 core

in vec2 pass_texture_coords;
in vec3 normal_in_model_space;
in vec3 to_light_vector;
in vec3 to_camera_vector;

uniform sampler2D texture_albedo_sampler;
uniform sampler2D texture_normal_map_sampler;

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

    // Phong output (ambiant + diffuse + specular)
    out_Color = texture(texture_albedo_sampler, pass_texture_coords) *
        (vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0));
}
