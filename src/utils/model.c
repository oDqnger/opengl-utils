#include "model.h"
#include "../../include/glad/glad.h"
#include "stb_image.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void processNode(Model* model, struct aiNode* node, const struct aiScene* scene);
static Mesh processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene);
static void loadMaterialTextures(Model* model,
                                 struct aiMaterial* mat,
                                 enum aiTextureType type,
                                 const char* typeName,
                                 const struct aiScene* scene,
                                 Texture* out_textures,
                                 unsigned int* out_count);

/* ------------------------------------------------------------------ */
/* TextureFromFile                                                     */
/* ------------------------------------------------------------------ */
static unsigned int TextureFromFile(const char* filename, const char* directory)
{
    char cleanFilename[512];
    strncpy(cleanFilename, filename, sizeof(cleanFilename) - 1);
    cleanFilename[sizeof(cleanFilename) - 1] = '\0';
    for (char* p = cleanFilename; *p; p++)
        if (*p == '\\') *p = '/';

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, cleanFilename);

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
/* TextureFromEmbedded                                                 */
/* ------------------------------------------------------------------ */
static unsigned int TextureFromEmbedded(const struct aiScene* scene, const char* hint)
{
    const struct aiTexture* tex = aiGetEmbeddedTexture(scene, hint);

    if (!tex) {
        printf("Could not find embedded texture: %s\n", hint);
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = NULL;
    int need_free_data = 0;

    if (tex->mHeight == 0) {
        stbi_set_flip_vertically_on_load(1);
        data = stbi_load_from_memory(
            (unsigned char*)tex->pcData,
            tex->mWidth,
            &width, &height, &nrChannels, 0
        );
    } else {
        width      = tex->mWidth;
        height     = tex->mHeight;
        nrChannels = 4;
        data       = malloc(width * height * 4);
        need_free_data = 1;
        for (int i = 0; i < width * height; i++) {
            data[i*4+0] = tex->pcData[i].r;
            data[i*4+1] = tex->pcData[i].g;
            data[i*4+2] = tex->pcData[i].b;
            data[i*4+3] = tex->pcData[i].a;
        }
    }

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

        if (need_free_data)
            free(data);
        else
            stbi_image_free(data);
    } else {
        printf("Failed to decode embedded texture: %s\n", hint);
    }

    return textureID;
}

/* ------------------------------------------------------------------ */
/* Model_load                                                          */
/* ------------------------------------------------------------------ */
Model Model_load(const char* path)
{
    Model model;
    memset(&model, 0, sizeof(Model));

    model.mesh_capacity = 256;
    model.meshes        = malloc(model.mesh_capacity * sizeof(Mesh));

    model.textures_loaded_capacity = 256;
    model.textures_loaded          = malloc(model.textures_loaded_capacity * sizeof(Texture));
    model.texture_paths            = malloc(model.textures_loaded_capacity * 256);

    const struct aiScene* scene = aiImportFile(path,
             aiProcess_Triangulate |
             aiProcess_FlipUVs     |
             aiProcess_EmbedTextures);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        return model;
    }

    const char* lastSlash = strrchr(path, '/');
    if (lastSlash) {
        size_t len = (size_t)(lastSlash - path);
        strncpy(model.directory, path, len);
        model.directory[len] = '\0';
    } else {
        strcpy(model.directory, ".");
    }

    processNode(&model, scene->mRootNode, scene);

  printf("Total meshes loaded: %u\n", model.mesh_count);
  printf("Total nodes in scene: %u\n", scene->mNumMeshes);
  printf("Total textures loaded: %u\n", model.textures_loaded_count);

    aiReleaseImport(scene);
    return model;
}

/* ------------------------------------------------------------------ */
/* Model_draw                                                          */
/* ------------------------------------------------------------------ */
void Model_draw(Model* model, unsigned int* shader)
{
    for (unsigned int i = 0; i < model->mesh_count; i++) {
        Draw(&model->meshes[i], shader);
    }
}

/* ------------------------------------------------------------------ */
/* Model_free                                                          */
/* ------------------------------------------------------------------ */
void Model_free(Model* model)
{
    free(model->meshes);
    free(model->textures_loaded);
    free(model->texture_paths);
}

