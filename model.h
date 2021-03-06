#ifndef MODEL_HDR
#define MODEL_HDR
#include "mesh.h"

#include "m_mat.h"

#include "u_string.h"

struct Model {
    Model();
    ~Model();

    struct Batch {
        void *offset;
        size_t count;
        size_t material; // Material index for rendering (calculated in r_model.cpp)
    };

    bool load(const u::string &file, const u::vector<u::string> &anims = {});

    bool isHalf() const;

    const u::vector<Mesh::GeneralVertex> &generalVertices() const;
    const u::vector<Mesh::AnimVertex> &animVertices() const;

    // when isHalf() these are valid
    const u::vector<Mesh::GeneralHalfVertex> &generalHalfVertices() const;
    const u::vector<Mesh::AnimHalfVertex> &animHalfVertices() const;

    const u::vector<unsigned int> &indices() const;
    const u::vector<Batch> &batches() const;
    const u::vector<u::string> &meshNames() const;

    const m::bbox &bounds() const;

    bool animated() const;
    void animate(float curFrame);
    size_t joints() const;
    const float *bones() const;
    const u::string &name() const;

    void makeHalf();
    void makeSingle();

private:
    friend struct OBJ;
    friend struct IQM;

    bool m_isHalf;

    m::bbox m_bounds;
    u::vector<Batch> m_batches;
    u::vector<unsigned int> m_indices;

    // When loading OBJs this is populated with the names of the groups in
    // the OBJ file. When loading IQMs, this is populated with the names of the
    // meshes the IQM is composed of. The IQM file must have mesh names, otherwise
    // this gets populated with a bunch of "default" strings.
    u::vector<u::string> m_meshNames;

    u::string m_name;

    // these are only initialized when m_animated
    size_t m_numFrames; // number of frames
    size_t m_numJoints; // number of joints
    u::vector<m::mat3x4> m_frames; // frames
    u::vector<m::mat3x4> m_outFrame; // animated frames
    u::vector<int32_t> m_parents; // parent joint indices

    u::vector<Mesh::AnimVertex> m_animVertices; // generated data for animated models
    u::vector<Mesh::GeneralVertex> m_generalVertices; // generated data for unanimated models

    // when m_isHalf these are valid
    u::vector<Mesh::AnimHalfVertex> m_animHalfVertices; // generated data for animated models
    u::vector<Mesh::GeneralHalfVertex> m_generalHalfVertices; // generated data for unanimated models
};

inline Model::Model()
    : m_isHalf(false)
    , m_numFrames(0)
    , m_numJoints(0)
{
}

inline bool Model::isHalf() const {
    return m_isHalf;
}

inline const u::vector<Mesh::GeneralVertex> &Model::generalVertices() const {
    return m_generalVertices;
}

inline const u::vector<Mesh::AnimVertex> &Model::animVertices() const {
    return m_animVertices;
}

inline const u::vector<Mesh::GeneralHalfVertex> &Model::generalHalfVertices() const {
    return m_generalHalfVertices;
}

inline const u::vector<Mesh::AnimHalfVertex> &Model::animHalfVertices() const {
    return m_animHalfVertices;
}

inline const u::vector<unsigned int> &Model::indices() const {
    return m_indices;
}

inline const u::vector<Model::Batch> &Model::batches() const {
    return m_batches;
}

inline const u::vector<u::string> &Model::meshNames() const {
    return m_meshNames;
}

inline const m::bbox &Model::bounds() const {
    return m_bounds;
}

inline bool Model::animated() const {
    return m_numFrames;
}

inline const float *Model::bones() const {
    return m_outFrame[0].a.f;
}

inline size_t Model::joints() const {
    return m_numJoints;
}

inline const u::string &Model::name() const {
    return m_name;
}

#endif
