#ifndef S_GC_HDR
#define S_GC_HDR

#include <stddef.h>

namespace s {

struct Object;

struct GC {
    struct RootSet {
        Object **m_objects;
        size_t m_length;
        RootSet *m_prev;
        RootSet *m_next;
    };

    struct State {
        RootSet *m_tail;
    };

    static void *addRoots(Object **objects, size_t length);
    static void removeRoots(void *base);
    static void run();

private:
    static void mark();
    static void sweep();
};

}

#endif