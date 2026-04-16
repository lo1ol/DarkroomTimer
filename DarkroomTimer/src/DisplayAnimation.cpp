#include "DisplayAnimation.h"

#include "Hardware.h"
#include "Utils.h"

#ifdef PIO_UNIT_TESTING
    #define micros gMillis
#endif

#define BIT_ARRAY1(a) \
    { (((a) >> 0) & 0xFF) }
#define BIT_ARRAY2(a) \
    { (((a) >> 0) & 0xFF), (((a) >> 8) & 0xFF) }
#define BIT_ARRAY3(a) \
    { (((a) >> 0) & 0xFF), (((a) >> 8) & 0xFF), (((a) >> 16) & 0xFF) }
#define BIT_ARRAY4(a) \
    { (((a) >> 0) & 0xFF), (((a) >> 8) & 0xFF), (((a) >> 16) & 0xFF), (((a) >> 24) & 0xFF) }

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

#ifdef PIO_UNIT_TESTING
    #define memcpy_P memcpy
#endif
                memcpy_P(&imgRowPixels, img.img + imgRow * img.bytesInRow, img.bytesInRow);
#undef memcpy_P

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

    m_xPos += m_xShift;
    m_yPos += m_yShift;

    if (m_xPos <= 0) {
        m_xPos = 0;
        m_xShift = kBaseXShift;
    }

    if (m_xPos + m_imgDesc->bytesInRow * 8 >= 80) {
        m_xPos = 80 - m_imgDesc->bytesInRow * 8;
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
constexpr uint8_t gDvdImg[][2] PROGMEM = {
    BIT_ARRAY2(0b1110110001111100),
    BIT_ARRAY2(0b1110110001111100),
    BIT_ARRAY2(0b0011010001000110),
    BIT_ARRAY2(0b1001010001010010),
    BIT_ARRAY2(0b1001001010010010),
    BIT_ARRAY2(0b1011001010010110),
    BIT_ARRAY2(0b1110000100011100),
    BIT_ARRAY2(0b0000000000000000),
    BIT_ARRAY2(0b0001111111111000),
    BIT_ARRAY2(0b0111111001111100),
    BIT_ARRAY2(0b0011111111110000),
};
// clang-format on

ImgDesc gDvdImgDesc{
    .img = reinterpret_cast<const uint8_t*>(gDvdImg),
    .height = ARRAY_SIZE(gDvdImg),
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

// clang-format off
constexpr uint8_t gRussianDickKickerFrames[][gRussianDickKickerHeight][gRussianDickKickerBytesInRow] PROGMEM = {
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b001111100000000000000000),
        BIT_ARRAY3(0b010010010000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000010000110000000000000),
        BIT_ARRAY3(0b000101001001000110000000),
        BIT_ARRAY3(0b000101000101111101000000),
        BIT_ARRAY3(0b000101001001111101000000),
        BIT_ARRAY3(0b000101100110000110000000),
    },
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000111100000000000000000),
        BIT_ARRAY3(0b001010011000000000000000),
        BIT_ARRAY3(0b001010000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b111101000110000000000000),
        BIT_ARRAY3(0b100001001001000110000000),
        BIT_ARRAY3(0b000001000101111101000000),
        BIT_ARRAY3(0b000001001001111101000000),
        BIT_ARRAY3(0b000001100110000110000000),
    },
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000011100000000000000000),
        BIT_ARRAY3(0b000010011000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010100011000000000000),
        BIT_ARRAY3(0b000010000100100011000000),
        BIT_ARRAY3(0b000001100010111110100000),
        BIT_ARRAY3(0b000001010100111110100000),
        BIT_ARRAY3(0b000001001111000011000000),
        BIT_ARRAY3(0b000001100000000000000000),
    },
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000011100000000000000000),
        BIT_ARRAY3(0b000010011000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010100000110000000000),
        BIT_ARRAY3(0b000010000001001000110000),
        BIT_ARRAY3(0b000001100000101111101000),
        BIT_ARRAY3(0b000001010001001111101000),
        BIT_ARRAY3(0b000001001100110000110000),
        BIT_ARRAY3(0b000001100000000000000000),
    },
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000111100000000000000000),
        BIT_ARRAY3(0b000010011000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000010000000011000000000),
        BIT_ARRAY3(0b000101000000100100011000),
        BIT_ARRAY3(0b000101000000010111110100),
        BIT_ARRAY3(0b000101000000100111110100),
        BIT_ARRAY3(0b000101100000011000011000),
    },
    {
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000100100000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000111100000000000000000),
        BIT_ARRAY3(0b000010011000000000000000),
        BIT_ARRAY3(0b000011000000000000000000),
        BIT_ARRAY3(0b000010000000000000000000),
        BIT_ARRAY3(0b000010000000001100000000),
        BIT_ARRAY3(0b000101000000010010001100),
        BIT_ARRAY3(0b000101000000001011111010),
        BIT_ARRAY3(0b000101000000010011111010),
        BIT_ARRAY3(0b000101100000001100001100),
    },
    {
        BIT_ARRAY3(0b000000110000000000000000),
        BIT_ARRAY3(0b000001001000000000000000),
        BIT_ARRAY3(0b000001001000000000000000),
        BIT_ARRAY3(0b000000110000000000000000),
        BIT_ARRAY3(0b000000100000000000000000),
        BIT_ARRAY3(0b000011111000000000000000),
        BIT_ARRAY3(0b000100101000000000000000),
        BIT_ARRAY3(0b000000100000000000000000),
        BIT_ARRAY3(0b000000100000000000000000),
        BIT_ARRAY3(0b000000100000001100000000),
        BIT_ARRAY3(0b000001010000010010001100),
        BIT_ARRAY3(0b000001010000001011111010),
        BIT_ARRAY3(0b000001010000010011111010),
        BIT_ARRAY3(0b000001111000001100001100),
    },
};
// clang-format on

} // namespace

