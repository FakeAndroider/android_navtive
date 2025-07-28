//
// Created by apple on 2025/7/28.
//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/types.h>
#include <stdbool.h>
#include <string.h>
#include "binder.h"

#define HELLO_SVR_CMD_SAYHELLO     1
#define HELLO_SVR_CMD_SAYHELLO_TO  2

int g_handle = 0;
struct binder_state *g_bs;

void seyHello(void) {
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;

    ///构建 binder_io
    bio_init(&msg, iodata, sizeof(iodata), 4);



    /// 放入参数
    bio_put_uint32(&msg, 0);
    bio_put_string16_x(&msg, "IHelloService");

    if (binder_call(g_bs, &msg, &reply, HELLO_SVR_CMD_SAYHELLO)) {
        return;
    }

    /// 从reply 中解析出返回值
    binder_done(g_bs, &msg, &reply);

}

int main(int argc, char **argv) {

    int fd;
    struct binder_state *bs;

    u_int32_t svcmgr = BINDER_SERVICE_MANAGER;
    int ret;

    /// open binder
    bs = binder_open("/dev/binder", 128 * 1024);
    if (!bs) {
        fprintf(stderr, "client failed to open binder driver\n");
        return -1;
    }


    ///bind global
    g_bs = bs;
    g_handle = svcmgr_lookup(bs, svcmgr, "hello");
    if (g_handle) {
        fprintf(stderr, "client failed to lookup hello function\n");
        return -1;
    }

    seyHello();
}


