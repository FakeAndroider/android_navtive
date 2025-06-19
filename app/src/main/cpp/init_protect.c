//
// Created by apple on 2025/6/18.
//
#include "init_protect.h"

#include "log_util.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>



#define MAX_EVENTS 10
// 全局变量，用于 signal 通知机制
int signal_write_fd = -1;
int signal_read_fd = -1;

static void reap_children();

static void write_sigchld_handler(int signum);

static void read_signal_handler();


//写入数据
static void write_sigchld_handler(int signum) {
// 向 signal_write_fd 写入一个字符("1")，以便通过 epoll 得到通知
    if (TEMP_FAILURE_RETRY(write(signal_write_fd, "1", 1)) == -1) {
        ndk_log_error("write(signal_write_fd) failed: %s\n", strerror(errno));
    }
}

//读取数据
static void read_signal_handler() {
    // Clear outstanding requests.
    char buf[32];
    // 读取 signal_read_fd 中的数据，并放入 buf，这里读出并没有什么实际作用，只是用于阻塞等待
    ssize_t n = read(signal_read_fd, buf, sizeof(buf));
    if (n > 0) {
        reap_children();
    }
}

static void reap_children() {
    int status;
    pid_t pid;
    //使用 waitpid 循环回收退出的子进程
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        ndk_log_info("child process %d terminated, status is %d", pid, status);
    }
}


int reg_socketpair_callback() {
    /*
    初始化信号处理：
     创建 socketpair 用于在信号处理函数和 epoll 事件循环之间通信。
     注册 SIGCHLD 信号，指定 write_sigchld_handler 作为回调。
     */

    int s[2];
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, s) == -1) {
        ndk_log_error("socketpair failed: %s\n", strerror(errno));
        return -1;
    }
    signal_write_fd = s[0];
    signal_read_fd = s[1];

// SA_NOCLDSTOP 使 init 进程只有在其子进程终止时才会受到 SIGCHLD 信号
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = write_sigchld_handler;
    act.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        ndk_log_error("sigaction failed  %s \n ", strerror(errno));
        return -1;
    }
    return 0;

}


int reg_epoll_callback() {
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        ndk_log_error("epoll_create1 failed %s \n ", strerror(errno));
        return -1;
    }

    struct epoll_event ee;
    memset(&ee, 0, sizeof(ee));
    ee.events = EPOLLIN;
    ee.data.fd = signal_read_fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, signal_read_fd, &ee) == -1) {
        ndk_log_error("epoll_ctl failed %s \n ", strerror(errno));
        return -1;
    }
    return epfd;
}


int loop_epoll_event(int epfd) {
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, 5000);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            ndk_log_error("epoll_wait failed: %s", strerror(errno));
            break;
        }

        if (nfds == 0) {
            ndk_log_info("No epoll events, waiting...");
            continue;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == signal_read_fd && (events[i].events & EPOLLIN)) {
                read_signal_handler();
            }
        }

    }
    close(epfd);
    return 0;
}



