#include "DisplayAnimation.h"

#include "Hardware.h"
#include "Utils.h"

RenderedImgDesc DisplayAnimation::renderImg(const ImgDesc& img, uint8_t xOffset, uint8_t yOffset) {
    uint32_t imgPict[16];
    memcpy_P(imgPict, img.img, sizeof(uint32_t) * img.height);

    RenderedImgDesc res;

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        auto rowStr = res.row0Str;
        if (rowN == 1)
            rowStr = res.row1Str;
        uint8_t colN;

        for (colN = 0; colN != ARRAY_SIZE(res.row0Str) - 1; ++colN) {
            if (res.customSymsCnt == ARRAY_SIZE(res.customSyms))
                break;

            auto customSym = res.customSyms[res.customSymsCnt];

            bool isEmptySym = true;
            for (uint8_t inSymRow = 0; inSymRow != 8; ++inSymRow) {
                int8_t imgRow = inSymRow + rowN * 8 - yOffset;

                if (imgRow < 0 || imgRow >= img.height) {
                    customSym[inSymRow] = 0;
                    continue;
                }

                int8_t offset = img.width - (colN + 1) * 5 + xOffset;
                if (offset >= 0)
                    customSym[inSymRow] = (imgPict[imgRow] >> offset) & 0x1F;
                else
                    customSym[inSymRow] = (imgPict[imgRow] << -offset) & 0x1F;

                if (customSym[inSymRow] != 0)
                    isEmptySym = false;
            }

            if (isEmptySym) {
                rowStr[colN] = ' ';
            } else {
                rowStr[colN] = 0x80 + res.customSymsCnt;
                ++res.customSymsCnt;
            }
        }

        rowStr[colN] = '\0';
    }

    return res;
}

void DisplayAnimation::printRenderedImg(const RenderedImgDesc& desc, const uint8_t col) {
    char symN = '\x80';
    for (uint8_t symId = 0; symId != desc.customSymsCnt; ++symId)
        gLcd.fastAddCustomChar(symN++, desc.customSyms[symId]);

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        auto rowStr = desc.row0Str;
        if (rowN == 1)
            rowStr = desc.row1Str;

        bool needSetCursror = true;

        for (uint8_t i = 0; rowStr[i]; ++i) {
            if (col + i == 16)
                needSetCursror = true;

            if (rowStr[i] == ' ') {
                needSetCursror = true;
                continue;
            }

            if (needSetCursror)
                gLcd.setCursor((col + i) % 16, rowN);

            gLcd.print(rowStr[i]);
        }
    }
}

BounceAnimation::BounceAnimation(const ImgDesc* imgDesc, uint8_t xShift, uint8_t yShift)
    : m_imgDesc(imgDesc), kBaseXShift(xShift), kBaseYShift(yShift) {}

uint16_t BounceAnimation::tick() {
    auto col = m_xPos / 5;
    auto renderedImg = renderImg(*m_imgDesc, m_xPos % 5, m_yPos);

    gLcd.beginFastPrint();
    gLcd.clear();
    printRenderedImg(renderedImg, col);
    gLcd.endFastPrint();

    m_xPos += m_xShift;
    m_yPos += m_yShift;

    if (m_xPos <= 0) {
        m_xPos = 0;
        m_xShift = kBaseXShift;
    }

    if (m_xPos + m_imgDesc->width >= 80) {
        m_xPos = 80 - m_imgDesc->width;
        m_xShift = -kBaseXShift;
    }

    if (m_yPos <= 0) {
        m_yPos = 0;
        m_yShift = kBaseXShift;
    }

    if (m_yPos + m_imgDesc->height >= 16) {
        m_yPos = 16 - m_imgDesc->height;
        m_yShift = -kBaseYShift;
    }

    return 300;
}

namespace {
// clang-format off
constexpr uint32_t gDvdImg[] PROGMEM = {
    0b111011000111110,
    0b001101000100011,
    0b100101000101001,
    0b100100101001001,
    0b101100101001011,
    0b111000010001110,
    0b000000000000000,
    0b000111111111100,
    0b011111100111110,
    0b001111111111000
};
// clang-format on

constexpr ImgDesc gDvdImgDesc{
    .img = gDvdImg,
    .height = ARRAY_SIZE(gDvdImg),
    .width = 15,
};
} // namespace

DvdAnimation::DvdAnimation() : BounceAnimation(&gDvdImgDesc, 2, 1) {}

