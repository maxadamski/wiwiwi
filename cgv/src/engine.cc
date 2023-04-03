#include "engine.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "json.hpp"

using namespace std;
using namespace glm;
using nlohmann::json;

// -----------------------------------------------

void Camera::update() {
    front = vec3(
            cos(rot.x) * sin(rot.y),
            sin(rot.x),
            cos(rot.x) * cos(rot.y)
            );
    right = cross(front, world);
    up = cross(right, front);
}

// -----------------------------------------------

const vector<TextureType> textureTypes = {
    TextureType::diffuse,
    TextureType::specular,
    TextureType::height,
    TextureType::normal
};

void Model::readMesh(aiMesh *mesh, Mesh &x) {
    u32 vertexCount = mesh->mNumVertices, faceCount = mesh->mNumFaces;
    x.material = mesh->mMaterialIndex;
    x.model = this;

    x.vertices.resize(vertexCount);
    for (u32 i = 0; i < vertexCount; i++) {
        aiVector3D v = mesh->mVertices[i];
        x.vertices[i] = vec3(v.x, v.y, v.z);
    }

    for (u32 i = 0; i < faceCount; i++) {
        aiFace face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; j++) {
            x.indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->HasNormals()) {
        x.normals.resize(vertexCount);
        for (u32 i = 0; i < vertexCount; i++) {
            aiVector3D n = mesh->mNormals[i];
            x.normals[i] = vec3(n.x, n.y, n.z);
        }
    }

    if (mesh->HasTangentsAndBitangents()) {
        x.tangents.resize(vertexCount);
        x.bitangents.resize(vertexCount);
        for (u32 i = 0; i < vertexCount; i++) {
            aiVector3D t = mesh->mTangents[i];
            aiVector3D b = mesh->mBitangents[i];
            x.tangents[i] = vec3(t.x, t.y, t.z);
            x.bitangents[i] = vec3(b.x, b.y, b.z);
        }
    }

    // Assume 1 set of UV coords; AssImp supports 8 UV sets.
    if (mesh->HasTextureCoords(0)) {
        x.texCoords.resize(vertexCount);
        for (u32 i = 0; i < vertexCount; i++) {
            aiVector3D t = mesh->mTextureCoords[0][i];
            x.texCoords[i] = vec2(t.x, t.y);
        }
    }

    // compute bounding box
    boundingBox(mesh, minCoords, maxCoords);
}

void Model::readMaterial(aiMaterial *mat, Material &x) {
    aiColor3D c(0, 0, 0);
    aiString s;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, c);
    x.diffuse = vec3(c.r, c.g, c.b);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, c);
    x.specular = vec3(c.r, c.g, c.b);
    mat->Get(AI_MATKEY_COLOR_AMBIENT, c);
    x.ambient = vec3(c.r, c.g, c.b);
    mat->Get(AI_MATKEY_COLOR_EMISSIVE, c);
    x.emissive = vec3(c.r, c.g, c.b);
    mat->Get(AI_MATKEY_COLOR_TRANSPARENT, c);
    x.transparent = vec3(c.r, c.g, c.b);

    mat->Get(AI_MATKEY_SHININESS, x.shininess);
    mat->Get(AI_MATKEY_SHININESS_STRENGTH, x.specularScale);
    mat->Get(AI_MATKEY_OPACITY, x.opacity);
    mat->Get(AI_MATKEY_TWOSIDED, x.backfaceCulling);
    mat->Get(AI_MATKEY_NAME, s);
    x.name = s.data;

    string directory = path.substr(0, path.find_last_of('/'));
    for (TextureType type : textureTypes) {
        aiTextureType aiType = static_cast<aiTextureType>(type);
        u32 count = mat->GetTextureCount(aiType);

        for (u32 i = 0; i < count; i++) {
            aiString name;
            mat->GetTexture(aiType, i, &name);
            string path = directory + "/" + name.data;

            // TODO: do not load duplicate textures
            Texture tex;
            tex.type = type;
            tex.path = path;
            textures.push_back(tex);
            x.textures.push_back(textures.size() - 1);

            dprintf("-- material %s texture %s\n", x.name.c_str(), tex.path.c_str());
        }
    }
}

