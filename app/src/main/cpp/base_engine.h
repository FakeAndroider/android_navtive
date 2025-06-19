//
// Created by apple on 2025/6/12.
//

#ifndef CPLUSPLUS_BASE_ENGINE_H
#define CPLUSPLUS_BASE_ENGINE_H

template<typename T>
class BaseEngine {
public:

    virtual T output() const = 0;
    virtual   ~BaseEngine() = default;
};


#endif //CPLUSPLUS_BASE_ENGINE_H
