#ifndef MESH_H
#define MESH_H

#include <cglm/cglm.h>

typedef struct {
  vec3 Position;
  vec3 Normal;
  vec2 TexCoords;
} Vertex;

typedef struct {
  unsigned int id;
  char* type;
} Texture;

typedef struct {
  Vertex* vertices;
  Texture* textures;
  unsigned int* indices;

  unsigned int vertex_count;
  unsigned int index_count;
  unsigned int texture_count;

  unsigned int VAO, VBO, EBO;
} Mesh;

Mesh Mesh_create(Vertex* vertices, unsigned int vertex_count,
                 unsigned int* indices, unsigned int index_count,
                 Texture* textures, unsigned int texture_count);

void Draw(Mesh* mesh, unsigned int* Shader);

#endif
