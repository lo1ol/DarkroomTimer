#pragma once

#include <stdint.h>

struct ImgDesc {
    const uint32_t* img;
    uint8_t height;
    uint8_t width;
};

struct RenderedImgDesc {
    char row0Str[7];
    char row1Str[7];
    uint8_t customSyms[8][8];
    uint8_t customSymsCnt = 0;
};

class DisplayAnimation {
public:
    virtual void tick() = 0;

protected:
    static RenderedImgDesc renderImg(const ImgDesc& img, uint8_t xOffset, uint8_t yOffset);
    static void printRenderedImg(const RenderedImgDesc& desc, uint8_t col);
};

class BounceAnimation : public DisplayAnimation {
public:
    BounceAnimation(const ImgDesc* imgDesc, uint8_t xShift, uint8_t yShift);

    void tick() override;

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
