#include "r_ssao.h"
#include "u_vector.h"

namespace r {

ssao::ssao()
    : m_fbo(0)
    , m_width(0)
    , m_height(0)
{
    m_textures[kBuffer] = 0;
    m_textures[kRandom] = 0;
}

ssao::~ssao() {
    destroy();
}

bool ssao::init(const m::perspectiveProjection &project) {
    m_width = project.width / 2;
    m_height = project.height / 2;

    GLenum format = gl::has(ARB_texture_rectangle)
        ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;

    gl::GenTextures(2, m_textures);

    // 16-bit RED
    gl::BindTexture(format, m_textures[kBuffer]);
    gl::TexParameteri(format, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(format, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(format, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::TexParameterf(format, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexImage2D(format, 0, GL_R16F, m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);

    // 8-bit RED random texture
    u::vector<unsigned char> random;
    random.reserve(kRandomSize * kRandomSize);
    for (size_t i = 0; i < kRandomSize; i++)
        for (size_t j = 0; j < kRandomSize; j++)
            random.push_back(rand() % 0xFF);

    gl::BindTexture(format, m_textures[kRandom]);
    gl::TexImage2D(format, 0, GL_R8, kRandomSize, kRandomSize, 0, GL_RED, GL_UNSIGNED_BYTE, &random[0]);

    gl::GenFramebuffers(1, &m_fbo);
    gl::BindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    gl::FramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, format, m_textures[kBuffer], 0);

    static GLuint drawBuffers[] = { GL_COLOR_ATTACHMENT0 };

    gl::DrawBuffers(1, drawBuffers);

    GLenum status = gl::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        return false;

    gl::BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return true;
}

void ssao::update(const m::perspectiveProjection &project) {
    size_t width = project.width / 2;
    size_t height = project.height / 2;

    if (m_width != width || m_height != height) {
        GLenum format = gl::has(ARB_texture_rectangle)
            ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
        m_width = width;
        m_height = height;
        gl::BindTexture(format, m_textures[kBuffer]);
        gl::TexImage2D(format, 0, GL_R16F, m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);
    }
}

void ssao::destroy() {
    if (m_fbo)
        gl::DeleteFramebuffers(1, &m_fbo);
    if (m_textures[0])
        gl::DeleteTextures(2, m_textures);
}

void ssao::bindWriting() {
    gl::BindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

GLuint ssao::texture(textureType type) const {
    return m_textures[type];
}

}
