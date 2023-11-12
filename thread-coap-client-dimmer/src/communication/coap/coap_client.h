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

int dimmer_reply(const struct coap_packet *response,
				 struct coap_reply *reply,
				 const struct sockaddr *from);

void update_dimmer_work_cb(struct k_work *item);

void serv_addr_init();