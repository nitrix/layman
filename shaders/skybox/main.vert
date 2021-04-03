in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main() {
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // Removes the translation from the view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    // Little swizzle trick here, so that all fragments end up at 1.0 depth.
    // Requires glDepthFunc(GL_LEQUAL);
   gl_Position = clipPos.xyww;
}