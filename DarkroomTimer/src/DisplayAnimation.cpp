#include "DisplayAnimation.h"

#include "Hardware.h"
#include "Utils.h"

#ifdef PIO_UNIT_TESTING
    #define micros gMillis
#endif

#define BIT_ARRAY1(a) \
    { (((0b##a) >> 0) & 0xFF) }
#define BIT_ARRAY2(a) \
    { (((0b##a) >> 0) & 0xFF), (((0b##a) >> 8) & 0xFF) }
#define BIT_ARRAY3(a) \
    { (((0b##a) >> 0) & 0xFF), (((0b##a) >> 8) & 0xFF), (((0b##a) >> 16) & 0xFF) }
#define BIT_ARRAY4(a) \
    { (((0b##a) >> 0) & 0xFF), (((0b##a) >> 8) & 0xFF), (((0b##a) >> 16) & 0xFF), (((0b##a) >> 24) & 0xFF) }

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

                uint32_t imgRowPixels = 0;
                memcpy_P(&imgRowPixels, img.img + imgRow * img.bytesInRow, img.bytesInRow);

                int8_t offset = img.bytesInRow * 8 - (colN + 1) * 5 + xOffset;
                if (offset >= 0)
                    customSym[inSymRow] = (imgRowPixels >> offset) & 0x1F;
                else
                    customSym[inSymRow] = (imgRowPixels << -offset) & 0x1F;

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

void DisplayAnimation::printRenderedImg(const RenderedImgDesc& desc, const int8_t col) {
    char symN = '\x80';
    for (uint8_t symId = 0; symId != desc.customSymsCnt; ++symId)
        gLcd.addRAMCustomChar(symN++, desc.customSyms[symId]);

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        auto rowStr = desc.row0Str;
        if (rowN == 1)
            rowStr = desc.row1Str;

        bool needSetCursror = true;

        for (uint8_t i = 0; rowStr[i]; ++i) {
            if (col + i < 0)
                continue;

            if (col + i > 16)
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

    auto xShift = m_xShift;

    // to make animation more random
    uint32_t entropy = genRandom();
    if (entropy % 150 == 0) {
        if (m_xShift > 0)
            xShift = 1 + m_xShift % 2;
        else
            xShift = -1 - (-m_xShift) % 2;
    }

    m_xPos += xShift;
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
        m_yShift = kBaseYShift;
    }

    if (m_yPos + m_imgDesc->height >= 16) {
        m_yPos = 16 - m_imgDesc->height;
        m_yShift = -kBaseYShift;
    }

    return 500;
}

namespace {
// clang-format off
constexpr uint8_t gDvdImg[][2] PROGMEM = {
    BIT_ARRAY2(1110110001111100),
    BIT_ARRAY2(1110110001111100),
    BIT_ARRAY2(0011010001000110),
    BIT_ARRAY2(1001010001010010),
    BIT_ARRAY2(1001001010010010),
    BIT_ARRAY2(1011001010010110),
    BIT_ARRAY2(1110000100011100),
    BIT_ARRAY2(0000000000000000),
    BIT_ARRAY2(0001111111111000),
    BIT_ARRAY2(0111111001111100),
    BIT_ARRAY2(0011111111110000),
};
// clang-format on

ImgDesc gDvdImgDesc{
    .img = reinterpret_cast<const uint8_t*>(gDvdImg),
    .height = ARRAY_SIZE(gDvdImg),
    .width = 15,
    .bytesInRow = 2,
};
} // namespace

DvdAnimation::DvdAnimation() : BounceAnimation(&gDvdImgDesc, 2, 1) {}

namespace {
constexpr uint8_t gRussianDickKickerBytesInRow = 3;
constexpr uint8_t gRussianDickKickerHeight = 14;

constexpr uint8_t gRussianDickKickerFrameTime[7] = {
    20, 12, 7, 7, 7, 7, 15,
};

// TODO store penis and kicker separately

// clang-format off
constexpr uint8_t gRussianDickKickerFrames[][gRussianDickKickerHeight][gRussianDickKickerBytesInRow] PROGMEM = {
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(001111100000000000000000),
        BIT_ARRAY3(010010010000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000010000110000000000000),
        BIT_ARRAY3(000101001001000110000000),
        BIT_ARRAY3(000101000101111101000000),
        BIT_ARRAY3(000101001001111101000000),
        BIT_ARRAY3(000101100110000110000000),
    },
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000111100000000000000000),
        BIT_ARRAY3(001010011000000000000000),
        BIT_ARRAY3(001010000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(111101000110000000000000),
        BIT_ARRAY3(100001001001000110000000),
        BIT_ARRAY3(000001000101111101000000),
        BIT_ARRAY3(000001001001111101000000),
        BIT_ARRAY3(000001100110000110000000),
    },
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000011100000000000000000),
        BIT_ARRAY3(000010011000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010100011000000000000),
        BIT_ARRAY3(000010000100100011000000),
        BIT_ARRAY3(000001100010111110100000),
        BIT_ARRAY3(000001010100111110100000),
        BIT_ARRAY3(000001001111000011000000),
        BIT_ARRAY3(000001100000000000000000),
    },
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000011100000000000000000),
        BIT_ARRAY3(000010011000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010100000110000000000),
        BIT_ARRAY3(000010000001001000110000),
        BIT_ARRAY3(000001100000101111101000),
        BIT_ARRAY3(000001010001001111101000),
        BIT_ARRAY3(000001001100110000110000),
        BIT_ARRAY3(000001100000000000000000),
    },
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000111100000000000000000),
        BIT_ARRAY3(000010011000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000010000000011000000000),
        BIT_ARRAY3(000101000000100100011000),
        BIT_ARRAY3(000101000000010111110100),
        BIT_ARRAY3(000101000000100111110100),
        BIT_ARRAY3(000101100000011000011000),
    },
    {
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000100100000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000111100000000000000000),
        BIT_ARRAY3(000010011000000000000000),
        BIT_ARRAY3(000011000000000000000000),
        BIT_ARRAY3(000010000000000000000000),
        BIT_ARRAY3(000010000000001100000000),
        BIT_ARRAY3(000101000000010010001100),
        BIT_ARRAY3(000101000000001011111010),
        BIT_ARRAY3(000101000000010011111010),
        BIT_ARRAY3(000101100000001100001100),
    },
    {
        BIT_ARRAY3(000000110000000000000000),
        BIT_ARRAY3(000001001000000000000000),
        BIT_ARRAY3(000001001000000000000000),
        BIT_ARRAY3(000000110000000000000000),
        BIT_ARRAY3(000000100000000000000000),
        BIT_ARRAY3(000011111000000000000000),
        BIT_ARRAY3(000100101000000000000000),
        BIT_ARRAY3(000000100000000000000000),
        BIT_ARRAY3(000000100000000000000000),
        BIT_ARRAY3(000000100000001100000000),
        BIT_ARRAY3(000001010000010010001100),
        BIT_ARRAY3(000001010000001011111010),
        BIT_ARRAY3(000001010000010011111010),
        BIT_ARRAY3(000001111000001100001100),
    },
};
// clang-format on

} // namespace

uint16_t RussianDickKicker::tick() {
    ImgDesc img{ .img = reinterpret_cast<const uint8_t*>(gRussianDickKickerFrames[m_currentFrame]),
                 .height = gRussianDickKickerHeight,
                 .width = 23,
                 .bytesInRow = gRussianDickKickerBytesInRow };

    auto renderedImg = renderImg(img, 2, 2);

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

namespace {
// clang-format off
constexpr uint8_t kNosePic[] PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01010, 0b00000
};
constexpr uint8_t kStraightEyePic[] PROGMEM = {
    0b01110, 0b10001, 0b10001, 0b10101, 0b10001, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kRightEyePic[] PROGMEM = {
    0b01110, 0b10001, 0b10001, 0b10000, 0b10010, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kLeftEyePic[] PROGMEM = {
    0b01110, 0b10001, 0b10001, 0b00001, 0b01001, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kPreclosedEyePic[] PROGMEM = {
    0b00000, 0b00000, 0b01110, 0b10001, 0b10101, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kClosedEye1Pic[] PROGMEM = {
    0b00000, 0b00000, 0b01110, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kClosedEye2Pic[] PROGMEM = {
    0b00000, 0b01110, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kSleepyEyePic[] PROGMEM = {
    0b00000, 0b00100, 0b01010, 0b10001, 0b00000, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kSmileMousePic[] PROGMEM = {
    0b00000, 0b00000, 0b10001, 0b01110, 0b00000, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kTongueMousePic[] PROGMEM = {
    0b00000, 0b00000, 0b10001, 0b01110, 0b01000, 0b10001, 0b11001, 0b01110
};
constexpr uint8_t kYawnMousePic[] PROGMEM = {
    0b00000, 0b01110, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kYawnClosedMousePic[] PROGMEM = {
    0b00000, 0b00000, 0b01110, 0b10001, 0b01110, 0b00000, 0b00000, 0b00000
};
// clang-format on

#define kNose "\x80"
#define kStreightEye "\x81"
#define kSmileMouse "\x82"
#define kYawnMouse "\x83"
#define kYawnClosedMouse "\x84"

#define kRightEye "\x85"
#define kLeftEye "\x86"
#define kTongueMouse "\x87"

#define kPreclosedEye "\x85"
#define kClosedEye1 "\x86"
#define kClosedEye2 "\x87"

#define kSleepyEye "\x85"
#define kSleepyOpenEye "\x86"

} // namespace

void SleepyTimer::setPhaseChars() {
    gLcd.addPROGMEMCustomChar(kNose[0], kNosePic);
    gLcd.addPROGMEMCustomChar(kStreightEye[0], kStraightEyePic);
    gLcd.addPROGMEMCustomChar(kSmileMouse[0], kSmileMousePic);
    gLcd.addPROGMEMCustomChar(kYawnMouse[0], kYawnMousePic);
    gLcd.addPROGMEMCustomChar(kYawnClosedMouse[0], kYawnClosedMousePic);

    switch (m_phase) {
    case WakedUp:
        gLcd.addPROGMEMCustomChar(kRightEye[0], kRightEyePic);
        gLcd.addPROGMEMCustomChar(kLeftEye[0], kLeftEyePic);
        gLcd.addPROGMEMCustomChar(kTongueMouse[0], kTongueMousePic);
        break;
    case GoingToSleep:
    case WakingUp:
        gLcd.addPROGMEMCustomChar(kPreclosedEye[0], kPreclosedEyePic);
        gLcd.addPROGMEMCustomChar(kClosedEye1[0], kClosedEye1Pic);
        gLcd.addPROGMEMCustomChar(kClosedEye2[0], kClosedEye2Pic);
        break;
    case Sleep:
        gLcd.addPROGMEMCustomChar(kSleepyEye[0], kSleepyEyePic);
        gLcd.addPROGMEMCustomChar(kSleepyOpenEye[0], kPreclosedEyePic);
        break;
    default:;
    }
}

void SleepyTimer::nextPhase() {
    m_phase = ADD_TO_ENUM(Phase, m_phase, 1);
    m_inPhaseTime = 0;
}

uint16_t SleepyTimer::tick() {
    gLcd.beginFastPrint();

    gLcd.clear();
    setPhaseChars();

    ++m_inPhaseTime;
    uint32_t entropy = genRandom();

    switch (m_phase) {
    case WakedUp:
        gLcd.setCursor(5, 0);
        switch (entropy % 4) {
        case 0:
            gLcd.print(kRightEye " " kNose " " kRightEye);
            break;
        case 1:
            gLcd.print(kLeftEye " " kNose " " kLeftEye);
            break;
        default:
            gLcd.print(kStreightEye " " kNose " " kStreightEye);
        }

        gLcd.setCursor(7, 1);
        gLcd.print(kSmileMouse);

        if (entropy % 21 == 0) {
            gLcd.setCursor(5, 0);
            gLcd.print(kLeftEye " " kNose " " kRightEye);
            gLcd.setCursor(7, 1);
            gLcd.print(kTongueMouse);
        }

        if (m_inPhaseTime == 15)
            nextPhase();
        break;
    case GoingToSleep:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseTime) {
        case 1:
            gLcd.print(kStreightEye " " kNose " " kPreclosedEye);
            break;
        case 2:
            gLcd.print(kPreclosedEye " " kNose " " kPreclosedEye);
            break;
        case 3:
        case 4:
            gLcd.print(kClosedEye1 " " kNose " " kClosedEye1);
            break;
        case 5:
        case 6:
        default:
            gLcd.print(kClosedEye2 " " kNose " " kClosedEye2);
            break;
        }

        gLcd.setCursor(7, 1);
        switch (m_inPhaseTime) {
        case 1:
        case 4:
            gLcd.print(kYawnMouse);
            break;
        case 6:
            gLcd.print(kSmileMouse);
            break;
        default:
            gLcd.print(kYawnClosedMouse);
        }

        if (m_inPhaseTime == 6)
            nextPhase();
        break;
    case Sleep:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseTime % 2) {
        case 0:
            gLcd.print("^ " kNose " ^");
            break;
        default:
            gLcd.print(kSleepyEye " " kNose " " kSleepyEye);
            break;
        }

        if (entropy % 31 == 0) {
            gLcd.setCursor(5, 0);
            gLcd.print(kSleepyOpenEye);
        }

        if (entropy % 30 == 0) {
            gLcd.setCursor(9, 0);
            gLcd.print(kSleepyOpenEye);
        }

        gLcd.setCursor(7, 1);
        switch (entropy % 8) {
        case 0:
            gLcd.print(kSmileMouse);
            break;
        case 1:
        case 2:
        case 3:
            gLcd.print(kYawnMouse);
            break;
        case 4:
        case 5:
        case 6:
            gLcd.print(kYawnClosedMouse);
            break;
        default:
            gLcd.print("o");
            break;
        }

        switch (m_inPhaseTime % 4) {
        case 0:
            gLcd.setCursor(10, 0);
            gLcd.print(" z Z");
            gLcd.setCursor(10, 1);
            gLcd.print("z");
            break;
        case 1:
            gLcd.setCursor(10, 0);
            gLcd.print("  z ");
            gLcd.setCursor(10, 1);
            gLcd.print(" z");
            break;
        case 2:
            gLcd.setCursor(10, 0);
            gLcd.print("    Z");
            gLcd.setCursor(10, 1);
            gLcd.print("z z");
            break;
        default:
            gLcd.setCursor(10, 0);
            gLcd.print("    z");
            gLcd.setCursor(10, 1);
            gLcd.print(" z");
            break;
        }

        if (m_inPhaseTime == 100)
            nextPhase();
        break;
    case WakingUp:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseTime) {
        case 1:
            gLcd.print(kClosedEye2 " " kNose " " kClosedEye2);
            break;
        case 2:
        case 4:
        case 5:
        case 7:
            gLcd.print(kClosedEye1 " " kNose " " kClosedEye1);
            break;
        case 3:
            gLcd.print(kClosedEye1 " " kNose " " kPreclosedEye);
            break;
        case 6:
            gLcd.print(kPreclosedEye " " kNose " " kClosedEye1);
            break;
        case 8:
        default:
            gLcd.print(kPreclosedEye " " kNose " " kPreclosedEye);
            break;
        }

        gLcd.setCursor(7, 1);
        switch (m_inPhaseTime) {
        case 1:
        case 8:
            gLcd.print(kSmileMouse);
            break;
        case 2:
        case 4:
        case 7:
            gLcd.print(kYawnMouse);
            break;
        default:
            gLcd.print(kYawnClosedMouse);
        }

        if (m_inPhaseTime == 8)
            nextPhase();
        break;
    case last_:
        break;
    }

    gLcd.endFastPrint();

    return 1000;
}

namespace {
constexpr uint8_t gSpaceShipImg[][2] PROGMEM = {
    BIT_ARRAY2(1111000000000000), BIT_ARRAY2(0001010000000000), BIT_ARRAY2(0011111000000000),
    BIT_ARRAY2(0011101100000000), BIT_ARRAY2(1111110011000000), BIT_ARRAY2(0000001100000000),
};

ImgDesc gSpaceShipImgDesc{
    .img = reinterpret_cast<const uint8_t*>(gSpaceShipImg),
    .height = ARRAY_SIZE(gSpaceShipImg),
    .width = 10,
    .bytesInRow = 2,
};

// clang-format off
constexpr uint8_t kStar1Pic[] PROGMEM = {
    0b00000, 0b00010, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000
};
constexpr uint8_t kStar2Pic[] PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b00000, 0b00000
};
constexpr uint8_t kTwoStarsPic[] PROGMEM = {
    0b00000, 0b00000, 0b00001, 0b00000, 0b00000, 0b00000, 0b01000, 0b00000
};

constexpr uint8_t kMiddleStarPic[] PROGMEM = {
   0b00000, 0b00000, 0b00100, 0b01010, 0b00100, 0b00000, 0b00000, 0b00000
};
// clang-format on

#define kNothing ' '
#define kStar1 '\x84'
#define kStar2 '\x85'
#define kTwoStars '\x86'
#define kMiddleStar '.'
#define kBigStar '\x87'
#define kHugeStar '*'
#define kPlanet 'o'
#define kGalactic 's'
#define kEnemy '\x7e'
#define kSatellite '%'

} // namespace

SpaceShip::SpaceShip() {
    gLcd.addPROGMEMCustomChar(kStar1, kStar1Pic);
    gLcd.addPROGMEMCustomChar(kStar2, kStar2Pic);
    gLcd.addPROGMEMCustomChar(kTwoStars, kTwoStarsPic);
    gLcd.addPROGMEMCustomChar(kBigStar, kMiddleStarPic);

    // init space objects
    for (uint8_t i = 0; i != 16; ++i)
        shiftSpace();
}

void SpaceShip::shiftSpace() {
    m_phase++;

    if (m_lastBigObjectDistance < 16)
        m_lastBigObjectDistance++;

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        uint64_t* row = &m_r0spaceObjects;
        if (rowN == 1)
            row = &m_r1spaceObjects;

        SpaceObject prevObject = static_cast<SpaceObject>((*row >> 60) & 0x0F);
        *row >>= 4;

tryAgain:
        uint32_t entropy = genRandom() - m_phase;
        SpaceObject object = SpaceObject::Nothing;
        bool bigObject = false;

        if (entropy % 3 == 0) {
            object = SpaceObject::Star1;
        }

        if (entropy % 3 == 1) {
            object = SpaceObject::Star2;
        }

        if (entropy % 23 == 0) {
            object = SpaceObject::TwoStars;
        }

        if (entropy % 37 == 0) {
            object = SpaceObject::MiddleStar;
        }

        if (entropy % 83 == 0) {
            object = SpaceObject::BigStar;
            bigObject = true;
        }
        if (entropy % 307 == 0) {
            object = SpaceObject::HugeStar;
            bigObject = true;
        }

        if (entropy % 311 == 0) {
            object = SpaceObject::Planet;
            bigObject = true;
        }

        if (entropy % 313 == 0) {
            object = SpaceObject::Galactic;
            bigObject = true;
        }

        if (entropy % 397 == 0) {
            object = SpaceObject::Enemy;
            bigObject = true;
        }

        if (entropy % 401 == 0) {
            object = SpaceObject::Satellite;
            bigObject = true;
        }

        if (bigObject && m_lastBigObjectDistance < 8)
            goto tryAgain;

        // to prevent row of same objects
        if (object != SpaceObject::Nothing && object == prevObject)
            goto tryAgain;

        if (bigObject)
            m_lastBigObjectDistance = 0;

        *row |= static_cast<uint64_t>(object) << 60;
    }
}

char SpaceShip::getSpaceObjectSym(SpaceObject obj) {
    switch (obj) {
    case Nothing:
        return kNothing;
    case Star1:
        return kStar1;
    case Star2:
        return kStar2;
    case TwoStars:
        return kTwoStars;
    case MiddleStar:
        return kMiddleStar;
    case BigStar:
        return kBigStar;
    case HugeStar:
        return kHugeStar;
    case Planet:
        return kPlanet;
    case Galactic:
        return kGalactic;
    case Enemy:
        return kEnemy;
    case Satellite:
        return kSatellite;
    }

    return ' ';
}

void SpaceShip::shiftSpaceShip() {
    auto entropy = genRandom();
    switch (m_spaceShipDir) {
    case 0:
        if (entropy % 7)
            break; // keep going
        if (genRandom() % 2)
            m_spaceShipDir = 1;
        else
            m_spaceShipDir = -1;
        break;
    case 1:
    case -1:
        if (entropy % 7 == 0)
            m_spaceShipDir = 0;
        break;
    }

    m_spaceshipYPos += m_spaceShipDir;
    if (m_spaceshipYPos < 0) {
        m_spaceshipYPos = 0;
        m_spaceShipDir = 0;
    }

    if (m_spaceshipYPos + gSpaceShipImgDesc.height >= 16) {
        m_spaceshipYPos = 16 - gSpaceShipImgDesc.height;
        m_spaceShipDir = 0;
    }
}

uint16_t SpaceShip::tick() {
    shiftSpace();
    shiftSpaceShip();

    auto renderedSpaceShip = renderImg(gSpaceShipImgDesc, 0, m_spaceshipYPos);

    gLcd.beginFastPrint();
    gLcd.clear();

    for (uint8_t rowN = 0; rowN != 2; ++rowN) {
        uint64_t row;
        if (rowN == 0)
            row = m_r0spaceObjects;
        else
            row = m_r1spaceObjects;

        gLcd.setCursor(0, rowN);
        for (uint8_t colN = 0; colN != 16; ++colN)
            gLcd.print(getSpaceObjectSym(static_cast<SpaceObject>((row >> colN * 4) & 0x0F)));
    }

    printRenderedImg(renderedSpaceShip, 3);
    gLcd.endFastPrint();

    return 300;
}

DisplayAnimation::DisplayAnimation() {
    reinitRandomGenSeed(); // we need some random at some anims
}

DisplayAnimation* DisplayAnimation::createAnimation(Id id) {
    static uint8_t gAnimationBuf[sizeof(SpaceShip)];

tryAgain:
    switch (id) {
    case random:
        id = static_cast<Id>(1 + (genRandom() % (last_ - 1)));
        goto tryAgain;
    case dvd:
        return new (gAnimationBuf) DvdAnimation();
    case dickKicker:
        return new (gAnimationBuf) RussianDickKicker();
    case sleepyTimer:
        return new (gAnimationBuf) SleepyTimer();
    case spaceship:
        return new (gAnimationBuf) SpaceShip();
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
    case sleepyTimer:
        return F("Sleepy");
    case spaceship:
        return F("Spaceship");
    case last_:
    default:
        return nullptr;
    }
}
