#include <stdlib.h>

void *operator new(size_t len) {
    void *ptr = malloc(len);
    if (!ptr)
        abort();
    return ptr;
}

void *operator new[](size_t len) {
    void *ptr = malloc(len);
    if (!ptr)
        abort();
    return ptr;
}

void operator delete(void *ptr) {
    free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

extern "C" void __cxa_guard_acquire() {
    // Do nothing
}

extern "C" void __cxa_guard_release() {
    // Do nothing
}
