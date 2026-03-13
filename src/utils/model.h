#ifndef MODEL_H
#define MODEL_H

#include "./mesh.h"

#define MAX_MESHES 64
#define MAX_TEXTURES_LOADED 128

typedef struct {
    Mesh meshes[MAX_MESHES];
    unsigned int mesh_count;

    Texture textures_loaded[MAX_TEXTURES_LOADED];
    char texture_paths[MAX_TEXTURES_LOADED][256]; // parallel array storing paths for cache lookup
    unsigned int textures_loaded_count;

    char directory[256];
} Model;

Model Model_load(const char* path);
void Model_draw(Model* model, unsigned int* shader);

#endif

