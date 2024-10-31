#include "ModeProcessor.h"

#include "Config.h"
#include "Tools.h"

void ModeProcessor::printTimeHelper(Time (*timeGetter)(const void* ctx, uint8_t id, bool& current, const char*& mark),
                                    const void* ctx) const {
    uint8_t id = 0;

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        while (true) {
            char str[DISPLAY_COLS + 1] = { 0 };
            bool current = false;
            bool printTimer = gTimer.state() == Timer::RUNNING;
            uint8_t alignSize = 0;
            const char* mark = nullptr;

            Time time = timeGetter(ctx, id, current, mark);

            if (time == kBadTime)
                return;

            time.getFormatedTime(str, current, current);

            if (current && printTimer) {
                alignSize = strlen(str);
                if (gTimer.lag()) {
                    strcpy(str, "Lag");
                } else {
                    time = Time::fromMillis(gTimer.left());
                    time.getFormatedTime(str, true, true);
                }
            }

            if (!gDisplay[row].tryPrint(str, current && !printTimer, alignSize, mark))
                break;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
