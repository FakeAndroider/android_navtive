//
// Created by apple on 2025/6/12.
//

#ifndef CPLUSPLUS_ENGINE_H
#define CPLUSPLUS_ENGINE_H

#include "base_engine.h"

class Engine : public BaseEngine<int> {
public:
    explicit Engine(int val) : mValue(val) {};

    int add(int addVal);

    int output() const override;

private:
    int mValue;

};


#endif //CPLUSPLUS_ENGINE_H
