#pragma once

#include "util.hh"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <deque>

using std::deque;

struct Model;
struct Mesh;
struct Material;
struct Texture;

void printModel(Model &model);
bool testAABB(vec3 org, vec3 ray, vec3 wMin, vec3 wMax);

struct Shader {
    u32 id = 0;

    Shader(const string &path, i32 type);
    //~Shader() { glDeleteShader(id); }
};

struct Program {
    string key;
    u32 id = 0;
    string vs, fs, gs;

    Program(u32 id = 0): id(id) {};
    Program(string vs, string fs, string gs = ""): vs(vs), fs(fs), gs(gs) {};

    void load();
    void unload();
    void reload();

    u32 u(string name) { return glGetUniformLocation(id, name.c_str()); };
    u32 a(string name) { return glGetAttribLocation(id, name.c_str()); };
};

enum class TextureType : u32 {
    diffuse = aiTextureType_DIFFUSE,
    specular = aiTextureType_SPECULAR,
    height = aiTextureType_HEIGHT,
    normal = aiTextureType_NORMALS,
};

struct Texture {
    u32 id = 0, width, height, format;
    string path;
    TextureType type;
    bool embedded;

    void read();
    void load();
    void unload();
    void reload();
};

struct Material {
    string name;
    vec3 diffuse, specular, ambient, emissive, transparent;
    f32 shininess, opacity, specularScale = 32;
    bool backfaceCulling;
    vector<u32> textures;
};

struct Mesh {
    u32 indexCount;
    u32 vao, vbo[6];
    u32 material;

    vector<vec3> vertices, normals, tangents, bitangents;
    vector<vec2> texCoords;
    vector<u32> indices;

    Model *model;

    void load();
    void unload();

    void bindTexture(Program &shader);
    void draw(Program &shader);
};

struct Model {
    string key;
    string path;
    vector<Mesh> meshes;
    vector<Material> materials;
    vector<Texture> textures;
    vec3 minCoords, maxCoords, center, size;

    Model(string path = ""): path(path) {};

    void read();
    void readMesh(aiMesh *mesh, Mesh &x);
    void readMaterial(aiMaterial *mat, Material &x);
    void boundingBox(aiMesh *mesh, vec3 &min, vec3 &max);

    void load();
    void unload();
    void reload();
    void draw(Program &shader);
};

struct Camera {
    // rot.x: pitch, rot.y: yaw, rot.z: roll (roll not implemented)
    vec3 pos, rot, front, up, right, world;
    f32 effect1 = 0.0;

    Camera(vec3 pos, vec3 rot): pos(pos), rot(rot) {
        world = vec3(0, 1, 0);
        front = vec3(0, 0, -1);
        up = vec3(0, 1, 0);
        update();
    }

    mat4 lookAt() { return glm::lookAt(pos, pos + front, up); }
    void update();

    void offsetFront(f32 delta) { pos += front * delta; }
    void offsetRight(f32 delta) { pos += right * delta; }
    void offsetUp(f32 delta)    { pos += world * delta; }
    void offsetPitch(f32 delta) { rot.x += delta; update(); }
    void offsetYaw(f32 delta)   { rot.y += delta; update(); }
};

struct Object {
    Model *model;
    Program *shader;

    vec3 origin = vec3(0.0);
    f32 scale = 1.0;
    f32 rotation = 0.0;

    bool visible = true;
    bool selectable = true;
    bool drinkable = false;
};

struct Scene {
    map<string, Program*> shaders;
    map<string, Model*> models;
    vector<Object*> objects;

    void read(const string &path);
    void save(const string &path);
    void load();
    void unload();
};

