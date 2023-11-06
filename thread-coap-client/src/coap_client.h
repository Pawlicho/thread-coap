#pragma once

#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <zephyr/net/socket.h>

#include "coap_common.h"
#include "common.h"

int hello_world_reply(const struct coap_packet *response,
				       struct coap_reply *reply,
				       const struct sockaddr *from);

int curr_temp_reply(const struct coap_packet *response,
				    struct coap_reply *reply,
				    const struct sockaddr *from);

void hello_world_work_cb(struct k_work *item);

void update_curr_temp_work_cb(struct k_work *item);

void serv_addr_init();