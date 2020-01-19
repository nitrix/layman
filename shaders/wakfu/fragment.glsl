#version 400 core

in vec2 pass_texture_coords;
in vec3 transformed_normal;
in vec3 to_light_vector;

out vec4 out_Color;

uniform sampler2D texture_sampler;
uniform vec3 light_color;

void main(void) {
    // Diffuse lighting
    vec3 normalized_transformed_normal = normalize(transformed_normal);
    vec3 normalized_to_light_vector = normalize(to_light_vector);
    float brightness = max(dot(normalized_transformed_normal, normalized_to_light_vector), 0.0);
    vec3 diffuse = brightness * light_color;

    // Output
    out_Color = vec4(diffuse, 0.0) * texture(texture_sampler, pass_texture_coords);
}