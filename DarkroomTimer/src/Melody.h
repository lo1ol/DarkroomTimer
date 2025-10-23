#pragma once

#if __has_include("new.h")
    #include <new.h>
#else
    #include <cstddef>
    #include <new>
#endif

class Melody {
public:
    enum Name { alarm, nice, hotline, last_ };

    virtual ~Melody() = default;

    // This class shouldn't be created in heap
    // Use placement new instead
    // It allows us to reduce firmware memory about 3%
    static void* operator new(size_t) = delete;
    static void* operator new(size_t size, void* location) { return ::operator new(size, location); }

    static void operator delete(void*) {}

    [[nodiscard]] static Melody* getMelody(Name);
    [[nodiscard]] static const char* getMelodyName(Name);

    virtual void init() = 0;
    [[nodiscard]] virtual bool end() const = 0;
    [[nodiscard]] virtual bool tick() = 0;
};
