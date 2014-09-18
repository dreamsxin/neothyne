// File automatically generated by ./r_generator.py
#include <SDL2/SDL.h> // SDL_GL_GetProcAddress
#include <stdarg.h>
#define R_COMMON_NO_DEFINES
#include "r_common.h"
#include "util.h"

#ifndef APIENTRY
#   define APIENTRY
#endif
#ifndef APIENTRYP
#   define APIENTRYP APIENTRY *
#endif

#ifdef DEBUG_GL
#   define GL_CHECK_0(...)     debugCheck("", __func__, file, line)
#   define GL_CHECK_1(SP, ...) debugCheck(SP, __func__, file, line)
#else
#   define GL_CHECK_0(...)
#   define GL_CHECK_1(...)
#endif

#define PP_PHELP(X, Y) \
   X ## Y
#define PP_PASTE(X, Y) \
   PP_PHELP(X, Y)
#define PP_SKIP(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N, ...) \
   N
#define PP_SCAN(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, ...) \
   PP_SKIP(, __VA_ARGS__, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
#define PP_COUNT(...) \
   PP_SCAN(12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, __VA_ARGS__)
#define GL_CHECK_ARG(...) \
   PP_SCAN(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, __VA_ARGS__)
#define GL_CHECK(...) \
   PP_PASTE(GL_CHECK_, GL_CHECK_ARG(__VA_ARGS__))(__VA_ARGS__)

typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar**, const GLint*);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)();
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar*);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei, const GLuint*);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint, GLint);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint, GLfloat);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint, GLfloat, GLfloat);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint*);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint*);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei, const GLenum*);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum);
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum, GLsizei, GLenum, const GLvoid*);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum, GLuint);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei, const GLuint*);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum, GLenum, GLfloat);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum, GLenum, GLint);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum, GLint, GLsizei);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum, GLenum);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum);
typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean, GLboolean, GLboolean, GLboolean);
typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum, GLint*);
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC)(GLenum);
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum, GLuint);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)();

