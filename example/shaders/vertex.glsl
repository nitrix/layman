#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 tangent;

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 projection_transform;

out vec2 UV;
out vec3 WorldPosition;
out mat3 TBN;

void main() {
    gl_Position = projection_transform * view_transform * model_transform * vec4(position, 1.0);
    WorldPosition = vec3(model_transform * vec4(position, 1.0));
    UV = uv;

    // TODO: That's incredibly expensive, should be computed outside this shader.
    mat4 normal_transform = transpose(inverse(model_transform));

    // TBN: Tangent space to world space.
    vec3 T = normalize(mat3(normal_transform) * tangent.xyz);
    vec3 N = normalize(mat3(normal_transform) * normal);
    vec3 B = normalize(cross(T, N));
    TBN = mat3(T, B, N);
}