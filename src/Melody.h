#pragma once

class Melody {
public:
    virtual ~Melody() {}

    enum Name { alarm, nice, last_ };

    static Melody* getMelody(Name);
    static const char* getMelodyName(Name);

    virtual void init() = 0;
    virtual bool end() const = 0;
    virtual bool tick() = 0;
};
