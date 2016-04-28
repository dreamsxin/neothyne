#ifndef A_FILTER_HDR
#define A_FILTER_HDR
#include "u_vector.h"
#include "m_vec.h"

namespace a {

struct source;

struct filterInstance {
    virtual void filter(float *buffer, size_t samples, bool strero, float sampleRate) = 0;
    virtual ~filterInstance();
};

struct filter {
    virtual void init(source *);
    virtual filterInstance *create() = 0;
    virtual ~filter();
};

struct echoFilter;

struct echoFilterInstance : filterInstance {
    virtual void filter(float *buffer, size_t samples, bool stereo, float sampleRate);
    virtual ~echoFilterInstance();
    echoFilterInstance(echoFilter *parent);

private:
    u::vector<float> m_buffer;
    echoFilter *m_parent;
    size_t m_offset;
};

struct echoFilter : filter {
    virtual void init(source *sourcer_);
    virtual filterInstance *create();
    echoFilter();
    void setParams(float delay, float decay);

private:
    friend struct echoFilterInstance;
    float m_delay;
    float m_decay;
};

struct BQRFilter;

struct BQRFilterInstance : filterInstance {
    BQRFilterInstance(BQRFilter *parent);
    virtual void filter(float *buffer, size_t samples, bool stereo, float sampleRate);
    virtual ~BQRFilterInstance();
private:
    BQRFilter *m_parent;
    m::vec2 m_x1, m_x2;
    m::vec2 m_y1, m_y2;
};

struct BQRFilter : filter {
    enum {
        kLowPass,
        kHighPass,
        kBandPass
    };

    virtual void init(source *);
    virtual BQRFilterInstance *create();
    BQRFilter();
    virtual ~BQRFilter();
    void setParams(int type, float sampleRate, float frequency, float resonance);

private:
    friend struct BQRFilterInstance;

    m::vec3 m_a;
    m::vec2 m_b;
};

}

#endif