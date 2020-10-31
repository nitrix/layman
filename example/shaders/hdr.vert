#version 460 core

out vec2 UV;

void main() {
    vec2 vertices[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
    gl_Position = vec4(vertices[gl_VertexID], 0, 1);
    UV = 0.5 * gl_Position.xy + vec2(0.5);
    //UV = (vec2(gl_Position.x, gl_Position.y) + 1.0) / 2.0;
}