/* ------------------------------------------------------------------ */
/* processNode                                                         */
/* ------------------------------------------------------------------ */
static void processNode(Model* model, struct aiNode* node, const struct aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        if (model->mesh_count >= model->mesh_capacity) {
            model->mesh_capacity *= 2;
            model->meshes = realloc(model->meshes, model->mesh_capacity * sizeof(Mesh));
        }

        model->meshes[model->mesh_count++] = processMesh(model, mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(model, node->mChildren[i], scene);
    }
}

/* ------------------------------------------------------------------ */
/* processMesh                                                         */
/* ------------------------------------------------------------------ */
static Mesh processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene)
{
    Vertex*       vertices = malloc(mesh->mNumVertices * sizeof(Vertex));
    unsigned int* indices  = NULL;
    Texture*      textures = NULL;
    unsigned int  index_count   = 0;
    unsigned int  texture_count = 0;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;

        v.Position[0] = mesh->mVertices[i].x;
        v.Position[1] = mesh->mVertices[i].y;
        v.Position[2] = mesh->mVertices[i].z;

        if (mesh->mNormals) {
            v.Normal[0] = mesh->mNormals[i].x;
            v.Normal[1] = mesh->mNormals[i].y;
            v.Normal[2] = mesh->mNormals[i].z;
        } else {
            v.Normal[0] = v.Normal[1] = v.Normal[2] = 0.0f;
        }

        if (mesh->mTextureCoords[0]) {
            v.TexCoords[0] = mesh->mTextureCoords[0][i].x;
            v.TexCoords[1] = mesh->mTextureCoords[0][i].y;
        } else {
            v.TexCoords[0] = v.TexCoords[1] = 0.0f;
        }

        vertices[i] = v;
    }

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

    Texture diffuse_textures[32];
    Texture specular_textures[32];
    unsigned int diffuse_count  = 0;
    unsigned int specular_count = 0;

    if (mesh->mMaterialIndex >= 0) {
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        loadMaterialTextures(model, material, aiTextureType_DIFFUSE,
                             "texture_diffuse", scene, diffuse_textures, &diffuse_count);
        loadMaterialTextures(model, material, aiTextureType_SPECULAR,
                             "texture_specular", scene, specular_textures, &specular_count);
    }

    texture_count = diffuse_count + specular_count;
    textures = malloc(texture_count * sizeof(Texture));
    memcpy(textures,                 diffuse_textures,  diffuse_count  * sizeof(Texture));
    memcpy(textures + diffuse_count, specular_textures, specular_count * sizeof(Texture));

    Mesh result = Mesh_create(vertices, mesh->mNumVertices,
                              indices,  index_count,
                              textures, texture_count);
    return result;
}

/* ------------------------------------------------------------------ */
/* loadMaterialTextures                                                */
/* ------------------------------------------------------------------ */
static void loadMaterialTextures(Model* model,
                                 struct aiMaterial* mat,
                                 enum aiTextureType type,
                                 const char* typeName,
                                 const struct aiScene* scene,
                                 Texture* out_textures,
                                 unsigned int* out_count)
{
    *out_count = 0;
    unsigned int count = aiGetMaterialTextureCount(mat, type);

    for (unsigned int i = 0; i < count; i++) {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

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

            const struct aiTexture* embedded = aiGetEmbeddedTexture(scene, str.data);
            if (embedded) {
                texture.id = TextureFromEmbedded(scene, str.data);
            } else {
                char cleanPath[256];
                strncpy(cleanPath, str.data, 255);
                for (char* p = cleanPath; *p; p++)
                    if (*p == '\\') *p = '/';
                texture.id = TextureFromFile(cleanPath, model->directory);
            }

            texture.type = (char*)typeName;
            out_textures[(*out_count)++] = texture;

            if (model->textures_loaded_count >= model->textures_loaded_capacity) {
                model->textures_loaded_capacity *= 2;
                model->textures_loaded = realloc(model->textures_loaded,
                                         model->textures_loaded_capacity * sizeof(Texture));
                model->texture_paths   = realloc(model->texture_paths,
                                         model->textures_loaded_capacity * 256);
            }

            model->textures_loaded[model->textures_loaded_count] = texture;
            strncpy(model->texture_paths[model->textures_loaded_count], str.data, 255);
            model->textures_loaded_count++;
        }
    }
}
