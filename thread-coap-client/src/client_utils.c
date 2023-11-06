#include "client_utils.h"

#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <zephyr/net/socket.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

/* Callback for switching MED mode*/
mtd_mode_toggle_cb_t mtd_mode_toggle;

/* Global var indicating Thread state */
static bool is_connected;

static volatile float curr_temp = -11.23f;
static volatile float correction;

/* Tasks declaration */
static struct k_work hello_world_work;
static struct k_work update_curr_temp_work;
static struct k_work toggle_MTD_SED_work;

/* CoAP resources URIs*/
static const char *const hello_world_option[]        = { HELLO_WORLD_URI_PATH,        NULL };
static const char *const curr_temp_option[]          = { CURR_TEMP_URI_PATH,          NULL };

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

    /* Some casting */
    struct sockaddr receive = *(from);
    struct sockaddr_in6* rcv_sockaddr = net_sin6(&receive);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(rcv_sockaddr->sin6_port);

    uint8_t response_code = coap_header_get_code(response);


    if (inet_ntop(AF_INET6, &rcv_sockaddr->sin6_addr, addr_str, sizeof(addr_str)) == NULL) 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr");
    } 

    /*! TODO: Remove magic numbers */
    LOG_WRN("Received curr_temp.PUT.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
              addr_str,
              (unsigned int)port,
              (int)(response_code >> 5),
              (int)(response_code & 0x1F));

    payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from curr_temp.PUT.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
                addr_str,
                (unsigned int)port,
                (int)(response_code >> 5),
                (int)(response_code & 0x1F));
                
        return -EINVAL;
    }

    float echo_value = atof(payload);

    LOG_WRN("Received data payload from curr_temp.PUT.Rsp. DATA: %s, SRC: %s:%u, CODE: (%d.%02d)",
            payload,
            addr_str,
            (unsigned int)port,
            (int)(response_code >> 5),
            (int)(response_code & 0x1F));

    printk("\nReceived value from curr_temp.PUT.Rsp: %.2f\n", echo_value);

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

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(server_addr.sin6_port);

    if (inet_ntop(AF_INET6, &server_addr.sin6_addr, addr_str, sizeof(addr_str)) != NULL) 
    {
        LOG_WRN("Sending curr_temp.PUT.Req. DST: %s:%u",
                addr_str,
                (unsigned int)port);
    } 
    else 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr.");
    }

    /* Send curr_temp.PUT request */
    coap_send_request(COAP_METHOD_PUT,
                      (const struct sockaddr*)&server_addr,
                      curr_temp_option,
                      payload,
                      payload_size,
                      curr_temp_reply);
}

static void toggle_minimal_sleepy_end_device_work_cb(struct k_work *item)
{
	otError error;
	otLinkModeConfig mode;
	struct openthread_context *context = openthread_get_default_context();

	__ASSERT_NO_MSG(context != NULL);

	openthread_api_mutex_lock(context);
	mode = otThreadGetLinkMode(context->instance);
	mode.mRxOnWhenIdle = !mode.mRxOnWhenIdle;
	error = otThreadSetLinkMode(context->instance, mode);
	openthread_api_mutex_unlock(context);

	if (error != OT_ERROR_NONE) 
    {
		LOG_ERR("Failed to set MLE link mode configuration");
	} 
    else 
    {
		mtd_mode_toggle(mode.mRxOnWhenIdle);
	}
}

void coap_client_init(mtd_mode_toggle_cb_t on_toggle)
{
    mtd_mode_toggle = on_toggle;

    serv_addr_init();
    coap_init(AF_INET6, NULL);

    k_work_init(&hello_world_work, hello_world_work_cb);
    k_work_init(&update_curr_temp_work, update_curr_temp_work_cb);
    k_work_init(&toggle_MTD_SED_work, toggle_minimal_sleepy_end_device_work_cb);
    
	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb);
	openthread_start(openthread_get_default_context());
}

/* UI functions */

void testConnection(void)
{
    submit_work_if_connected(&hello_world_work);
}

void updateCurrentTemperature(void)
{
    submit_work_if_connected(&update_curr_temp_work);
}

void toggle_minimal_sleepy_end_device(void)
{
	k_work_submit(&toggle_MTD_SED_work);
}