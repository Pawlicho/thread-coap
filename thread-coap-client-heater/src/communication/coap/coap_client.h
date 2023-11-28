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

int temperature_reply(const struct coap_packet *response,
				    struct coap_reply *reply,
				    const struct sockaddr *from);

void update_temperature_work_cb(struct k_work *item);

int heater_regulation_reply(const struct coap_packet *response,
				            struct coap_reply *reply,
				            const struct sockaddr *from);

void update_heater_regulation_work_cb(struct k_work *item);

void serv_addr_init(const uint8_t* server_ip_address);