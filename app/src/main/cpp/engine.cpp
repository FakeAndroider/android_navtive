//
// Created by apple on 2025/6/12.
//

#include "engine.h"

int Engine::add(int addVal) {
    mValue += addVal;
    return mValue;
}

int Engine::output() const {
    return mValue;
}

