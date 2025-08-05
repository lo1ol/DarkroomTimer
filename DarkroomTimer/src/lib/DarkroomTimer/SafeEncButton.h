#pragma once

#ifndef PIO_UNIT_TESTING

    #define EB_NO_FOR
    #define EB_NO_CALLBACK
    #define EB_NO_COUNTER
    #define EB_NO_BUFFER
    #define EB_DEB_TIME 50    // таймаут гашения дребезга кнопки (кнопка)
    #define EB_CLICK_TIME 200 // таймаут ожидания кликов (кнопка)
    #define EB_STEP_TIME 200  // таймаут импульсного удержания (кнопка)
    #define EB_HOLD_TIME 500  // таймаут удержания (кнопка)
    #define EB_FAST_TIME 13   // таймаут быстрого поворота (энкодер)

    #include <EncButton.h>
#else

// classes for tests
template<int>
class ButtonT {
public:
    void emulPress() { m_reqPress = true; }
    void emulRelease() { m_reqRelease = true; }
    void emulHold() { m_reqHold = true; }
    void emulClick() { m_reqClick = true; }

    bool pressing() const { return m_pressing; }
    bool press() const { return m_justPress; }
    bool holding() const { return m_holding; }
    bool hold() const { return m_justHolded; }
    bool click() const { return m_justClicked; }
    void skipEvents() {
        m_justClicked = false;
        m_justHolded = false;
        m_justPress = false;
    }

    void tick() {
        skipEvents();

        if (m_reqPress) {
            if (!m_pressing)
                m_justPress = true;
            m_pressing = true;
        }

        if (m_reqClick) {
            if (!m_pressing)
                m_justPress = true;
            m_justClicked = true;
            m_justHolded = false;
            m_pressing = false;
            m_holding = false;
        }

        if (m_reqHold) {
            m_justHolded = true;
            m_justPress = false;
            m_holding = true;
            m_pressing = true;
        }

        if (m_reqRelease) {
            m_pressing = false;
            m_holding = false;
            m_justPress = false;
        }

        m_reqPress = false;
        m_reqRelease = false;
        m_reqClick = false;
        m_reqHold = false;
    }

private:
    bool m_pressing = false;
    bool m_holding = false;
    bool m_justPress = false;
    bool m_justClicked = false;
    bool m_justHolded = false;

    bool m_reqPress;
    bool m_reqRelease;
    bool m_reqClick;
    bool m_reqHold;
};

// classes for tests
class VirtButton {
public:
    bool pressing() const { return m_pressing; }
    bool hold() const { return m_justHolded; }
    bool click() const { return m_justClicked; }

    template<class T1, class T2>
    void tick(T1& t1, T2& t2) {
        m_justHolded = false;
        m_justClicked = false;

        if (t1.pressing() && t2.pressing())
            m_pressing = true;
        else
            m_pressing = false;

        if (t1.click() && t2.click()) {
            m_justClicked = true;
            m_justHolded = false;
            m_pressing = true;
        }

        if (t1.hold() && t2.hold()) {
            m_justHolded = true;
            m_justClicked = false;
            m_pressing = true;
        }
    }

private:
    bool m_pressing = false;
    bool m_justHolded = false;
    bool m_justClicked = false;
};

#endif
