#pragma once

class Melody {
public:
    virtual ~Melody() {}

    enum Name { nice, last_ };

    static Melody* getMelody(Name);

    virtual void init() = 0;
    virtual bool end() const = 0;
    virtual bool tick() = 0;
};
