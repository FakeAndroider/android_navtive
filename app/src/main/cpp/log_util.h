//
// Created by apple on 2025/6/18.
//

#ifndef CPLUSPLUS_LOG_UTIL_H
#define CPLUSPLUS_LOG_UTIL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>

#include <errno.h>

#define LOG_TAG "--->engine"

static inline void ALOGE(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, fmt, args);
    va_end(args);
}


static inline void ALOGW(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, fmt, args);
    va_end(args);
}



#endif //CPLUSPLUS_LOG_UTIL_H
