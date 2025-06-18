#include <jni.h>
#include <string>
#include <sys/wait.h>
#include "engine.h"
#include "log_util.h"
#include "init_protect.h"
#include "logger.h"

void test_child_process();

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_cplusplus_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_cplusplus_MainActivity_onEngineCreate(JNIEnv *env, jobject) {

    Engine engine = Engine(10);
    engine.add(1);
    ndk_log_info("output is %d", engine.output());

    reg_socketpair_callback();

    test_child_process();

//    int epfd = reg_epoll_callback();
//    if (epfd != -1) {
//        loop_epoll_event(epfd);
//    }


    return JNI_TRUE;
}


void test_child_process() {
    Logger::init(true);
    Logger::startLogThread();
    Logger::log(Logger::INFO, "parent process started, pid = %d", getpid());

    sleep(2);
    ndk_log_info("testing..");

    pid_t pid = fork();
    if (pid < 0) {
        Logger::log(Logger::ERROR, "fork failed %s \n ", strerror(errno));
    }

    if (pid == 0) {
        // 子进程
        Logger::init(false);
        sleep(1);
        //wait child ipc start
        Logger::log(Logger::INFO, "Hello from child process, PID = %d\n", getpid());
        for (int i = 0; i < 3; i++) {
            Logger::log(Logger::DEBUG, "Child iteration %d\n", i);
            sleep(1);
        }

    } else {
        ndk_log_info("parent pid = %d", getpid());
    }
    wait(NULL);
    Logger::stopLogThread();
}