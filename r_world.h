#ifndef R_WORLD_HDR
#define R_WORLD_HDR
#include "kdmap.h"

#include "r_ssao.h"
#include "r_skybox.h"
#include "r_billboard.h"
#include "r_geom.h"
#include "r_model.h"
#include "r_light.h"

#include "u_map.h"

struct world;

namespace r {

struct bboxMethod : method {
    bool init();

    void setWVP(const m::mat4 &wvp);
    void setColor(const m::vec3 &color);

private:
    GLint m_WVPLocation;
    GLint m_colorLocation;
};

struct geomMethod : method {
    bool init(const u::vector<const char *> &defines = u::vector<const char *>());

    void setWVP(const m::mat4 &wvp);
    void setWorld(const m::mat4 &wvp);
    void setColorTextureUnit(int unit);
    void setNormalTextureUnit(int unit);
    void setSpecTextureUnit(int unit);
    void setDispTextureUnit(int unit);
    void setSpecPower(float power);
    void setSpecIntensity(float intensity);
    void setEyeWorldPos(const m::vec3 &position);
    void setParallax(float scale, float bias);

private:
    GLint m_WVPLocation;
    GLint m_worldLocation;
    GLint m_colorTextureUnitLocation;
    GLint m_normalTextureUnitLocation;
    GLint m_specTextureUnitLocation;
    GLint m_dispTextureUnitLocation;
    GLint m_specPowerLocation;
    GLint m_specIntensityLocation;
    GLint m_eyeWorldPositionLocation;
    GLint m_parallaxLocation;
};

struct finalMethod : method {
    bool init(const u::vector<const char *> &defines = u::vector<const char *>());

    void setWVP(const m::mat4 &wvp);
    void setColorTextureUnit(int unit);
    void setPerspective(const m::perspective &perspective);

private:
    GLint m_WVPLocation;
    GLint m_colorMapLocation;
    GLint m_screenSizeLocation;
};

struct finalComposite {
    finalComposite();
    ~finalComposite();

    bool init(const m::perspective &p, GLuint depth);
    void update(const m::perspective &p, GLuint depth);
    void bindWriting();

    GLuint texture() const;

private:
    enum {
        kBuffer,
        kDepth
    };

    void destroy();

    GLuint m_fbo;
    GLuint m_texture;
    size_t m_width;
    size_t m_height;
};

struct portal {
    void render(const pipeline &pl);
private:
    model m_mesh;
};

struct renderTextureBatch {
    int permute;
    size_t start;
    size_t count;
    size_t index;
    material mat; // Rendering material (world and models share this)
};

struct world : geom {
    world();
    ~world();

    bool load(const kdMap &map);
    bool upload(const m::perspective &p);

    void render(const pipeline &pl, ::world *map);

private:
    void geometryPass(const pipeline &pl, ::world *map);
    void lightingPass(const pipeline &pl, ::world *map);
    void forwardPass(const pipeline &pl, ::world *map);
    void compositePass(const pipeline &pl);

    // world shading methods and permutations
    u::vector<geomMethod> m_geomMethods;
    u::vector<finalMethod> m_finalMethods;
    u::vector<directionalLightMethod> m_directionalLightMethods;
    pointLightMethod m_pointLightMethod;
    spotLightMethod m_spotLightMethod;
    ssaoMethod m_ssaoMethod;
    bboxMethod m_bboxMethod;

    // Other things in the world to render
    skybox m_skybox;
    quad m_quad;
    sphere m_sphere;
    bbox m_bbox;
    u::map<u::string, model*> m_models;

    // The world itself
    u::vector<uint32_t> m_indices;
    u::vector<kdBinVertex> m_vertices;
    u::vector<renderTextureBatch> m_textureBatches;
    u::map<u::string, texture2D*> m_textures2D;

    gBuffer m_gBuffer;
    ssao m_ssao;
    finalComposite m_final;

    m::mat4 m_identity;
    m::frustum m_frustum;

    bool m_uploaded;
};

}

#endif