void Model::boundingBox(aiMesh *mesh, vec3 &minCoords, vec3 &maxCoords) {
    for (u32 i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D v = mesh->mVertices[i];
        if (v.x < minCoords.x) minCoords.x = v.x;
        if (v.y < minCoords.y) minCoords.y = v.y;
        if (v.z < minCoords.z) minCoords.z = v.z;
        if (v.x > maxCoords.x) maxCoords.x = v.x;
        if (v.y > maxCoords.y) maxCoords.y = v.y;
        if (v.z > maxCoords.z) maxCoords.z = v.z;
    }
}

void Model::read() {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs
            | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        eprintf("error: assimp error '%s'\n", importer.GetErrorString());
        return;
    }

    u32 materialCount = scene->mNumMaterials, meshCount = scene->mNumMeshes, textureCount = scene->mNumTextures;
    dprintf("info: reading model '%s' (%u meshes, %u materials, %u textures)\n",
            path.c_str(), meshCount, materialCount, textureCount);

    materials.resize(materialCount);
    for (u32 i = 0; i < materialCount; i++) {
        readMaterial(scene->mMaterials[i], materials[i]);
    }

    // initialize bounding box
    aiVector3D v0 = scene->mMeshes[0]->mVertices[0];
    minCoords = maxCoords = vec3(v0.x, v0.y, v0.z);

    meshes.resize(meshCount);
    for (u32 i = 0; i < meshCount; i++) {
        readMesh(scene->mMeshes[i], meshes[i]);
    }

    size = maxCoords - minCoords;
    center = (minCoords + maxCoords) / 2.0f;
}

void Model::load() {
    read();
    for (Texture &x : textures) x.load();
    for (Mesh &x : meshes) x.load();
}

void Model::unload() {
    for (Texture &x : textures) x.unload();
    for (Mesh &x : meshes) x.unload();
}

void Model::draw(Program &shader) {
    for (Mesh &x : meshes) x.draw(shader);
}

void printModel(Model &model) {
    dprintf("------------\n");

    dprintf("model '%s':\n", model.path.c_str());

    dprintf("  textures:\n");
    for (u32 i = 0; i < model.textures.size(); i++) {
        Texture &tex = model.textures[i];
        dprintf("  - '%s'\n", tex.path.c_str());
    }

    dprintf("  materials:\n");
    for (u32 i = 0; i < model.materials.size(); i++) {
        Material &mat = model.materials[i];
        dprintf("  - '%s':\n", mat.name.c_str());
        dprintf("      diffuse:  (%.1f, %.1f, %.1f)\n", mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
        dprintf("      specular: (%.1f, %.1f, %.1f)\n", mat.specular.x, mat.specular.y, mat.specular.z);
        dprintf("      ambient:  (%.1f, %.1f, %.1f)\n", mat.ambient.x, mat.ambient.y, mat.ambient.z);
        dprintf("      emissive: (%.1f, %.1f, %.1f)\n", mat.emissive.x, mat.emissive.y, mat.emissive.z);
        dprintf("      transp:   (%.1f, %.1f, %.1f)\n", mat.transparent.x, mat.transparent.y, mat.transparent.z);
        for (u32 j = 0; j < mat.textures.size(); j++) {
            Texture &tex = model.textures[mat.textures[j]];
            dprintf("      texture: %s\n", tex.path.c_str());
        }
    }

    dprintf("  meshes:\n");
    for (u32 i = 0; i < model.meshes.size(); i++) {
        Mesh &mesh = model.meshes[i];
        Material &mat = model.materials[mesh.material];
        dprintf("  - %u vao=%u:\n", i, mesh.vao);
        dprintf("      material: '%s'\n", mat.name.c_str());
        dprintf("      indices:    %lu\n", mesh.indices.size());
        dprintf("      vertices:   %lu\n", mesh.vertices.size());
        dprintf("      normals:    %lu\n", mesh.normals.size());
        dprintf("      tangents:   %lu\n", mesh.tangents.size());
        dprintf("      bitangents: %lu\n", mesh.bitangents.size());
    }
}

// -----------------------------------------------

void Mesh::load() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(6, vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vec(vertices), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vec(normals), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vec(texCoords), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vec(tangents), tangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vec(bitangents), bitangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_vec(indices), indices.data(), GL_STATIC_DRAW);
    indexCount = indices.size();

    glBindVertexArray(0);
}

