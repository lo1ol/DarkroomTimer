#pragma once

#include <Arduino.h>
#include <stdint.h>

#if __has_include("new.h")
    #include <new.h>
#else
    #include <cstddef>
    #include <new>
#endif

struct ImgDesc {
    const uint32_t* img;
    uint8_t height;
    uint8_t width;
};

struct RenderedImgDesc {
    char row0Str[7];
    char row1Str[7];
    uint8_t customSymsCnt = 0;
    uint8_t customSyms[8][8];
};

class DisplayAnimation {
public:
    // return time to next update
    virtual uint16_t tick() = 0;

    // random has to be first
    enum Id { random, dvd, dickKicker, last_ };

    virtual ~DisplayAnimation() = default;

    // This class shouldn't be created in heap
    // Use placement new instead
    // It allows us to reduce firmware memory about 3%
    static void* operator new(size_t) = delete;
    static void* operator new(size_t size, void* location) { return ::operator new(size, location); }

    static void operator delete(void*) {}

    [[nodiscard]] static DisplayAnimation* createAnimation(Id);
    [[nodiscard]] static const __FlashStringHelper* getAnimationName(Id);

protected:
    static RenderedImgDesc renderImg(const ImgDesc& img, uint8_t xOffset, uint8_t yOffset);
    static void printRenderedImg(const RenderedImgDesc& desc, const int8_t col);
};

class BounceAnimation : public DisplayAnimation {
public:
    BounceAnimation(const ImgDesc* imgDesc, uint8_t xShift, uint8_t yShift);

    uint16_t tick() override;

private:
    int8_t m_xPos = 0;
    int8_t m_yPos = 0;

    int8_t m_xShift = 2;
    int8_t m_yShift = 1;
    const ImgDesc* m_imgDesc;
    const uint8_t kBaseXShift;
    const uint8_t kBaseYShift;
};

class DvdAnimation : public BounceAnimation {
public:
    DvdAnimation();
};

class RussianDickKicker : public DisplayAnimation {
public:
    uint16_t tick() override;

private:
    uint8_t m_currentFrame = 0;
    int8_t m_xPos = -4;
};
