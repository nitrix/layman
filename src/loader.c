#include "loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct model *loader_load_model(const char *filepath) {
    float *final_vertices = NULL;
    float *final_textures = NULL;
    float *final_normals = NULL;
    unsigned int *final_faces = NULL;
    size_t final_vertex_count = 0;
    size_t final_face_count = 0;

    tk_collection(struct vector3f) vertices;
    tk_collection(struct vector3f) normals;
    tk_collection(struct vector2f) textures;
    tk_collection(struct vector3i) faces;

    tk_collection_init(&vertices, NULL);
    tk_collection_init(&textures, NULL);
    tk_collection_init(&normals, NULL);
    tk_collection_init(&faces, NULL);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Unable to load file\n");
        //TODO: cleanup
    }

    char line[1024];
    while (fgets(line, TK_COUNT(line), file)) {
        if (strncmp("v ", line, 2) == 0) {
            struct vector3f v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            tk_collection_insert(&vertices, v);
        }
        else if (strncmp("vt ", line, 3) == 0) {
            struct vector2f v;
            sscanf(line, "vt %f %f", &v.x, &v.y);
            tk_collection_insert(&textures, v);
        }
        else if (strncmp("vn ", line, 3) == 0) {
            struct vector3f v;
            sscanf(line, "vn %f %f %f", &v.x, &v.y, &v.z);
            tk_collection_insert(&normals, v);
        }
        else if (strncmp("f ", line, 2) == 0) {
            if (!final_vertices) {
                final_vertex_count = tk_collection_count(&vertices);
                final_vertices = malloc(final_vertex_count * 3 * sizeof (float));
            }

            if (!final_normals) {
                final_normals = malloc(final_vertex_count * 3 * sizeof (float));
            }

            if (!final_textures) {
                final_textures = malloc(final_vertex_count * 2 * sizeof (float));
            }

            int id_v1, id_vt1, id_vn1;
            int id_v2, id_vt2, id_vn2;
            int id_v3, id_vt3, id_vn3;
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &id_v1, &id_vt1, &id_vn1, &id_v2, &id_vt2, &id_vn2, &id_v3, &id_vt3, &id_vn3);

            struct vector3i face = {
                    .x = id_v1,
                    .y = id_v2,
                    .z = id_v3,
            };

            tk_collection_insert(&faces, face);

            // First
            int dest_index = id_v1 - 1;
            struct vector3f first_vertex = tk_collection_at(&vertices, id_v1 - 1);
            final_vertices[dest_index*3 + 0] = first_vertex.x;
            final_vertices[dest_index*3 + 1] = first_vertex.y;
            final_vertices[dest_index*3 + 2] = first_vertex.z;
            struct vector2f first_texture = tk_collection_at(&textures, id_vt1 - 1);
            final_textures[dest_index*2 + 0] = first_texture.x;
            final_textures[dest_index*2 + 1] = 1 - first_texture.y;
            struct vector3f first_normal = tk_collection_at(&normals, id_vn1 - 1);
            final_normals[dest_index*3 + 0] = first_normal.x;
            final_normals[dest_index*3 + 1] = first_normal.y;
            final_normals[dest_index*3 + 2] = first_normal.z;

            // Second
            dest_index = id_v2 - 1;
            struct vector3f second_vertex = tk_collection_at(&vertices, id_v2 - 1);
            final_vertices[dest_index*3 + 0] = second_vertex.x;
            final_vertices[dest_index*3 + 1] = second_vertex.y;
            final_vertices[dest_index*3 + 2] = second_vertex.z;
            struct vector2f second_texture = tk_collection_at(&textures, id_vt2 - 1);
            final_textures[dest_index*2 + 0] = second_texture.x;
            final_textures[dest_index*2 + 1] = 1 - second_texture.y;
            struct vector3f second_normal = tk_collection_at(&normals, id_vn2 - 1);
            final_normals[dest_index*3 + 0] = second_normal.x;
            final_normals[dest_index*3 + 1] = second_normal.y;
            final_normals[dest_index*3 + 2] = second_normal.z;

            // Third
            dest_index = id_v3 - 1;
            struct vector3f third_vertex = tk_collection_at(&vertices, id_v3 - 1);
            final_vertices[dest_index*3 + 0] = third_vertex.x;
            final_vertices[dest_index*3 + 1] = third_vertex.y;
            final_vertices[dest_index*3 + 2] = third_vertex.z;
            struct vector2f third_texture = tk_collection_at(&textures, id_vt3 - 1);
            final_textures[dest_index*2 + 0] = third_texture.x;
            final_textures[dest_index*2 + 1] = 1 - third_texture.y;
            struct vector3f third_normal = tk_collection_at(&normals, id_vn3 - 1);
            final_normals[dest_index*3 + 0] = third_normal.x;
            final_normals[dest_index*3 + 1] = third_normal.y;
            final_normals[dest_index*3 + 2] = third_normal.z;
        }
    }

    fclose(file);

    final_face_count = tk_collection_count(&faces);
    final_faces = malloc(final_face_count * 3 * sizeof (unsigned int));

    for (size_t i = 0; i < tk_collection_count(&faces); i++) {
        struct vector3i face = tk_collection_at(&faces, i);
        final_faces[i*3+0] = face.x - 1;
        final_faces[i*3+1] = face.y - 1;
        final_faces[i*3+2] = face.z - 1;
    }

    tk_collection_fini(&vertices);
    tk_collection_fini(&normals);
    tk_collection_fini(&textures);
    tk_collection_fini(&faces);

    // TODO: Error handling
    return model_create_from_raw(final_vertices, final_vertex_count, final_faces, final_face_count, final_textures, final_normals);
}

