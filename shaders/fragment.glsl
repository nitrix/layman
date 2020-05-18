#version 400 core

in vec2 pass_texture_coords;
in vec3 normal_in_model_space;
in vec3 to_light_vector;
in vec3 to_camera_vector;

uniform sampler2D texture_sampler;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

out vec4 out_Color;

void main(void) {
    // Normalized
    vec3 N = normalize(normal_in_model_space); // Normal
    vec3 Li = normalize(to_light_vector); // Light inverse
    vec3 Ei = normalize(to_camera_vector); // Eye inverse
    vec3 L = -Li; // Light

    // Not normalized
    vec3 RL = reflect(L, N); // Reflected light

    // Ambient lighting
    vec3 ambient = light_ambient * material_ambient;

    // Diffuse lighting
    float brightness = max(dot(N, Li), 0.0);
    vec3 diffuse = light_diffuse * material_diffuse * brightness;

    // Specular lighting
    float specular_angle = max(dot(RL, Ei), 0.0);
    vec3 specular = light_specular * material_specular * pow(specular_angle, material_shininess);

    // Phong output (ambiant + diffuse + specular)
    // out_Color = vec4(ambient + diffuse + specular, 1.0);
    out_Color = vec4(texture(texture_sampler, pass_texture_coords).xyz * (ambient + diffuse) + specular, 1.0);
}
