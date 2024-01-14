#pragma once

#include <Stream.h>

class ShellClient : public Stream {
public:
    virtual bool active() = 0;

    virtual void quit() = 0;

    virtual bool endOfData() {
        return false;
    }

    virtual void clearEndOfData() {
    }

    virtual void onCommandEnd() {
    }
};