static PFNGLCREATESHADERPROC              glCreateShader_             = nullptr;
static PFNGLSHADERSOURCEPROC              glShaderSource_             = nullptr;
static PFNGLCOMPILESHADERPROC             glCompileShader_            = nullptr;
static PFNGLATTACHSHADERPROC              glAttachShader_             = nullptr;
static PFNGLCREATEPROGRAMPROC             glCreateProgram_            = nullptr;
static PFNGLLINKPROGRAMPROC               glLinkProgram_              = nullptr;
static PFNGLUSEPROGRAMPROC                glUseProgram_               = nullptr;
static PFNGLGETUNIFORMLOCATIONPROC        glGetUniformLocation_       = nullptr;
static PFNGLENABLEVERTEXATTRIBARRAYPROC   glEnableVertexAttribArray_  = nullptr;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC  glDisableVertexAttribArray_ = nullptr;
static PFNGLUNIFORMMATRIX4FVPROC          glUniformMatrix4fv_         = nullptr;
static PFNGLBINDBUFFERPROC                glBindBuffer_               = nullptr;
static PFNGLGENBUFFERSPROC                glGenBuffers_               = nullptr;
static PFNGLVERTEXATTRIBPOINTERPROC       glVertexAttribPointer_      = nullptr;
static PFNGLBUFFERDATAPROC                glBufferData_               = nullptr;
static PFNGLVALIDATEPROGRAMPROC           glValidateProgram_          = nullptr;
static PFNGLGENVERTEXARRAYSPROC           glGenVertexArrays_          = nullptr;
static PFNGLBINDVERTEXARRAYPROC           glBindVertexArray_          = nullptr;
static PFNGLDELETEPROGRAMPROC             glDeleteProgram_            = nullptr;
static PFNGLDELETEBUFFERSPROC             glDeleteBuffers_            = nullptr;
static PFNGLDELETEVERTEXARRAYSPROC        glDeleteVertexArrays_       = nullptr;
static PFNGLUNIFORM1IPROC                 glUniform1i_                = nullptr;
static PFNGLUNIFORM1FPROC                 glUniform1f_                = nullptr;
static PFNGLUNIFORM2FPROC                 glUniform2f_                = nullptr;
static PFNGLUNIFORM3FVPROC                glUniform3fv_               = nullptr;
static PFNGLGENERATEMIPMAPPROC            glGenerateMipmap_           = nullptr;
static PFNGLDELETESHADERPROC              glDeleteShader_             = nullptr;
static PFNGLGETSHADERIVPROC               glGetShaderiv_              = nullptr;
static PFNGLGETPROGRAMIVPROC              glGetProgramiv_             = nullptr;
static PFNGLGETSHADERINFOLOGPROC          glGetShaderInfoLog_         = nullptr;
static PFNGLACTIVETEXTUREPROC             glActiveTexture_            = nullptr;
static PFNGLGENFRAMEBUFFERSPROC           glGenFramebuffers_          = nullptr;
static PFNGLBINDFRAMEBUFFERPROC           glBindFramebuffer_          = nullptr;
static PFNGLFRAMEBUFFERTEXTURE2DPROC      glFramebufferTexture2D_     = nullptr;
static PFNGLDRAWBUFFERSPROC               glDrawBuffers_              = nullptr;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC    glCheckFramebufferStatus_   = nullptr;
static PFNGLDELETEFRAMEBUFFERSPROC        glDeleteFramebuffers_       = nullptr;
static PFNGLCLEARPROC                     glClear_                    = nullptr;
static PFNGLCLEARCOLORPROC                glClearColor_               = nullptr;
static PFNGLFRONTFACEPROC                 glFrontFace_                = nullptr;
static PFNGLCULLFACEPROC                  glCullFace_                 = nullptr;
static PFNGLENABLEPROC                    glEnable_                   = nullptr;
static PFNGLDISABLEPROC                   glDisable_                  = nullptr;
static PFNGLDRAWELEMENTSPROC              glDrawElements_             = nullptr;
static PFNGLDEPTHMASKPROC                 glDepthMask_                = nullptr;
static PFNGLBINDTEXTUREPROC               glBindTexture_              = nullptr;
static PFNGLTEXIMAGE2DPROC                glTexImage2D_               = nullptr;
static PFNGLDELETETEXTURESPROC            glDeleteTextures_           = nullptr;
static PFNGLGENTEXTURESPROC               glGenTextures_              = nullptr;
static PFNGLTEXPARAMETERFPROC             glTexParameterf_            = nullptr;
static PFNGLTEXPARAMETERIPROC             glTexParameteri_            = nullptr;
static PFNGLDRAWARRAYSPROC                glDrawArrays_               = nullptr;
static PFNGLBLENDEQUATIONPROC             glBlendEquation_            = nullptr;
static PFNGLBLENDFUNCPROC                 glBlendFunc_                = nullptr;
static PFNGLDEPTHFUNCPROC                 glDepthFunc_                = nullptr;
static PFNGLCOLORMASKPROC                 glColorMask_                = nullptr;
static PFNGLREADPIXELSPROC                glReadPixels_               = nullptr;
static PFNGLVIEWPORTPROC                  glViewport_                 = nullptr;
static PFNGLGETINTEGERVPROC               glGetIntegerv_              = nullptr;
static PFNGLGETSTRINGPROC                 glGetString_                = nullptr;
static PFNGLGETSTRINGIPROC                glGetStringi_               = nullptr;
static PFNGLGETERRORPROC                  glGetError_                 = nullptr;

#ifdef DEBUG_GL
template <char C, typename T>
u::string stringize(T, char base='?');

