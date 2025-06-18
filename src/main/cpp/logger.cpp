//
// Created by apple on 2025/6/18.
//

#include <sys/socket.h>

#include "logger.h"
#include <Android/log.h>

#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

#define LOG_TAG "--->engine"
int Logger::s_ipc_fd_parent = -1;
int Logger::s_ipc_fd_child = -1;
bool  Logger::s_isParent = true;
pthread_t  Logger::s_logThread;
bool  Logger::s_threadRunning = false;


void Logger::init(bool isParent) {
    s_isParent = isParent;

    int sv[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        __android_log_print(levelToAndroidLog(ERROR), LOG_TAG,
                            "socketpair failed:%s", strerror(errno));

        exit(EXIT_FAILURE);
    }


    if (s_isParent) {
        s_ipc_fd_parent = sv[0];
        s_ipc_fd_child = sv[1];
    }

}

void Logger::log(Logger::LogLevel level, const char *fmt, ...) {
    va_list args;
    char buf[256];
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (s_isParent) {
        // 父进程直接打印
        __android_log_print(levelToAndroidLog(level), LOG_TAG, "%s", buf);
    } else {
        /// 子线程
        if (s_ipc_fd_child >= 0) {
            ssize_t ret = write(s_ipc_fd_child, buf, strlen(buf));
            if (ret == -1) {
                __android_log_print(levelToAndroidLog(ERROR), LOG_TAG,
                                    "child log write failed: %s\n",
                                    strerror(errno));
            }
        }
    }
}


int Logger::levelToAndroidLog(Logger::LogLevel level) {
    switch (level) {

        case DEBUG:
            return ANDROID_LOG_DEBUG;

        case INFO:
            return ANDROID_LOG_INFO;

        case WARN:
            return ANDROID_LOG_WARN;

        case ERROR:
            return ANDROID_LOG_ERROR;

    }
    return 0;
}

void Logger::processLogs() {
    if (!s_isParent || s_ipc_fd_parent < 0) return;

    char buffer[256];
    ssize_t n;
    while ((n = read(s_ipc_fd_parent, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        __android_log_print(levelToAndroidLog(INFO), LOG_TAG, "receive child process Log: %s",
                            buffer);
    }
}

void *Logger::logThreadFunc(void *arg) {
    s_threadRunning = true;
    while (s_threadRunning) {
        processLogs();
        sleep(1);
    }

    return 0;
}


void Logger::startLogThread() {
    if (!s_isParent) return;
    pthread_create(&s_logThread, nullptr, logThreadFunc, nullptr);

}

void Logger::stopLogThread() {

    s_threadRunning = false;
    if (s_ipc_fd_parent >= 0) {
        close(s_ipc_fd_parent);
        s_ipc_fd_parent = -1;
    }

    if (s_ipc_fd_child >= 0) {
        close(s_ipc_fd_child);
        s_ipc_fd_child = -1;
    }
    pthread_join(s_logThread, nullptr);

}