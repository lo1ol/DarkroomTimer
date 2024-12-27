#pragma once

#define EB_NO_FOR
#define EB_NO_CALLBACK
#define EB_NO_COUNTER
#define EB_NO_BUFFER
#define EB_DEB_TIME 50    // таймаут гашения дребезга кнопки (кнопка)
#define EB_CLICK_TIME 200 // таймаут ожидания кликов (кнопка)
#define EB_STEP_TIME 200  // таймаут импульсного удержания (кнопка)
#define EB_HOLD_TIME 500  // таймаут удержания (кнопка)
#define EB_FAST_TIME 15   // таймаут быстрого поворота (энкодер)

#include <EncButton.h>
