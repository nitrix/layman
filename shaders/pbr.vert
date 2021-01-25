#version 330 core

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

uniform mat4 viewProjectionMatrix;
uniform mat4 skyProjectionMatrix;
uniform mat4 sceneRotationMatrix;

out vec3 Position;
out vec2 UV;
out mat3 TangentBasis;

void main() {
	Position = vec3(sceneRotationMatrix * vec4(position, 1.0));
	UV = vec2(uv.x, 1.0 - uv.y);
	TangentBasis = mat3(sceneRotationMatrix) * mat3(tangent, bitangent, normal);

	gl_Position = viewProjectionMatrix * sceneRotationMatrix * vec4(position, 1.0);
}