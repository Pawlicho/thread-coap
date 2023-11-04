#include "coap_client.h"
#include "utils.h"

#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <zephyr/net/socket.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

/* Global var indicating Thread state */
static bool is_connected;

static volatile float curr_temp = -11.23f;
static volatile enum OCCUPATION_STATE occupation_state = OCCUPATION_STATE_ROOM_OCCUPIED;

/* Tasks declaration */
static struct k_work hello_world_work;
static struct k_work update_curr_temp_work;
static struct k_work update_occupation_state_work;

/* CoAP resources URIs*/
static const char *const hello_world_option[]        = { HELLO_WORLD_URI_PATH,        NULL };
static const char *const curr_temp_option[]          = { CURR_TEMP_URI_PATH,          NULL };
static const char *const is_room_occupied_option[]   = { IS_ROOM_OCCUPIED_URI_PATH,   NULL };

/* CoAP server address structure */
static struct sockaddr_in6 server_addr = {};

static void serv_addr_init()
{
    int err;
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(SERVER_PORT);
    err = zsock_inet_pton(AF_INET6, SERVER_IPV6_ADDRESS, &server_addr.sin6_addr);

    __ASSERT(err == 1, "zsock_inet_pton() failed %d", err);
}

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
		LOG_WRN("Thread connection is broken");
	}
}

/* Callback for hello.GET.reply */
static int hello_world_reply(const struct coap_packet *response,
				             struct coap_reply *reply,
				             const struct sockaddr *from)
{
	const uint8_t* payload;
	uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_HELLO_WORLD;

	ARG_UNUSED(reply);
	ARG_UNUSED(from);

	payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from hello_world.GET response");
        return -EINVAL;
    }

    /* Print as a debug */
    printk("%s", payload);
    return 0;
}

static int curr_temp_reply(const struct coap_packet *response,
				           struct coap_reply *reply,
				           const struct sockaddr *from)
{
    const uint8_t* payload;
    uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_CURR_TEMP;

    ARG_UNUSED(reply);
	ARG_UNUSED(from);

    payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from curr_temp.PUT.Rsp.");
        return -EINVAL;
    }

    float echo_value = atof(payload);

    printk("\nReceived value from curr_temp.PUT.Rsp: %.2f\n", echo_value);

    return 0;
}

static int is_room_occupied_reply(const struct coap_packet *response,
				                  struct coap_reply *reply,
				                  const struct sockaddr *from)
{
    const uint8_t* payload;
    uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_IS_ROOM_OCCUPIED;

    ARG_UNUSED(reply);
	ARG_UNUSED(from);

    payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from is_room_occupied.PUT.Rsp.");
        return -EINVAL;
    }

    unsigned int echo_value = (unsigned int)atoi(payload);

    printk("\nReceived value from is_room_occupied.PUT: %u\n", echo_value);

    return 0;
}

static void hello_world_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    /* Unused - for now */
    uint8_t payload[REQUEST_PAYLOAD_SIZE_HELLO_WORLD];
    uint32_t payload_size = sizeof(payload);

    LOG_INF("Sending hello_world.GET.Req."); /* Include address here in the future */

    coap_send_request(COAP_METHOD_GET,
                      (const struct sockaddr*)&server_addr,
                      hello_world_option, payload,
                      payload_size, hello_world_reply);
}

static void update_curr_temp_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_CURR_TEMP];
    uint32_t payload_size = sizeof(payload);

    snprintk(payload, payload_size, TEMPERATURE_FORMAT, curr_temp);
    LOG_INF("Sending curr_temp.PUT.Req.");

    /* Send curr_temp.PUT request */
    coap_send_request(COAP_METHOD_PUT,
                      (const struct sockaddr*)&server_addr,
                      curr_temp_option,
                      payload,
                      payload_size,
                      curr_temp_reply);
}

static void update_occupation_state_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_IS_ROOM_OCCUPIED];
    uint32_t payload_size = sizeof(payload);

    snprintk(payload, payload_size, "%d", occupation_state);
    LOG_INF("Sending is_room_occupied.PUT.Req.");

    /* Send curr_temp.PUT request */
    coap_send_request(COAP_METHOD_PUT,
                      (const struct sockaddr*)&server_addr,
                      is_room_occupied_option,
                      payload,
                      payload_size,
                      is_room_occupied_reply);
}

void coap_client_init()
{
    serv_addr_init();
    coap_init(AF_INET6, NULL);

    k_work_init(&hello_world_work, hello_world_work_cb);
    k_work_init(&update_curr_temp_work, update_curr_temp_work_cb);
    k_work_init(&update_occupation_state_work, update_occupation_state_work_cb);

	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb);
	openthread_start(openthread_get_default_context());
}

/* UI functions */

void testConnection()
{
    submit_work_if_connected(&hello_world_work);
}

void updateCurrentTemperature()
{
    submit_work_if_connected(&update_curr_temp_work);
}

void updateOccupationState()
{
    submit_work_if_connected(&update_occupation_state_work);
}