template<>
u::string stringize<'1', GLchar>(GLchar value, char) {
    return u::format("GLchar=%c", value);
}
template<>
u::string stringize<'2', GLenum>(GLenum value, char) {
    return u::format("GLenum=0x%X", value);
}
template<>
u::string stringize<'3', GLboolean>(GLboolean value, char) {
    return u::format("GLboolean=%c", value);
}
template<>
u::string stringize<'4', GLbitfield>(GLbitfield value, char) {
    return u::format("GLbitfield=%u", value);
}
template<>
u::string stringize<'5', GLbyte>(GLbyte value, char) {
    return u::format("GLbyte=%x", value);
}
template<>
u::string stringize<'6', GLshort>(GLshort value, char) {
    return u::format("GLshort=%d", value);
}
template<>
u::string stringize<'7', GLint>(GLint value, char) {
    return u::format("GLint=%d", value);
}
template<>
u::string stringize<'8', GLsizei>(GLsizei value, char) {
    return u::format("GLsizei=%d", value);
}
template<>
u::string stringize<'9', GLubyte>(GLubyte value, char) {
    return u::format("GLubyte=%X", value);
}
template<>
u::string stringize<'a', GLushort>(GLushort value, char) {
    return u::format("GLushort=%u", value);
}
template<>
u::string stringize<'b', GLuint>(GLuint value, char) {
    return u::format("GLuint=%u", value);
}
template<>
u::string stringize<'c', GLfloat>(GLfloat value, char) {
    return u::format("GLfloat=%.2f", value);
}
template<>
u::string stringize<'d', GLclampf>(GLclampf value, char) {
    return u::format("GLclampf=%f", value);
}
template<>
u::string stringize<'e', GLintptr>(GLintptr value, char) {
    return u::format("GLintptr=%p", value);
}
template<>
u::string stringize<'f', GLsizeiptr>(GLsizeiptr value, char) {
    return u::format("GLsizeiptr=%p", value);
}
template <>
u::string stringize<'*', void *>(void *value, char base) {
    switch (base) {
        case '?': return "unknown";
        case '0': return u::format("GLvoid*=%p", value);
        case '1': return u::format("GLchar*=%p", value);
        case '2': return u::format("GLenum*=%p", value);
        case '3': return u::format("GLboolean*=%p", value);
        case '4': return u::format("GLbitfield*=%p", value);
        case '5': return u::format("GLbyte*=%p", value);
        case '6': return u::format("GLshort*=%p", value);
        case '7': return u::format("GLint*=%p", value);
        case '8': return u::format("GLsizei*=%p", value);
        case '9': return u::format("GLubyte*=%p", value);
        case 'a': return u::format("GLushort*=%p", value);
        case 'b': return u::format("GLuint*=%p", value);
        case 'c': return u::format("GLfloat*=%p", value);
        case 'd': return u::format("GLclampf*=%p", value);
        case 'e': return u::format("GLintptr*=%p", value);
        case 'f': return u::format("GLsizeiptr*=%p", value);
    }

    return u::format("GLchar*=\"%s\"", (const char *)value);
}

static const char *debugErrorString(GLenum error) {
    switch (error) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
    }
    return "unknown";
}

static void debugCheck(const char *spec, const char *function, const char *file, size_t line, ...) {
    GLenum error = glGetError_();
    if (error == GL_NO_ERROR)
        return;

    va_list va;
    va_start(va, line);
    u::string contents;

    for (const char *s = spec; *s; s++) {
        switch (*s) {
            case '1':
                contents += stringize<'1'>((GLchar)va_arg(va, int));
                break;
            case '2':
                contents += stringize<'2'>((GLenum)va_arg(va, unsigned int));
                break;
            case '3':
                contents += stringize<'3'>((GLboolean)va_arg(va, int));
                break;
            case '4':
                contents += stringize<'4'>((GLbitfield)va_arg(va, unsigned int));
                break;
            case '5':
                contents += stringize<'5'>((GLbyte)va_arg(va, int));
                break;
            case '6':
                contents += stringize<'6'>((GLshort)va_arg(va, int));
                break;
            case '7':
                contents += stringize<'7'>((GLint)va_arg(va, int));
                break;
            case '8':
                contents += stringize<'8'>((GLsizei)va_arg(va, int));
                break;
            case '9':
                contents += stringize<'9'>((GLubyte)va_arg(va, unsigned int));
                break;
            case 'a':
                contents += stringize<'a'>((GLushort)va_arg(va, unsigned int));
                break;
            case 'b':
                contents += stringize<'b'>((GLuint)va_arg(va, unsigned int));
                break;
            case 'c':
                contents += stringize<'c'>((GLfloat)va_arg(va, double));
                break;
            case 'd':
                contents += stringize<'d'>((GLclampf)va_arg(va, double));
                break;
            case 'e':
                contents += stringize<'e'>((GLintptr)va_arg(va, intptr_t));
                break;
            case 'f':
                contents += stringize<'f'>((GLsizeiptr)va_arg(va, intptr_t));
                break;
            case '*':
                contents += stringize<'*'>(va_arg(va, void *), s[1]);
                s++; // skip basetype spec
                break;
        }
        if (s[1])
            contents += ", ";
    }
    va_end(va);
    fprintf(stderr, "error %s(%s) (%s:%zu) %s\n", function, contents.c_str(),
        file, line, debugErrorString(error));
}
#endif

