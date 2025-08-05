#pragma once

class Melody {
public:
    virtual ~Melody() = default;

    enum Name { alarm, nice, hotline, last_ };

    [[nodiscard]] static Melody* getMelody(Name);
    [[nodiscard]] static const char* getMelodyName(Name);

    virtual void init() = 0;
    [[nodiscard]] virtual bool end() const = 0;
    [[nodiscard]] virtual bool tick() = 0;
};
