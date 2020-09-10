#version 400 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;
uniform vec3 light_position;

uniform bool use_normal_map;

in vec3 position;
in vec2 texture_coords;
in vec3 normal;
in vec3 tangent;

out vec2 pass_texture_coords;
out vec3 normal_in_model_space;
out vec3 to_light_vector;
out vec3 to_camera_vector;

void main() {
    vec4 world_position = transform * vec4(position.x, position.y, position.z, 1.0);

    // Direct output
    gl_Position = projection * view * world_position;

    // Outputs for fragment shader
    pass_texture_coords = texture_coords;
    normal_in_model_space = (transform * vec4(normal, 0.0)).xyz;
    to_light_vector = light_position - world_position.xyz;
    to_camera_vector = (inverse(view) * vec4(1, 1, 1, 1.0)).xyz - world_position.xyz;

    // Normal mapping
    if (use_normal_map) {
        vec3 T = normalize((transform * vec4(tangent, 1.0)).xyz);
        vec3 N = normalize((transform * vec4(normal, 1.0)).xyz);
        vec3 B = cross(N, T);
        mat3 TBN = transpose(mat3(T, B, N)); // to tangeant space conversion matrix
        to_light_vector = TBN * to_light_vector;
        to_camera_vector = TBN * to_camera_vector;
    }
}