void Mesh::unload() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(6, vbo);
}

void Mesh::bindTexture(Program &shader) {
    Material &mat = model->materials[material];

    u32 diffuseId = 0, normalId = 0, heightId = 0, specularId = 0;
    for (u32 i = 0; i < mat.textures.size(); i++) {
        Texture &tex = model->textures[mat.textures[i]];

        string var;
        switch (tex.type) {
            case TextureType::diffuse: var = "DiffuseMap" + to_string(diffuseId++); break;
            case TextureType::normal: var = "NormalMap" + to_string(normalId++); break;
            case TextureType::height: var = "HeightMap" + to_string(heightId++); break;
            case TextureType::specular: var = "SpecularMap" + to_string(specularId++); break;
        }

        //dprintf("info: -- %u tex(vao=%u, id=%u, name=%s) -> %s\n", i, vao, tex.id, tex.path.c_str(), var.c_str());
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glUniform1i(shader.u(var.c_str()), i);
    }

    i32 textured = mat.textures.size() > 0 ? 1 : 0;
    glUniform1i(shader.u("isTextured"), textured);
    glUniform3fv(shader.u("mat.ambient"), 1, value_ptr(mat.ambient));
    glUniform3fv(shader.u("mat.diffuse"), 1, value_ptr(mat.diffuse));
    glUniform3fv(shader.u("mat.specular"), 1, value_ptr(mat.specular));
    glUniform1f(shader.u("mat.shine"), mat.specularScale);
}

void Mesh::draw(Program &shader) {
    bindTexture(shader);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

// -----------------------------------------------

void Texture::read() {
}

void Texture::load() {
    if (!ilLoadImage(path.c_str())) {
        eprintf("error: could not load image '%s'\n", path.c_str());
        return;
    }

    format = ilGetInteger(IL_IMAGE_FORMAT);
    height = ilGetInteger(IL_IMAGE_HEIGHT);
    width = ilGetInteger(IL_IMAGE_WIDTH);
    u8 *data = ilGetData();

    glGenTextures(1, &id);
    //dprintf("info: loading texture '%s' at id %u\n", path.c_str(), id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::unload() {
    glDeleteTextures(1, &id);
}

// -----------------------------------------------

i32 checkShaderStatus(u32 id) {
    i32 logLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        vector<char> message(logLength + 1);
        glGetShaderInfoLog(id, logLength, NULL, &message[0]);
        fprintf(stderr, "%s\n", message.data());
    }

    i32 result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    return result;
}

Shader::Shader(const string &path, i32 type) {
    string code;
    if (!readFile(path, code)) return;
    const char *code_ptr = code.c_str();

    eprintf("info: compiling shader '%s'\n", path.c_str());
    id = glCreateShader(type);
    glShaderSource(id, 1, &code_ptr, NULL);
    glCompileShader(id);
    checkShaderStatus(id);
}

void Program::load() {
    vector<Shader> shaders;
    shaders.push_back(Shader(vs, GL_VERTEX_SHADER));
    shaders.push_back(Shader(fs, GL_FRAGMENT_SHADER));
    if (!gs.empty()) shaders.push_back(Shader(gs, GL_GEOMETRY_SHADER));

    eprintf("info: linking shader program\n");
    id = glCreateProgram();

    for (Shader &shader : shaders) glAttachShader(id, shader.id);
    glLinkProgram(id);
    checkShaderStatus(id);
    for (Shader &shader : shaders) glDetachShader(id, shader.id);
}

void Program::unload() {
    glDeleteProgram(id);
}