struct texture *loader_load_texture(const char *filepath) {
    struct texture *texture = malloc(sizeof *texture);

    if (!texture) {
        return NULL;
    }

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    int width, height, comp;
    unsigned char *image = stbi_load(filepath, &width, &height, &comp, STBI_rgb);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);

    // Clamping pixels that are out of bound.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Filtering (what happens when the texture's pixels don't quite match the resolution of what's being pasted on).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}


GLuint loader_load_shader_file(const char *filepath, GLenum shader_type) {
    FILE *file = fopen(filepath, "rb");
    fseek(file, 0, SEEK_END);
    GLint length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = malloc(length);
    fread(content, 1, length, file);
    fclose(file);

    GLuint shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, (const char **) &content, &length);
    glCompileShader(shader_id);

    free(content);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar log[1024];
        glGetShaderInfoLog(shader_id, sizeof log, NULL, log);

        fprintf(stderr, "Unable to load shader `%s`!\n%s", filepath, log);

        return 0;
    }

    return shader_id;
}

struct shader *loader_load_shader(const char *name) {
    struct shader *shader = malloc(sizeof *shader);

    if (!shader) {
        return NULL;
    }

    // Copy name
    shader->name = strdup(name);

    // Generate paths
    size_t size = 0;
    size = snprintf(NULL, 0, "shaders/%s/vertex.glsl", name);
    char vertex_path[size + 1];
    sprintf(vertex_path, "shaders/%s/vertex.glsl", name);
    size = snprintf(NULL, 0, "shaders/%s/fragment.glsl", name);
    char fragment_path[size + 1];
    sprintf(fragment_path, "shaders/%s/fragment.glsl", name);

    shader->vertex_shader_id = loader_load_shader_file(vertex_path, GL_VERTEX_SHADER);
    shader->fragment_shader_id = loader_load_shader_file(fragment_path, GL_FRAGMENT_SHADER);

    // Create the shader program
    shader->program_id = glCreateProgram();
    glAttachShader(shader->program_id, shader->vertex_shader_id);
    glAttachShader(shader->program_id, shader->fragment_shader_id);

    // Binding shader input variables to attributes.
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_VERTEX_COORDINATES, "position");
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_TEXTURE_COORDINATES, "texture_coords");
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_NORMALS, "normal");

    // Linking
    glLinkProgram(shader->program_id);

    // Verify the linking went well
    GLint status = 0;
    glGetProgramiv(shader->program_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[1024];

        glGetProgramInfoLog(shader->program_id, sizeof log, NULL, log);
        fprintf(stderr, "Unable to link shader program named `%s`!\n%s", name, log);

        // TODO: Cleanup shaders on failure
        return NULL;
    }

    // Find the uniforms
    shader->uniform_model = glGetUniformLocation(shader->program_id, "model");
    shader->uniform_view = glGetUniformLocation(shader->program_id, "view");
    shader->uniform_projection = glGetUniformLocation(shader->program_id, "projection");
    shader->uniform_light_position = glGetUniformLocation(shader->program_id, "light_position");
    shader->uniform_light_color = glGetUniformLocation(shader->program_id, "light_color");
    shader->uniform_shine_damper = glGetUniformLocation(shader->program_id, "shine_damper");
    shader->uniform_reflectivity = glGetUniformLocation(shader->program_id, "reflectivity");

    return shader;
}
