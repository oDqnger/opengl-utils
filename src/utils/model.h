#ifndef MODEL_H
#define MODEL_H
#include "./mesh.h"

typedef struct {
    Mesh* meshes;
    unsigned int mesh_count;
    unsigned int mesh_capacity;
    Texture* textures_loaded;
    char (*texture_paths)[256];
    unsigned int textures_loaded_count;
    unsigned int textures_loaded_capacity;
    char directory[256];
} Model;

Model Model_load(const char* path);
void  Model_draw(Model* model, unsigned int* shader);
void  Model_free(Model* model);

#endif
