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
    // Ambient lighting
    vec3 ambient = light_ambient * material_ambient;

    // Diffuse lighting
    vec3 normalized_normal_in_model_space = normalize(normal_in_model_space);
    vec3 normalized_to_light_vector = normalize(to_light_vector);
    float brightness = max(dot(normalized_normal_in_model_space, normalized_to_light_vector), 0.0);
    vec3 diffuse = light_diffuse * material_diffuse * brightness * texture(texture_sampler, pass_texture_coords).xyz;

    // Specular lighting
    vec3 normalized_to_camera_vector = normalize(to_camera_vector);
    vec3 normalized_from_light_vector = -normalized_to_light_vector;
    vec3 reflected_light_vector = reflect(normalized_from_light_vector, normalized_normal_in_model_space);
    float specular_angle = max(dot(reflected_light_vector, normalized_to_camera_vector), 0.0);
    vec3 specular = light_specular * material_specular * pow(specular_angle, material_shininess);

    // Phong output (ambiant + diffuse + specular)
    out_Color = vec4(ambient + diffuse + specular, 1.0);
}