namespace {
constexpr uint8_t gRussianDickKickerWidth = 23;
constexpr uint8_t gRussianDickKickerHeight = 14;

constexpr uint8_t gRussianDickKickerFrameTime[7] = {
    20, 12, 7, 7, 7, 7, 15,
};

// clang-format off
constexpr uint32_t gRussianDickKickerFrames[7][gRussianDickKickerHeight] PROGMEM = {
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00111110000000000000000,
        0b01001001000000000000000,
        0b00001000000000000000000,
        0b00001000000000000000000,
        0b00001000011000000000000,
        0b00010100100100011000000,
        0b00010100010111110100000,
        0b00010100100111110100000,
        0b00010110011000011000000,
    },
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00011110000000000000000,
        0b00101001100000000000000,
        0b00101000000000000000000,
        0b00001000000000000000000,
        0b11110100011000000000000,
        0b10000100100100011000000,
        0b00000100010111110100000,
        0b00000100100111110100000,
        0b00000110011000011000000,
    },
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00001110000000000000000,
        0b00001001100000000000000,
        0b00001100000000000000000,
        0b00001010001100000000000,
        0b00001000010010001100000,
        0b00000110001011111010000,
        0b00000101010011111010000,
        0b00000100111100001100000,
        0b00000110000000000000000,
    },
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00001110000000000000000,
        0b00001001100000000000000,
        0b00001100000000000000000,
        0b00001010000011000000000,
        0b00001000000100100011000,
        0b00000110000010111110100,
        0b00000101000100111110100,
        0b00000100110011000011000,
        0b00000110000000000000000,
    },
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00011110000000000000000,
        0b00001001100000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00001000000001100000000,
        0b00010100000010010001100,
        0b00010100000001011111010,
        0b00010100000010011111010,
        0b00010110000001100001100,
    },
    {
        0b00001100000000000000000,
        0b00010010000000000000000,
        0b00010010000000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00011110000000000000000,
        0b00001001100000000000000,
        0b00001100000000000000000,
        0b00001000000000000000000,
        0b00001000000000110000000,
        0b00010100000001001000110,
        0b00010100000000101111101,
        0b00010100000001001111101,
        0b00010110000000110000110,
    },
    {
        0b00000011000000000000000,
        0b00000100100000000000000,
        0b00000100100000000000000,
        0b00000011000000000000000,
        0b00000010000000000000000,
        0b00001111100000000000000,
        0b00010010100000000000000,
        0b00000010000000000000000,
        0b00000010000000000000000,
        0b00000010000000110000000,
        0b00000101000001001000110,
        0b00000101000000101111101,
        0b00000101000001001111101,
        0b00000111100000110000110,
    },
};
// clang-format on

} // namespace

uint16_t RussianDickKicker::tick() {
    ImgDesc img{ .img = gRussianDickKickerFrames[m_currentFrame],
                 .height = gRussianDickKickerHeight,
                 .width = gRussianDickKickerWidth };

    auto renderedImg = renderImg(img, 0, 2);

    gLcd.beginFastPrint();
    gLcd.clear();
    gLcd.setCursor(5, 0);
    gLcd.print("Relax");
    printRenderedImg(renderedImg, m_xPos);
    gLcd.endFastPrint();

    uint16_t frameTime = gRussianDickKickerFrameTime[m_currentFrame] * 50UL;

    ++m_currentFrame;

    if (m_currentFrame == ARRAY_SIZE(gRussianDickKickerFrames)) {
        m_currentFrame = 0;
        ++m_xPos;
    }

    if (m_xPos == 16)
        m_xPos = 0;

    return frameTime;
}

DisplayAnimation* DisplayAnimation::createAnimation(Id id) {
    static uint8_t gAnimationBuf[sizeof(DvdAnimation)];

tryAgain:
    switch (id) {
    case random:
        id = static_cast<Id>(random + millis() % (last_ - random));
        goto tryAgain;
    case dvd:
        return new (gAnimationBuf) DvdAnimation();
    case dickKicker:
        return new (gAnimationBuf) RussianDickKicker();
    case last_:
    default:
        return nullptr;
    }
}

const __FlashStringHelper* DisplayAnimation::getAnimationName(Id id) {
    switch (id) {
    case random:
        return F("Surprise me");
    case dvd:
        return F("DVD");
    case dickKicker:
        return F("Dick Kicker");
    case last_:
    default:
        return nullptr;
    }
}
