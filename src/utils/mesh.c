#include "./mesh.h"
#include "../../include/glad/glad.h"
#include <cglm/cglm.h>
#include <string.h>

void Draw(Mesh* mesh, unsigned int* shader) {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  glUseProgram(*shader);
  for (unsigned int i = 0; i < mesh->texture_count; i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    char number[16];
    char uniform[128];

    if (strcmp(mesh->textures[i].type, "texture_diffuse") == 0)
      sprintf(number, "%u", diffuseNr++);
    else if (strcmp(mesh->textures[i].type, "texture_specular") == 0)
      sprintf(number, "%u", specularNr++);
    else
      sprintf(number, "%u", 1);

    sprintf(uniform, "material.%s%s", mesh->textures[i].type, number);

    glUniform1i(glGetUniformLocation(*shader, uniform), i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  }

  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

static void setupMesh(Mesh* mesh) {
  glGenVertexArrays(1, &mesh->VAO);
  glGenBuffers(1, &mesh->VBO);
  glGenBuffers(1, &mesh->EBO);

  glBindVertexArray(mesh->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}
Mesh Mesh_create(Vertex* vertices, unsigned int vertex_count,
                 unsigned int* indices, unsigned int index_count,
                 Texture* textures, unsigned int texture_count)
{
    Mesh mesh;
    mesh.vertices      = vertices;
    mesh.vertex_count  = vertex_count;
    mesh.indices       = indices;
    mesh.index_count   = index_count;
    mesh.textures      = textures;
    mesh.texture_count = texture_count;
    setupMesh(&mesh);
    return mesh;
}

