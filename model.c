#include "model.h"
#include "../../include/glad/glad.h"
#include "stb_image.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Forward declarations of internal helpers                           */
/* ------------------------------------------------------------------ */
static void        processNode(Model* model, struct aiNode* node, const struct aiScene* scene);
static Mesh        processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene);
static Texture     loadTexture(Model* model, const char* path);
static void        loadMaterialTextures(Model* model,
                                        struct aiMaterial* mat,
                                        enum aiTextureType type,
                                        const char* typeName,
                                        Texture* out_textures,
                                        unsigned int* out_count);

/* ------------------------------------------------------------------ */
/* TextureFromFile – load an image and upload it to the GPU           */
/* ------------------------------------------------------------------ */
static unsigned int TextureFromFile(const char* filename, const char* directory)
{
    /* Build the full path: directory + '/' + filename */
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(fullPath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if      (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        printf("Texture failed to load: %s\n", fullPath);
        stbi_image_free(data);
    }

    return textureID;
}

/* ------------------------------------------------------------------ */
/* Model_load – entry point                                           */
/* ------------------------------------------------------------------ */
Model Model_load(const char* path)
{
    Model model;
    memset(&model, 0, sizeof(Model));
    /* Ask Assimp to load the file.
       aiProcess_Triangulate  – convert all faces to triangles.
       aiProcess_FlipUVs      – flip UV y-axis to match OpenGL convention. */
    const struct aiScene* scene = aiImportFile(path,
                                               aiProcess_Triangulate |
                                               aiProcess_FlipUVs);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        return model;
    }

    /* Store the directory so we can locate textures relative to the model */
    const char* lastSlash = strrchr(path, '/');
    if (lastSlash) {
        size_t len = (size_t)(lastSlash - path);
        strncpy(model.directory, path, len);
        model.directory[len] = '\0';
    } else {
        strcpy(model.directory, ".");
    }

    /* Recursively walk the node tree */
    processNode(&model, scene->mRootNode, scene);

    aiReleaseImport(scene);
    return model;
}

/* ------------------------------------------------------------------ */
/* Model_draw – draw every mesh in the model                          */
/* ------------------------------------------------------------------ */
void Model_draw(Model* model, unsigned int* shader)
{
    for (unsigned int i = 0; i < model->mesh_count; i++) {
        Draw(&model->meshes[i], shader);
    }
}

/* ------------------------------------------------------------------ */
/* processNode – recursively process each node in the scene tree      */
/* ------------------------------------------------------------------ */
static void processNode(Model* model, struct aiNode* node, const struct aiScene* scene)
{
    /* Process every mesh referenced by this node */
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        if (model->mesh_count < MAX_MESHES) {
            model->meshes[model->mesh_count++] = processMesh(model, mesh, scene);
        }
    }

    /* Recurse into children */
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(model, node->mChildren[i], scene);
    }
}

/* ------------------------------------------------------------------ */
/* processMesh – convert one aiMesh into our Mesh struct              */
/* ------------------------------------------------------------------ */
static Mesh processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene)
{
    /* --- Allocate output arrays --- */
    Vertex*       vertices = malloc(mesh->mNumVertices * sizeof(Vertex));
    unsigned int* indices  = NULL;
    Texture*      textures = NULL;
    unsigned int  index_count   = 0;
    unsigned int  texture_count = 0;

    /* --- Vertices --- */
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;

        /* Position */
        v.Position[0] = mesh->mVertices[i].x;
        v.Position[1] = mesh->mVertices[i].y;
        v.Position[2] = mesh->mVertices[i].z;

        /* Normal */
        if (mesh->mNormals) {
            v.Normal[0] = mesh->mNormals[i].x;
            v.Normal[1] = mesh->mNormals[i].y;
            v.Normal[2] = mesh->mNormals[i].z;
        } else {
            v.Normal[0] = v.Normal[1] = v.Normal[2] = 0.0f;
        }

        /* Texture coordinates (only the first set, if present) */
        if (mesh->mTextureCoords[0]) {
            v.TexCoords[0] = mesh->mTextureCoords[0][i].x;
            v.TexCoords[1] = mesh->mTextureCoords[0][i].y;
        } else {
            v.TexCoords[0] = v.TexCoords[1] = 0.0f;
        }

        vertices[i] = v;
    }

    /* --- Indices ---
       Each face is a triangle (thanks to aiProcess_Triangulate). */
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        index_count += mesh->mFaces[i].mNumIndices;
    }
    indices = malloc(index_count * sizeof(unsigned int));
    unsigned int idx = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
            indices[idx++] = mesh->mFaces[i].mIndices[j];
        }
    }

    /* --- Textures --- */
    /* Temporary buffers (we'll merge diffuse + specular) */
    Texture diffuse_textures[32];
    Texture specular_textures[32];
    unsigned int diffuse_count  = 0;
    unsigned int specular_count = 0;

    if (mesh->mMaterialIndex >= 0) {
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        loadMaterialTextures(model, material, aiTextureType_DIFFUSE,
                             "texture_diffuse", diffuse_textures, &diffuse_count);
        loadMaterialTextures(model, material, aiTextureType_SPECULAR,
                             "texture_specular", specular_textures, &specular_count);
    }

    texture_count = diffuse_count + specular_count;
    textures = malloc(texture_count * sizeof(Texture));
    memcpy(textures,                        diffuse_textures,  diffuse_count  * sizeof(Texture));
    memcpy(textures + diffuse_count,        specular_textures, specular_count * sizeof(Texture));

    /* --- Build our Mesh --- */
    Mesh result = Mesh_create(vertices, indices, textures);
    result.vertex_count  = mesh->mNumVertices;
    result.index_count   = index_count;
    result.texture_count = texture_count;

    return result;
}

/* ------------------------------------------------------------------ */
/* loadMaterialTextures – load all textures of one type from material */
/* ------------------------------------------------------------------ */
static void loadMaterialTextures(Model* model,
                                 struct aiMaterial* mat,
                                 enum aiTextureType type,
                                 const char* typeName,
                                 Texture* out_textures,
                                 unsigned int* out_count)
{
    *out_count = 0;
    unsigned int count = aiGetMaterialTextureCount(mat, type);

    for (unsigned int i = 0; i < count; i++) {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

        /* Check the texture cache – skip if already loaded */
        int already_loaded = 0;
        for (unsigned int j = 0; j < model->textures_loaded_count; j++) {
            if (strcmp(model->texture_paths[j], str.data) == 0) {
                out_textures[(*out_count)++] = model->textures_loaded[j];
                already_loaded = 1;
                break;
            }
        }

        if (!already_loaded) {
            Texture texture;
            texture.id   = TextureFromFile(str.data, model->directory);
            texture.type = (char*)typeName; /* points to a string literal – safe */

            out_textures[(*out_count)++] = texture;

            /* Add to cache */
            if (model->textures_loaded_count < MAX_TEXTURES_LOADED) {
                model->textures_loaded[model->textures_loaded_count] = texture;
                strncpy(model->texture_paths[model->textures_loaded_count],
                        str.data, 255);
                model->textures_loaded_count++;
            }
        }
    }
}
