#include "coap_client.h"

LOG_MODULE_DECLARE(COAP_CLIENT);

/* CoAP resources URIs*/
static const char *const hello_world_option[]        = { HELLO_WORLD_URI_PATH,        NULL };
static const char *const curr_temp_option[]          = { CURR_TEMP_URI_PATH,          NULL };

/* CoAP server address structure */
static struct sockaddr_in6 server_addr = {};

void serv_addr_init()
{
    int err;
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(SERVER_PORT);
    err = zsock_inet_pton(AF_INET6, SERVER_IPV6_ADDRESS, &server_addr.sin6_addr);

    __ASSERT(err == 1, "zsock_inet_pton() failed %d", err);
}

/* Callback for hello.GET.reply */
int hello_world_reply(const struct coap_packet *response,
				             struct coap_reply *reply,
				             const struct sockaddr *from)
{
	const uint8_t* payload;
	uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_HELLO_WORLD;

	ARG_UNUSED(reply);

	payload = coap_packet_get_payload(response, &payload_size);

    struct sockaddr receive = *(from);
    struct sockaddr_in6* rcv_sockaddr = net_sin6(&receive);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(rcv_sockaddr->sin6_port);

    uint8_t response_code = coap_header_get_code(response);

    if (inet_ntop(AF_INET6, &rcv_sockaddr->sin6_addr, addr_str, sizeof(addr_str)) == NULL) 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr");
    } 

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from hello_world.GET.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
                addr_str,
                (unsigned int)port,
                (int)(response_code >> 5),
                (int)(response_code & 0x1F));

        return -EINVAL;
    }
    else
    {
        LOG_INF("Received data payload from hello_world.GET.Rsp. DATA: %s, SRC: %s:%u, CODE: (%d.%02d)",
                payload,
                addr_str,
                (unsigned int)port,
                (int)(response_code >> 5),
                (int)(response_code & 0x1F));
        
        return 0;
    }   
}

int curr_temp_reply(const struct coap_packet *response,
				           struct coap_reply *reply,
				           const struct sockaddr *from)
{
    const uint8_t* payload;
    uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_CURR_TEMP;

    ARG_UNUSED(reply);

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
    LOG_INF("Received curr_temp.PUT.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
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

    [[maybe_unused]] float respTemp = atof(payload);

    LOG_INF("Received data payload from curr_temp.PUT.Rsp. DATA: %s, SRC: %s:%u, CODE: (%d.%02d)",
            payload,
            addr_str,
            (unsigned int)port,
            (int)(response_code >> 5),
            (int)(response_code & 0x1F));

    return 0;
}

void hello_world_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_HELLO_WORLD];
    uint32_t payload_size = sizeof(payload);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(server_addr.sin6_port);

    if (inet_ntop(AF_INET6, &server_addr.sin6_addr, addr_str, sizeof(addr_str)) != NULL) 
    {
        LOG_INF("Sending hello_world.GET.Req. DST: %s:%u",
                addr_str,
                (unsigned int)port);
    } 
    else 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr.");
    }

    coap_send_request(COAP_METHOD_GET,
                      (const struct sockaddr*)&server_addr,
                      hello_world_option, payload,
                      payload_size, hello_world_reply);
}

void update_curr_temp_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_CURR_TEMP];
    uint32_t payload_size = sizeof(payload);

    snprintk(payload, payload_size, TEMPERATURE_FORMAT, curr_temp);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(server_addr.sin6_port);

    if (inet_ntop(AF_INET6, &server_addr.sin6_addr, addr_str, sizeof(addr_str)) != NULL) 
    {
        LOG_INF("Sending curr_temp.PUT.Req. DST: %s:%u",
                addr_str,
                (unsigned int)port);
    } 
    else 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr.");
    }

    coap_send_request(COAP_METHOD_PUT,
                      (const struct sockaddr*)&server_addr,
                      curr_temp_option,
                      payload,
                      payload_size,
                      curr_temp_reply);
}