uint16_t RussianDickKicker::tick() {
    ImgDesc img{ .img = reinterpret_cast<const uint8_t*>(gRussianDickKickerFrames[m_currentFrame]),
                 .height = gRussianDickKickerHeight,
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
    m_inPhaseCnt = 0;
}

uint16_t SleepyTimer::tick() {
    gLcd.beginFastPrint();

    gLcd.clear();
    setPhaseChars();

    ++m_inPhaseCnt;
    uint16_t entropy = micros() + gAnalogRead(A6);

    switch (m_phase) {
    case WakedUp:
        gLcd.setCursor(5, 0);
        switch (entropy % 5) {
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

        if (m_inPhaseCnt == 15)
            nextPhase();
        break;
    case GoingToSleep:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseCnt) {
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
        switch (m_inPhaseCnt) {
        case 1:
        case 4:
            gLcd.print(kYawnMouse);
            break;
        default:
            gLcd.print(kYawnClosedMouse);
        }

        if (m_inPhaseCnt == 6)
            nextPhase();
        break;
    case Sleep:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseCnt % 2) {
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

        switch (m_inPhaseCnt % 4) {
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

        if (m_inPhaseCnt == 100)
            nextPhase();
        break;
    case WakingUp:
        gLcd.setCursor(5, 0);
        switch (m_inPhaseCnt) {
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
        switch (m_inPhaseCnt) {
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

        if (m_inPhaseCnt == 8)
            nextPhase();
        break;
    case last_:
        break;
    }

    gLcd.endFastPrint();

    return 1000;
}

DisplayAnimation* DisplayAnimation::createAnimation(Id id) {
    static uint8_t gAnimationBuf[sizeof(DvdAnimation)];

tryAgain:
    switch (id) {
    case random:
        id = static_cast<Id>(random + micros() % (last_ - random));
        goto tryAgain;
    case dvd:
        return new (gAnimationBuf) DvdAnimation();
    case dickKicker:
        return new (gAnimationBuf) RussianDickKicker();
    case sleepyTimer:
        return new (gAnimationBuf) SleepyTimer();
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
    case last_:
    default:
        return nullptr;
    }
}
