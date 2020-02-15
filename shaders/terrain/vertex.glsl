#version 400 core

in vec3 position;
in vec2 texture_coords;
in vec3 normal;

out vec2 pass_texture_coords;
out vec3 normal_in_model_space;
out vec3 to_light_vector;
out vec3 to_camera_vector;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_position;

void main(void) {
    vec4 world_position = model * vec4(position.x, position.y, position.z, 1.0);

    // Direct output
    gl_Position = projection * view * world_position;

    // Outputs for fragment shader
    pass_texture_coords = texture_coords * 128; // FIXME Seems like a hack, relying on GL_REPEAT.
    normal_in_model_space = (model * vec4(normal, 0.0)).xyz;
    to_light_vector = light_position - world_position.xyz;
    to_camera_vector = (inverse(view) * vec4(0, 0, 0, 1.0)).xyz - world_position.xyz;
}
