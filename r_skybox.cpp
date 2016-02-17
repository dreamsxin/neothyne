#include "cvar.h"
#include "world.h"

#include "r_skybox.h"
#include "r_pipeline.h"

#include "u_string.h"
#include "u_vector.h"
#include "u_misc.h"

namespace r {

///! renderer
skybox::skybox()
    : m_skyboxMethod(nullptr)
    , m_foggedSkyboxMethod(nullptr)
{
}

bool skybox::load(const u::string &skyboxName) {
    if (!m_cubemap.load(skyboxName + "_ft", skyboxName + "_bk", skyboxName + "_up",
                        skyboxName + "_dn", skyboxName + "_rt", skyboxName + "_lf"))
        return false;

    // Calculate the average color of the top of the skybox. We utilize this color
    // for vertical fog mixture that reaches into the sky if the map has fog at
    // all.
    const auto &tex = m_cubemap.get(texture3D::kUp);
    const auto &data = tex.data();

    uint32_t totals[3] = {0};
    const size_t stride = tex.width() * tex.bpp();
    for (size_t y = 0; y < tex.height(); y++) {
        for (size_t x = 0; x < tex.width(); x++) {
            for (size_t i = 0; i < 3; i++) {
                const size_t index = (y * stride) + x*tex.bpp() + i;
                totals[i] += data[index];
            }
        }
    }
    int reduce[3] = {0};
    for (size_t i = 0; i < 3; i++)
        reduce[i] = totals[i] / (tex.width() * tex.height());

    m_skyColor = { reduce[0] / 255.0f,
                   reduce[1] / 255.0f,
                   reduce[2] / 255.0f };

    return true;
}

bool skybox::upload() {
    if (!m_cube.upload())
        return false;
    if (!m_cubemap.upload())
        return false;

    m_skyboxMethod = &r::methods::instance()["skybox"];
    m_skyboxMethod->enable();
    m_skyboxMethod->getUniform("gCubemap")->set(0);
    m_skyboxMethod->getUniform("gSkyColor")->set(m_skyColor);

    m_foggedSkyboxMethod = &r::methods::instance()["fogged skybox"];
    m_foggedSkyboxMethod->enable();
    m_foggedSkyboxMethod->getUniform("gCubemap")->set(0);
    m_foggedSkyboxMethod->getUniform("gSkyColor")->set(m_skyColor);
    return true;
}

void skybox::render(const pipeline &pl, const fog &f) {
    // Construct the matrix for the skybox
    pipeline wpl = pl;
    pipeline p;
    p.setWorld(pl.position());
    p.setPosition(pl.position());
    p.setRotation(pl.rotation());
    p.setPerspective(pl.perspective());

    method *renderMethod = nullptr;
    if (varGet<int>("r_fog")) {
        renderMethod = m_foggedSkyboxMethod;
        renderMethod->enable();
        renderMethod->getUniform("gFog.color")->set(f.color);
        renderMethod->getUniform("gFog.density")->set(f.density);
    } else {
        renderMethod = m_skyboxMethod;
        renderMethod->enable();
    }

    renderMethod->getUniform("gWVP")->set(p.projection() * p.view() * p.world());
    renderMethod->getUniform("gWorld")->set(wpl.world());

    // render skybox cube
    gl::DepthRange(1, 1);
    gl::DepthFunc(GL_LEQUAL);
    gl::CullFace(GL_FRONT);
    gl::Disable(GL_BLEND);

    m_cubemap.bind(GL_TEXTURE0); // bind cubemap texture

    m_cube.render();

    gl::DepthRange(0, 1);
    gl::DepthFunc(GL_LESS);
    gl::CullFace(GL_BACK);
    gl::Enable(GL_BLEND);
}

}
