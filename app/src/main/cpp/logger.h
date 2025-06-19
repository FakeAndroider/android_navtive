//
// Created by apple on 2025/6/18.
//

#ifndef CPLUSPLUS_LOGGER_H
#define CPLUSPLUS_LOGGER_H


#include <android/log.h>
#include <pthread.h>

class Logger {
public:
    enum LogLevel {
        DEBUG, INFO, WARN, ERROR
    };

    static void init(bool isParent);

    static void log(LogLevel level, const char *fmt, ...);


    // 启动日志处理线程（仅对父进程有效）
    static void startLogThread();

    // 停止日志线程（可选）
    static void stopLogThread();

private:
    static int levelToAndroidLog(LogLevel level);


    static void *logThreadFunc(void *arg);

    static int s_ipc_fd_parent;
    // 父进程保留读端
    static int s_ipc_fd_child;
    // 子进程使用写端
    static bool s_isParent;
    static pthread_t s_logThread;
    static bool s_threadRunning;





};


#endif //CPLUSPLUS_LOGGER_H
