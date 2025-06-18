//
// Created by apple on 2025/6/18.
//

#ifndef CPLUSPLUS_INIT_PROTECT_H
#define CPLUSPLUS_INIT_PROTECT_H

#ifdef __cplusplus
extern "C" {
#endif

///step1: socketpair
int reg_socketpair_callback();

/// step2: epoll
int reg_epoll_callback();
int loop_epoll_event(int epfd);


#ifdef __cplusplus
}
#endif

#endif //CPLUSPLUS_INIT_PROTECT_H
