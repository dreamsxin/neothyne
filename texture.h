#ifndef TEXTURE_HDR
#define TEXTURE_HDR
#include "util.h"
#include "u_optional.h"

enum textureFormat {
    TEX_RGB,
    TEX_RGBA,
    TEX_BGR,
    TEX_BGRA,
    TEX_LUMINANCE
};

struct texture {
    texture() :
        m_normal(false)
    {
    }

    bool load(const u::string &file);
    void from(const unsigned char *const data, size_t length, size_t width, size_t height, textureFormat format);

    template <size_t S>
    static void halve(unsigned char *src, size_t sw, size_t sh, size_t stride, unsigned char *dst);

    template <size_t S>
    static void shift(unsigned char *src, size_t sw, size_t sh, size_t stride, unsigned char *dst, size_t dw, size_t dh);

    template <size_t S>
    static void scale(unsigned char *src, size_t sw, size_t sh, size_t stride, unsigned char *dst, size_t dw, size_t dh);

    static void scale(unsigned char *src, size_t sw, size_t sh, size_t bpp, size_t pitch, unsigned char *dst, size_t dw, size_t dh);
    static void reorient(unsigned char *src, size_t sw, size_t sh, size_t bpp, size_t stride, unsigned char *dst, bool flipx, bool flipy, bool swapxy);

    void resize(size_t width, size_t height);

    template <textureFormat F>
    void convert(void);

    size_t width(void) const;
    size_t height(void) const;
    textureFormat format(void) const;

    const unsigned char *data(void) const;

    size_t size(void) const;

    const u::string &hashString(void) const;

    // If the texture is a normal map this will return true.
    // We need to delininate between normal map textures and diffuse textures
    // because we don't want to represent normals with S3TC compression.
    bool normal(void) const;

    void unload(void);

private:
    u::optional<u::string> find(const u::string &file);
    template <typename T>
    bool decode(const u::vector<unsigned char> &data, const char *fileName);

    u::string m_hashString;
    u::vector<unsigned char> m_data;
    size_t m_width;
    size_t m_height;
    size_t m_bpp;
    size_t m_pitch;
    bool m_normal;
    textureFormat m_format;
};

#endif
