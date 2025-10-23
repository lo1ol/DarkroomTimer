#pragma once

#if __has_include("new.h")
    #include <new.h>
#else
    #include <cstddef>
    #include <new>
#endif

class ModeProcessor {
public:
    virtual ~ModeProcessor() = default;

    // This class shouldn't be created in heap
    // Use placement new instead
    // It allows us to reduce firmware memory about 3%
    static void* operator new(size_t) = delete;
    static void* operator new(size_t size, void* location) { return ::operator new(size, location); }

    static void operator delete(void*) {}

    virtual void process() = 0;
    virtual void reset() = 0;
    virtual void switchMode() = 0;

    virtual void repaint() = 0;
};