namespace gl {
    static u::set<u::string> extensions;

    void init(void) {
        glCreateShader_             = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
        glShaderSource_             = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
        glCompileShader_            = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
        glAttachShader_             = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
        glCreateProgram_            = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
        glLinkProgram_              = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
        glUseProgram_               = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
        glGetUniformLocation_       = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
        glEnableVertexAttribArray_  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
        glDisableVertexAttribArray_ = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glDisableVertexAttribArray");
        glUniformMatrix4fv_         = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
        glBindBuffer_               = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
        glGenBuffers_               = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
        glVertexAttribPointer_      = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
        glBufferData_               = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
        glValidateProgram_          = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
        glGenVertexArrays_          = (PFNGLGENVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glGenVertexArrays");
        glBindVertexArray_          = (PFNGLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
        glDeleteProgram_            = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");
        glDeleteBuffers_            = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
        glDeleteVertexArrays_       = (PFNGLDELETEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glDeleteVertexArrays");
        glUniform1i_                = (PFNGLUNIFORM1IPROC)SDL_GL_GetProcAddress("glUniform1i");
        glUniform1f_                = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
        glUniform2f_                = (PFNGLUNIFORM2FPROC)SDL_GL_GetProcAddress("glUniform2f");
        glUniform3fv_               = (PFNGLUNIFORM3FVPROC)SDL_GL_GetProcAddress("glUniform3fv");
        glGenerateMipmap_           = (PFNGLGENERATEMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateMipmap");
        glDeleteShader_             = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
        glGetShaderiv_              = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
        glGetProgramiv_             = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
        glGetShaderInfoLog_         = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
        glActiveTexture_            = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");
        glGenFramebuffers_          = (PFNGLGENFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glGenFramebuffers");
        glBindFramebuffer_          = (PFNGLBINDFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBindFramebuffer");
        glFramebufferTexture2D_     = (PFNGLFRAMEBUFFERTEXTURE2DPROC)SDL_GL_GetProcAddress("glFramebufferTexture2D");
        glDrawBuffers_              = (PFNGLDRAWBUFFERSPROC)SDL_GL_GetProcAddress("glDrawBuffers");
        glCheckFramebufferStatus_   = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)SDL_GL_GetProcAddress("glCheckFramebufferStatus");
        glDeleteFramebuffers_       = (PFNGLDELETEFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteFramebuffers");
        glClear_                    = (PFNGLCLEARPROC)SDL_GL_GetProcAddress("glClear");
        glClearColor_               = (PFNGLCLEARCOLORPROC)SDL_GL_GetProcAddress("glClearColor");
        glFrontFace_                = (PFNGLFRONTFACEPROC)SDL_GL_GetProcAddress("glFrontFace");
        glCullFace_                 = (PFNGLCULLFACEPROC)SDL_GL_GetProcAddress("glCullFace");
        glEnable_                   = (PFNGLENABLEPROC)SDL_GL_GetProcAddress("glEnable");
        glDisable_                  = (PFNGLDISABLEPROC)SDL_GL_GetProcAddress("glDisable");
        glDrawElements_             = (PFNGLDRAWELEMENTSPROC)SDL_GL_GetProcAddress("glDrawElements");
        glDepthMask_                = (PFNGLDEPTHMASKPROC)SDL_GL_GetProcAddress("glDepthMask");
        glBindTexture_              = (PFNGLBINDTEXTUREPROC)SDL_GL_GetProcAddress("glBindTexture");
        glTexImage2D_               = (PFNGLTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glTexImage2D");
        glDeleteTextures_           = (PFNGLDELETETEXTURESPROC)SDL_GL_GetProcAddress("glDeleteTextures");
        glGenTextures_              = (PFNGLGENTEXTURESPROC)SDL_GL_GetProcAddress("glGenTextures");
        glTexParameterf_            = (PFNGLTEXPARAMETERFPROC)SDL_GL_GetProcAddress("glTexParameterf");
        glTexParameteri_            = (PFNGLTEXPARAMETERIPROC)SDL_GL_GetProcAddress("glTexParameteri");
        glDrawArrays_               = (PFNGLDRAWARRAYSPROC)SDL_GL_GetProcAddress("glDrawArrays");
        glBlendEquation_            = (PFNGLBLENDEQUATIONPROC)SDL_GL_GetProcAddress("glBlendEquation");
        glBlendFunc_                = (PFNGLBLENDFUNCPROC)SDL_GL_GetProcAddress("glBlendFunc");
        glDepthFunc_                = (PFNGLDEPTHFUNCPROC)SDL_GL_GetProcAddress("glDepthFunc");
        glColorMask_                = (PFNGLCOLORMASKPROC)SDL_GL_GetProcAddress("glColorMask");
        glReadPixels_               = (PFNGLREADPIXELSPROC)SDL_GL_GetProcAddress("glReadPixels");
        glViewport_                 = (PFNGLVIEWPORTPROC)SDL_GL_GetProcAddress("glViewport");
        glGetIntegerv_              = (PFNGLGETINTEGERVPROC)SDL_GL_GetProcAddress("glGetIntegerv");
        glGetString_                = (PFNGLGETSTRINGPROC)SDL_GL_GetProcAddress("glGetString");
        glGetStringi_               = (PFNGLGETSTRINGIPROC)SDL_GL_GetProcAddress("glGetStringi");
        glGetError_                 = (PFNGLGETERRORPROC)SDL_GL_GetProcAddress("glGetError");

        GLint count = 0;
        glGetIntegerv_(GL_NUM_EXTENSIONS, &count);
        for (GLint i = 0; i < count; i++)
            extensions.emplace((const char *)glGetStringi_(GL_EXTENSIONS, i));
    }

    bool has(const char *ext) {
         return extensions.find(ext) != extensions.end();
    }

    GLuint CreateShader(GLenum shaderType GL_INFOP) {
        GLuint result = glCreateShader_(shaderType);
        GL_CHECK("2", shaderType);
        return result;
    }

    void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length GL_INFOP) {
        glShaderSource_(shader, count, string, length);
        GL_CHECK("b8*0*7", shader, count, string, length);
    }

    void CompileShader(GLuint shader GL_INFOP) {
        glCompileShader_(shader);
        GL_CHECK("b", shader);
    }

    void AttachShader(GLuint program, GLuint shader GL_INFOP) {
        glAttachShader_(program, shader);
        GL_CHECK("bb", program, shader);
    }

    GLuint CreateProgram(GL_INFO) {
        GLuint result = glCreateProgram_();
        GL_CHECK();
        return result;
    }

    void LinkProgram(GLuint program GL_INFOP) {
        glLinkProgram_(program);
        GL_CHECK("b", program);
    }

    void UseProgram(GLuint program GL_INFOP) {
        glUseProgram_(program);
        GL_CHECK("b", program);
    }

    GLint GetUniformLocation(GLuint program, const GLchar* name GL_INFOP) {
        GLint result = glGetUniformLocation_(program, name);
        GL_CHECK("b*1", program, name);
        return result;
    }

    void EnableVertexAttribArray(GLuint index GL_INFOP) {
        glEnableVertexAttribArray_(index);
        GL_CHECK("b", index);
    }

    void DisableVertexAttribArray(GLuint index GL_INFOP) {
        glDisableVertexAttribArray_(index);
        GL_CHECK("b", index);
    }

    void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value GL_INFOP) {
        glUniformMatrix4fv_(location, count, transpose, value);
        GL_CHECK("783*c", location, count, transpose, value);
    }

    void BindBuffer(GLenum target, GLuint buffer GL_INFOP) {
        glBindBuffer_(target, buffer);
        GL_CHECK("2b", target, buffer);
    }

    void GenBuffers(GLsizei n, GLuint* buffers GL_INFOP) {
        glGenBuffers_(n, buffers);
        GL_CHECK("8*b", n, buffers);
    }

    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer GL_INFOP) {
        glVertexAttribPointer_(index, size, type, normalized, stride, pointer);
        GL_CHECK("b7238*0", index, size, type, normalized, stride, pointer);
    }

    void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage GL_INFOP) {
        glBufferData_(target, size, data, usage);
        GL_CHECK("2f*02", target, size, data, usage);
    }

    void ValidateProgram(GLuint program GL_INFOP) {
        glValidateProgram_(program);
        GL_CHECK("b", program);
    }

    void GenVertexArrays(GLsizei n, GLuint* arrays GL_INFOP) {
        glGenVertexArrays_(n, arrays);
        GL_CHECK("8*b", n, arrays);
    }

    void BindVertexArray(GLuint array GL_INFOP) {
        glBindVertexArray_(array);
        GL_CHECK("b", array);
    }

    void DeleteProgram(GLuint program GL_INFOP) {
        glDeleteProgram_(program);
        GL_CHECK("b", program);
    }

    void DeleteBuffers(GLsizei n, const GLuint* buffers GL_INFOP) {
        glDeleteBuffers_(n, buffers);
        GL_CHECK("8*b", n, buffers);
    }

    void DeleteVertexArrays(GLsizei n, const GLuint* arrays GL_INFOP) {
        glDeleteVertexArrays_(n, arrays);
        GL_CHECK("8*b", n, arrays);
    }

    void Uniform1i(GLint location, GLint v0 GL_INFOP) {
        glUniform1i_(location, v0);
        GL_CHECK("77", location, v0);
    }

    void Uniform1f(GLint location, GLfloat v0 GL_INFOP) {
        glUniform1f_(location, v0);
        GL_CHECK("7c", location, v0);
    }

    void Uniform2f(GLint location, GLfloat v0, GLfloat v1 GL_INFOP) {
        glUniform2f_(location, v0, v1);
        GL_CHECK("7cc", location, v0, v1);
    }

    void Uniform3fv(GLint location, GLsizei count, const GLfloat* value GL_INFOP) {
        glUniform3fv_(location, count, value);
        GL_CHECK("78*c", location, count, value);
    }

    void GenerateMipmap(GLenum target GL_INFOP) {
        glGenerateMipmap_(target);
        GL_CHECK("2", target);
    }

    void DeleteShader(GLuint shader GL_INFOP) {
        glDeleteShader_(shader);
        GL_CHECK("b", shader);
    }

    void GetShaderiv(GLuint shader, GLenum pname, GLint* params GL_INFOP) {
        glGetShaderiv_(shader, pname, params);
        GL_CHECK("b2*7", shader, pname, params);
    }

    void GetProgramiv(GLuint program, GLenum pname, GLint* params GL_INFOP) {
        glGetProgramiv_(program, pname, params);
        GL_CHECK("b2*7", program, pname, params);
    }

    void GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog GL_INFOP) {
        glGetShaderInfoLog_(shader, maxLength, length, infoLog);
        GL_CHECK("b8*8*1", shader, maxLength, length, infoLog);
    }

    void ActiveTexture(GLenum texture GL_INFOP) {
        glActiveTexture_(texture);
        GL_CHECK("2", texture);
    }

    void GenFramebuffers(GLsizei n, GLuint* ids GL_INFOP) {
        glGenFramebuffers_(n, ids);
        GL_CHECK("8*b", n, ids);
    }

    void BindFramebuffer(GLenum target, GLuint framebuffer GL_INFOP) {
        glBindFramebuffer_(target, framebuffer);
        GL_CHECK("2b", target, framebuffer);
    }

    void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level GL_INFOP) {
        glFramebufferTexture2D_(target, attachment, textarget, texture, level);
        GL_CHECK("222b7", target, attachment, textarget, texture, level);
    }

    void DrawBuffers(GLsizei n, const GLenum* bufs GL_INFOP) {
        glDrawBuffers_(n, bufs);
        GL_CHECK("8*2", n, bufs);
    }

    GLenum CheckFramebufferStatus(GLenum target GL_INFOP) {
        GLenum result = glCheckFramebufferStatus_(target);
        GL_CHECK("2", target);
        return result;
    }

    void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers GL_INFOP) {
        glDeleteFramebuffers_(n, framebuffers);
        GL_CHECK("8*b", n, framebuffers);
    }

