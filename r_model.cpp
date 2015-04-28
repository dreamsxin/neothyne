#include <assert.h>

#include "r_model.h"
#include "r_texture.h"

#include "engine.h"

#include "u_misc.h"
#include "u_file.h"

namespace r {

///! Model Material Loading (Also used for the world)
material::material()
    : permute(0)
    , diffuse(nullptr)
    , normal(nullptr)
    , spec(nullptr)
    , displacement(nullptr)
    , specParams(false)
    , specPower(0.0f)
    , specIntensity(0.0f)
    , dispScale(0.0f)
    , dispBias(0.0f)
{
}

bool material::load(u::map<u::string, texture2D*> &textures, const u::string &materialName, const u::string &basePath) {
    u::string diffuseName;
    u::string specName;
    u::string normalName;
    u::string displacementName;

    // Read the material
    auto fp = u::fopen(neoGamePath() + materialName + ".cfg", "r");
    if (!fp) {
        u::print("Failed to load material: %s\n", materialName);
        return false;
    }

    bool specParams = false;
    float specIntensity = 0;
    float specPower = 0;
    float dispScale = 0;
    float dispBias = 0;

    while (auto line = u::getline(fp)) {
        auto get = *line;
        auto split = u::split(get);
        if (split.size() < 2)
            continue;
        auto key = split[0];
        auto value = split[1];
        if (key == "diffuse")
            diffuseName = basePath + value;
        else if (key == "normal")
            normalName = "<normal>" + basePath + value;
        else if (key == "displacement")
            displacementName = "<grey>" + basePath + value;
        else if (key == "spec")
            specName = basePath + value;
        else if (key == "specparams") {
            specPower = u::atof(value);
            if (split.size() > 2)
                specIntensity = u::atof(split[2]);
            specParams = true;
        } else if (key == "parallax") {
            dispScale = u::atof(value);
            if (split.size() > 2)
                dispBias = u::atof(split[2]);
        }
    }

    auto loadTexture = [&](const u::string &ident, texture2D **store) {
        if (ident.empty())
            return;
        if (textures.find(ident) != textures.end()) {
            *store = textures[ident];
        } else {
            u::unique_ptr<texture2D> tex(new texture2D);
            if (tex->load(ident)) {
                auto release = tex.release();
                textures[ident] = release;
                *store = release;
            } else {
                *store = nullptr;
            }
        }
    };

    loadTexture(diffuseName, &diffuse);
    loadTexture(normalName, &normal);
    loadTexture(specName, &spec);
    loadTexture(displacementName, &displacement);

    // If there is a specular map, silently drop the specular parameters
    if (spec && specParams)
        specParams = false;

    this->specParams = specParams;
    this->specIntensity = specIntensity / 2.0f;
    this->specPower = m::log2(specPower) / 8.0f;
    this->dispScale = dispScale;
    this->dispBias = dispBias;

    return true;
}

bool material::upload() {
    if (diffuse && !diffuse->upload())
        return false;
    if (normal && !normal->upload())
        return false;
    if (spec && !spec->upload())
        return false;
    if (displacement && !displacement->upload())
        return false;
    return true;
}

///! Model Loading and Rendering
model::model()
    : m_indices(0)
{
}

bool model::load(u::map<u::string, texture2D*> &textures, const u::string &file) {
    // Open the model file and look for a model configuration
    u::file fp = u::fopen(neoGamePath() + file + ".cfg", "r");
    if (!fp)
        return false;

    u::string name;
    while (auto getline = u::getline(fp)) {
        auto split = u::split(*getline);
        if (split.size() < 2)
            continue;
        if (split[0] == "model" && name.empty())
            name = u::move(split[1]);
        else if (split[0] == "scale") {
            scale.x = u::atof(split[1]);
            if (split.size() > 2) scale.y = u::atof(split[2]);
            if (split.size() > 3) scale.z = u::atof(split[3]);
        } else if (split[0] == "rotate") {
            rotate.x = u::atof(split[1]);
            if (split.size() > 2) rotate.y = u::atof(split[2]);
            if (split.size() > 3) rotate.z = u::atof(split[3]);
        }
    }

    // Now use that to load the mesh
    if (!m_model.load("models/" + name))
        return false;
    // Load the model file all over again, except we treat it as a material file
    if (!mat.load(textures, file, "models/"))
        return false;
    return true;
}

bool model::upload() {
    geom::upload();

    const auto &indices = m_model.indices();
    m_indices = indices.size();

    gl::BindVertexArray(vao);
    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);

    if (m_model.animated()) {
        const auto &vertices = m_model.animVertices();
        mesh::animVertex *vert = nullptr;
        gl::BufferData(GL_ARRAY_BUFFER, sizeof(mesh::animVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        gl::VertexAttribPointer(0, 3, GL_FLOAT,         GL_FALSE, sizeof(mesh::animVertex), &vert->position); // vertex
        gl::VertexAttribPointer(1, 3, GL_FLOAT,         GL_FALSE, sizeof(mesh::animVertex), &vert->normal); // normals
        gl::VertexAttribPointer(2, 2, GL_FLOAT,         GL_FALSE, sizeof(mesh::animVertex), &vert->coordinate); // texCoord
        gl::VertexAttribPointer(3, 4, GL_FLOAT,         GL_FALSE, sizeof(mesh::animVertex), &vert->tangent); // tangent
        gl::VertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(mesh::animVertex), &vert->blendWeight); // blend weight
        gl::VertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(mesh::animVertex), &vert->blendIndex); // blend index
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);
        gl::EnableVertexAttribArray(2);
        gl::EnableVertexAttribArray(3);
        gl::EnableVertexAttribArray(4);
        gl::EnableVertexAttribArray(5);
    } else {
        const auto &vertices = m_model.vertices();
        mesh::vertex *vert = nullptr;
        gl::BufferData(GL_ARRAY_BUFFER, sizeof(mesh::vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh::vertex), &vert->position); // vertex
        gl::VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh::vertex), &vert->normal); // normals
        gl::VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh::vertex), &vert->coordinate); // texCoord
        gl::VertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mesh::vertex), &vert->tangent); // tangent
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);
        gl::EnableVertexAttribArray(2);
        gl::EnableVertexAttribArray(3);
    }

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // TODO: multiple materials
    if (!mat.upload())
        return false;

    return true;
}

void model::render() {
    gl::BindVertexArray(vao);

    // TODO: multiple materials
    const auto &batches = m_model.batches();
    for (const auto &it : batches)
        gl::DrawElements(GL_TRIANGLES, it.count, GL_UNSIGNED_INT, it.offset);
}

void model::animate(float curFrame) {
    m_model.animate(curFrame);
}

bool model::animated() const {
    return m_model.animated();
}

}
