//
// Created by apple on 2025/7/16.
//
#include <linux/types.h>

#include "binder.h"
#include "../log_util.h"

#define  LOG_TAG "binder_server"

#define  HELLO_SVG_CMD_SAYHELLO 1
#define  HELLO_SVG_CMD_SAYHELLO_TO 2


void sayHello(void) {
    static int cnt = 0;
    ALOGW("say hello : %d", ++cnt);
}

int sayHelloTo(char *name) {
    static int cnt = 0;
    ALOGW("say hello to %s : %d", name, ++cnt);
    return cnt;
}


int hello_service_handler(struct binder_state *bs,
                          struct binder_transaction_data_secctx *txn_secctx,
                          struct binder_io *msg,
                          struct binder_io *reply) {

    struct binder_transaction_data *txn = &txn_secctx->transaction_data;
    uint16_t *s;
    char name[512];
    size_t len;
    uint32_t strict_policy;
    int i = 0;


    strict_policy = bio_get_uint32(msg);
    switch (txn->code) {
        case HELLO_SVG_CMD_SAYHELLO: {
            sayHello();
            bio_put_uint32(reply, 0);
            return 0;
        }
            break;
        case HELLO_SVG_CMD_SAYHELLO_TO: {
            s = bio_get_string16(msg, &len); // "IHelloService"
            s = bio_get_string16(msg, &len); // name
            if (s == NULL) {
                return -1;
            }
            for (; i < len; ++i) name[i] = s[i];
            name[i] = '\0';
            i = sayHelloTo(name);

            bio_put_uint32(reply, 0);
            bio_put_uint32(reply, i);
        }
            break;
        default:
            ALOGW("unknown  code %d\n", txn->code);
            return -1;
            break;

    }

    return 0;
}

int test_server_handle(struct binder_state *bs,
                       struct binder_transaction_data_secctx *txn_secctx,
                       struct binder_io *msg,
                       struct binder_io *reply) {


    struct binder_transaction_data *txn = &txn_secctx->transaction_data;


    int (*handler)(struct binder_state *bs,
                   struct binder_transaction_data *txn,
                   struct binder_io *msg,
                   struct binder_io *reply);


    //svcmgr_publish 传入的函数指针 == target.ptr


    handler = (int (*)(struct binder_state *bs,
                       struct binder_transaction_data *txn,
                       struct binder_io *msg,
                       struct binder_io *reply)) txn->target.ptr;


    return handler(bs, txn, msg, reply);

}

/// main 函数
int main(int argc, char  **argv) {


    struct  binder_state *bs;
    uint32_t  svcmgr = BINDER_SERVICE_MANAGER;
    uint32_t handle;
    int ret;


    /// open binder
    bs = binder_open("/dev/binder", 128 * 1024);
    if (!bs)
    {
        fprintf(stderr, "service failed to open binder driver\n");
        return -1;
    }


    /// 发布服务
    ret = svcmgr_publish(bs, svcmgr, "hello", hello_service_handler);
    if (ret) {
        fprintf(stderr, "service failed to publish hello service\n");
        return  -1;
    }


    /// 等待事件
    binder_loop(bs, test_server_handle);

    return 0;

}












































































