    void Clear(GLbitfield mask GL_INFOP) {
        glClear_(mask);
        GL_CHECK("4", mask);
    }

    void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha GL_INFOP) {
        glClearColor_(red, green, blue, alpha);
        GL_CHECK("cccc", red, green, blue, alpha);
    }

    void FrontFace(GLenum mode GL_INFOP) {
        glFrontFace_(mode);
        GL_CHECK("2", mode);
    }

    void CullFace(GLenum mode GL_INFOP) {
        glCullFace_(mode);
        GL_CHECK("2", mode);
    }

    void Enable(GLenum cap GL_INFOP) {
        glEnable_(cap);
        GL_CHECK("2", cap);
    }

    void Disable(GLenum cap GL_INFOP) {
        glDisable_(cap);
        GL_CHECK("2", cap);
    }

    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices GL_INFOP) {
        glDrawElements_(mode, count, type, indices);
        GL_CHECK("282*0", mode, count, type, indices);
    }

    void DepthMask(GLboolean flag GL_INFOP) {
        glDepthMask_(flag);
        GL_CHECK("3", flag);
    }

    void BindTexture(GLenum target, GLuint texture GL_INFOP) {
        glBindTexture_(target, texture);
        GL_CHECK("2b", target, texture);
    }

    void TexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data GL_INFOP) {
        glTexImage2D_(target, level, internalFormat, width, height, border, format, type, data);
        GL_CHECK("27788722*0", target, level, internalFormat, width, height, border, format, type, data);
    }

    void DeleteTextures(GLsizei n, const GLuint* textures GL_INFOP) {
        glDeleteTextures_(n, textures);
        GL_CHECK("8*b", n, textures);
    }

    void GenTextures(GLsizei n, GLuint* textures GL_INFOP) {
        glGenTextures_(n, textures);
        GL_CHECK("8*b", n, textures);
    }

    void TexParameterf(GLenum target, GLenum pname, GLfloat param GL_INFOP) {
        glTexParameterf_(target, pname, param);
        GL_CHECK("22c", target, pname, param);
    }

    void TexParameteri(GLenum target, GLenum pname, GLint param GL_INFOP) {
        glTexParameteri_(target, pname, param);
        GL_CHECK("227", target, pname, param);
    }

    void DrawArrays(GLenum mode, GLint first, GLsizei count GL_INFOP) {
        glDrawArrays_(mode, first, count);
        GL_CHECK("278", mode, first, count);
    }

    void BlendEquation(GLenum mode GL_INFOP) {
        glBlendEquation_(mode);
        GL_CHECK("2", mode);
    }

    void BlendFunc(GLenum sfactor, GLenum dfactor GL_INFOP) {
        glBlendFunc_(sfactor, dfactor);
        GL_CHECK("22", sfactor, dfactor);
    }

    void DepthFunc(GLenum func GL_INFOP) {
        glDepthFunc_(func);
        GL_CHECK("2", func);
    }

    void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha GL_INFOP) {
        glColorMask_(red, green, blue, alpha);
        GL_CHECK("3333", red, green, blue, alpha);
    }

    void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data GL_INFOP) {
        glReadPixels_(x, y, width, height, format, type, data);
        GL_CHECK("778822*0", x, y, width, height, format, type, data);
    }

    void Viewport(GLint x, GLint y, GLsizei width, GLsizei height GL_INFOP) {
        glViewport_(x, y, width, height);
        GL_CHECK("7788", x, y, width, height);
    }

    void GetIntegerv(GLenum pname, GLint* data GL_INFOP) {
        glGetIntegerv_(pname, data);
        GL_CHECK("2*7", pname, data);
    }

    const GLubyte* GetString(GLenum name GL_INFOP) {
        const GLubyte* result = glGetString_(name);
        GL_CHECK("2", name);
        return result;
    }

    const GLubyte* GetStringi(GLenum name, GLuint index GL_INFOP) {
        const GLubyte* result = glGetStringi_(name, index);
        GL_CHECK("2b", name, index);
        return result;
    }

    GLenum GetError(GL_INFO) {
        GLenum result = glGetError_();
        GL_CHECK();
        return result;
    }
}