void Program::reload() {
    unload();
    load();
}

// -----------------------------------------------

string encodeFloat(f32 x) {
    std::ostringstream s;
    s << std::fixed << std::setprecision(2) << x;
    return s.str();
}

f32 decodeFloat(json x) {
    return std::stof(x.get<string>());
}

vector<string> encodeVec3(vec3 x) {
    //return vector<i32>({ (i32) (x.x*PREC), (i32) (x.y * PREC), (i32) (x.z * PREC) });
    return {encodeFloat(x.x), encodeFloat(x.y), encodeFloat(x.z)};
}

vec3 decodeVec3(json x) {
    //return vec3((f32) x[0] / PREC, (f32) x[1] / PREC, (f32) x[2] / PREC);
    return vec3(decodeFloat(x[0]), decodeFloat(x[1]), decodeFloat(x[2]));
}


void Scene::save(const string &path) {
    dprintf("saving scene to %s\n", path.c_str());

    json j;

    for (auto& [k, v] : shaders) {
        j["shaders"][k]["vs"] = v->vs;
        j["shaders"][k]["fs"] = v->fs;
    }

    for (auto& [k, v] : models) {
        j["models"][k] = v->path;
    }

    for (auto& x : objects) {
        json o;
        o["model"] = x->model->key;
        o["shader"] = x->shader->key;
        o["origin"] = encodeVec3(x->origin);
        o["scale"] = encodeFloat(x->scale);
        o["rotation"] = encodeFloat(x->rotation);
        o["selectable"] = x->selectable;
        o["drinkable"] = x->drinkable;
        j["objects"].push_back(o);
    }

    std::ofstream file(path);
    file.precision(4);
    file << std::setw(4) << j << endl;
}

void Scene::read(const string &path) {
    dprintf("loading scene from %s\n", path.c_str());

    std::ifstream file(path);
    json j;
    file >> j;

    for (auto& [k, v] : j["shaders"].items()) {
        Program *x = new Program(v["vs"], v["fs"]);
        x->key = k;
        shaders.insert({k, x});
    }

    for (auto& [k, v] : j["models"].items()) {
        Model *x = new Model(v);
        x->key = k;
        models.insert({k, x});
    }

    for (auto& v : j["objects"]) {
        Object *x = new Object;

        x->shader = shaders[v["shader"]];
        x->model = models[v["model"]];
        x->origin = decodeVec3(v["origin"]);

        if (v.count("rotation") == 1) x->rotation = decodeFloat(v["rotation"]);
        if (v.count("scale") == 1) x->scale = decodeFloat(v["scale"]);

        if (v.count("selectable") == 1) x->selectable = v["selectable"].get<bool>();
        if (v.count("drinkable") == 1) x->drinkable = v["drinkable"].get<bool>();

        objects.push_back(x);
    }
}

void Scene::load() {
    for (auto& [k, v] : shaders) v->load();
    for (auto& [k, v] : models) v->load();
}

void Scene::unload() {
    for (auto& [k, v] : models) {
        v->unload();
        delete v;
    }

    for (auto& [k, v] : shaders) {
        v->unload();
        delete v;
    }
}

// -----------------------------------------------

bool testAABB(vec3 org, vec3 ray, vec3 wMin, vec3 wMax) {
    using std::max;
    using std::min;

    ray = 1.0f / ray;
    f32 t1 = (wMin.x - org.x) * ray.x;
    f32 t2 = (wMax.x - org.x) * ray.x;
    f32 t3 = (wMin.y - org.y) * ray.y;
    f32 t4 = (wMax.y - org.y) * ray.y;
    f32 t5 = (wMin.z - org.z) * ray.z;
    f32 t6 = (wMax.z - org.z) * ray.z;

    f32 tMin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    f32 tMax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us, or
    // if tmin > tmax, ray doesn't intersect AABB
    //f32 minDistance;
    if (tMax < 0 || tMin > tMax) {
        //minDistance = tMax;
        return false;
    } else {
        //minDistance = tMin;
        return true;
    }
}

