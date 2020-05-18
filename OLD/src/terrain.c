#include "terrain.h"
#include "model.h"

#define SIZE 640
#define VERTEX_COUNT 128

struct model *terrain_generate_model(void) {
    float vertices[VERTEX_COUNT * VERTEX_COUNT * 3];
    float normals[VERTEX_COUNT * VERTEX_COUNT * 3];
    float texture_coords[VERTEX_COUNT * VERTEX_COUNT * 2];
    unsigned int faces[6 * VERTEX_COUNT * VERTEX_COUNT]; // TODO: slightly too much

    int vertex_count = 0;
    for (int i = 0; i < VERTEX_COUNT; i++) {
        for (int j = 0; j < VERTEX_COUNT; j++) {
            vertices[vertex_count * 3 + 0] = (float) j / ((float) VERTEX_COUNT - 1) * SIZE;
            vertices[vertex_count * 3 + 1] = 0;
            vertices[vertex_count * 3 + 2] = (float) i / ((float) VERTEX_COUNT - 1) * SIZE;
            normals[vertex_count * 3 + 0] = 0;
            normals[vertex_count * 3 + 1] = 1;
            normals[vertex_count * 3 + 2] = 0;
            texture_coords[vertex_count * 2 + 0] = (float) j / ((float) VERTEX_COUNT - 1);
            texture_coords[vertex_count * 2 + 1] = (float) i / ((float) VERTEX_COUNT - 1);
            vertex_count++;
        }
    }

    int face_count = 0;
    for (int z = 0; z < VERTEX_COUNT-1; z++) {
        for (int x = 0; x < VERTEX_COUNT-1; x++) {
            int top_left = z * VERTEX_COUNT + x;
            int top_right = top_left + 1;
            int bottom_left = (z + 1) * VERTEX_COUNT + x;
            int bottom_right = bottom_left + 1;

            faces[face_count * 3 + 0] = top_left;
            faces[face_count * 3 + 1] = bottom_left;
            faces[face_count * 3 + 2] = top_right;
            face_count++;

            faces[face_count * 3 + 0] = top_right;
            faces[face_count * 3 + 1] = bottom_left;
            faces[face_count * 3 + 2] = bottom_right;
            face_count++;
        }
    }

    return model_create_from_raw(vertices, vertex_count, faces, face_count, texture_coords, normals);
}
