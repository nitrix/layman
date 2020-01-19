#include "obj.h"
#include "toolkit.h"
#include "vector.h"
#include "model.h"

struct model *obj_load_model(const char *filepath) {
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
        final_faces[i*3+1] = face.z - 1;
        final_faces[i*3+2] = face.y - 1;
    }

    tk_collection_fini(&vertices);
    tk_collection_fini(&normals);
    tk_collection_fini(&textures);
    tk_collection_fini(&faces);

    // TODO: Error handling
    return model_create_from_raw(final_vertices, final_vertex_count, final_faces, final_face_count, final_textures, final_normals);
}
