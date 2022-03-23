/*
 * @Author: muggle
 * @Date: 2022-02-25 13:12:44
 * @LastEditors: muggle
 * @LastEditTime: 2022-02-25 15:02:32
 * @Description: 
 */
#ifndef __TCP_CLIENT_H__
#define  __TCP_CLIENT_H__

#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "addr_from_stdin.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

int connect_tcp_server(const char *ip, uint16_t port);

#endif
