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

//    reg_socketpair_callback();

    test_child_process();

//    int epfd = reg_epoll_callback();
//    if (epfd != -1) {
//        loop_epoll_event(epfd);
//    }


    return JNI_TRUE;
}


void test_child_process() {
    Logger::init(true);
    Logger::log(Logger::INFO, "start testing..", nullptr);

    sleep(2);

    pid_t pid = fork();
    if (pid < 0) {
        Logger::log(Logger::ERROR, "fork failed %s \n ", strerror(errno));
    }

    if (pid == 0) {
        // 子进程
        Logger::init(false);
        sleep(1);
        //wait child ipc start
        Logger::log(Logger::INFO, "child pid = %d\n", getpid());
        for (int i = 0; i < 3; i++) {
            Logger::log(Logger::INFO, "child iteration %d\n", i);
            sleep(1);
        }
        Logger::log(Logger::ERROR, "child exit pid = %d\n", getpid());
        Logger::stopLogThread();
        exit(0);// child exit cleanly


    } else {

        Logger::startLogThread();
        Logger::log(Logger::INFO, "parent pid = %d", getpid());
    }


}