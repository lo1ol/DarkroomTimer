#include "DisplayAnimation.h"

#include "Hardware.h"
#include "Utils.h"

RenderedImgDesc DisplayAnimation::renderImg(const ImgDesc& img, uint8_t xOffset, uint8_t yOffset) {
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
                    customSym[inSymRow] = (img.img[imgRow] >> offset) & 0x1F;
                else
                    customSym[inSymRow] = (img.img[imgRow] << -offset) & 0x1F;

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

void DisplayAnimation::printRenderedImg(const RenderedImgDesc& desc, uint8_t col) {
    char symN = '\x80';
    for (uint8_t symId = 0; symId != desc.customSymsCnt; ++symId)
        gLcd.addFastCustomChar(symN++, desc.customSyms[symId]);

    char endStr[ARRAY_SIZE(desc.row0Str)];

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        auto rowStr = desc.row0Str;
        if (rowN == 1)
            rowStr = desc.row1Str;
        auto rowStrLen = strlen(rowStr);

        // print end of row str
        auto endLineLen = min(rowStrLen, static_cast<uint8_t>(16 - col));
        memcpy(endStr, rowStr, endLineLen);
        endStr[endLineLen] = 0;
        gLcd.setCursor(col, rowN);
        gLcd.print(endStr);

        // if something left -- print it at beginning
        if (endLineLen != rowStrLen) {
            gLcd.setCursor(0, rowN);
            gLcd.print(rowStr += endLineLen);
        }
    }
}

BounceAnimation::BounceAnimation(const ImgDesc* imgDesc, uint8_t xShift, uint8_t yShift)
    : m_imgDesc(imgDesc), kBaseXShift(xShift), kBaseYShift(yShift) {}

void BounceAnimation::tick() {
    auto col = m_xPos / 5;
    auto renderedImg = renderImg(*m_imgDesc, m_xPos % 5, m_yPos);

    gLcd.clear();
    printRenderedImg(renderedImg, col);

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
}

namespace {
// clang-format off
constexpr uint32_t gDvdImg[] = {
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
