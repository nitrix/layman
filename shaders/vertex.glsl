#version 400 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;
uniform vec3 light_position;

in vec3 position;
in vec2 texture_coords;
in vec3 normal;
in vec3 tangent;

out vec2 pass_texture_coords;
out vec3 normal_in_model_space;
out vec3 to_light_vector;
out vec3 to_camera_vector;
out mat3 TBN;

void main() {
    vec4 world_position = transform * vec4(position.x, position.y, position.z, 1.0);

    // Direct output
    gl_Position = projection * view * world_position;

    // Normal mapping
    vec3 T = normalize(vec3(transform * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(transform * vec4(normal, 0.0)));
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    // Outputs for fragment shader
    pass_texture_coords = texture_coords;
    normal_in_model_space = (transform * vec4(normal, 0.0)).xyz;
    to_light_vector = light_position - world_position.xyz;
    to_camera_vector = (inverse(view) * vec4(0, 0, 0, 1.0)).xyz - world_position.xyz;
}