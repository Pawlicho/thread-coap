#include "coap_client.h"

#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <zephyr/net/socket.h>

LOG_MODULE_REGISTER(coap_client, LOG_LEVEL_DBG);

static bool is_connected;
static struct k_work hello_world_work;

static struct sockaddr_in6 server_addr = {};

static void serv_addr_init()
{
    int err;
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(SERVER_PORT);
    err = zsock_inet_pton(AF_INET6, SERVER_IPV6_ADDRESS, &server_addr.sin6_addr);

    __ASSERT(err == 1, "zsock_inet_pton() failed %d", err);
}

static const char *const hello_world_option[] = { HELLO_WORLD_URI_PATH, NULL };

static void on_thread_state_changed(otChangedFlags flags,
                                    struct openthread_context *ot_context,
                                    void *user_data)
{
	if (flags & OT_CHANGED_THREAD_ROLE) 
    {
		switch (otThreadGetDeviceRole(ot_context->instance)) 
        {
            case OT_DEVICE_ROLE_CHILD:
            case OT_DEVICE_ROLE_ROUTER:
            case OT_DEVICE_ROLE_LEADER:
            is_connected = true;
            break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default:
            is_connected = false;
            break;
		}
	}
}

static struct openthread_state_changed_cb ot_state_chaged_cb = 
{
	.state_changed_cb = on_thread_state_changed
};

static void submit_work_if_connected(struct k_work *work)
{
	if (is_connected) 
    {
		k_work_submit(work);
	} 
    else 
    {
		LOG_WRN("Connection is broken");
	}
}

/* Callback for hello.GET.reply */
static int hello_reply(const struct coap_packet *response,
				       struct coap_reply *reply,
				       const struct sockaddr *from)
{
	const uint8_t *payload;
	uint16_t payload_size = 32; /*! TODO: Hardcoded, change it */

	ARG_UNUSED(reply);
	ARG_UNUSED(from);

	payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload");
        return -EINVAL;
    }

    /* Print as a debug */
    printk("%s", payload);
    return 0;
}

static void hello_world_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    /* Unused - for now */
    uint8_t* payload = NULL;
    uint32_t payload_size = 0;

    /*! TODO: Add some hello_world message, for the server to see it works */

    LOG_INF("Send 'hello' request to the server"); /* Include address here in the future */
    coap_send_request(COAP_METHOD_GET, (const struct sockaddr*)&server_addr, hello_world_option, payload, payload_size, hello_reply);

}

void coap_client_init()
{
    serv_addr_init();
    coap_init(AF_INET6, NULL);
    k_work_init(&hello_world_work, hello_world_work_cb);

	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb);
	openthread_start(openthread_get_default_context());
}

void testConnection()
{
    submit_work_if_connected(&hello_world_work);
}