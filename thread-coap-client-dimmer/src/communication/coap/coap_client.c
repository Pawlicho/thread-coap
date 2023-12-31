#include "coap_client.h"
#include "dimmer.h"

LOG_MODULE_DECLARE(COAP_CLIENT);

/* CoAP resources URIs*/
static const char *const illuminance_option[]       = { ILLUMINANCE_URI_PATH,       NULL };
static const char *const dimmer_regulation_option[] = { DIMMER_REGULATION_URI_PATH, NULL };

/* CoAP server address structure */
static struct sockaddr_in6 server_addr = {};

void serv_addr_init(const uint8_t* server_ip_address)
{
    int err;
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(SERVER_PORT);
    err = zsock_inet_pton(AF_INET6, server_ip_address, &server_addr.sin6_addr);

    __ASSERT(err == 1, "zsock_inet_pton() failed %d", err);
}

int illuminance_reply(const struct coap_packet *response,
				      struct coap_reply *reply,
				      const struct sockaddr *from)
{
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
    LOG_INF("Received illuminance.PUT.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
              addr_str,
              (unsigned int)port,
              (int)(response_code >> 5),
              (int)(response_code & 0x1F));

    return 0;
}

void update_illuminance_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_ILLUMINANCE];
    uint32_t payload_size = sizeof(payload);

    snprintk(payload, payload_size, ILLUMINANCE_FORMAT, dimmer.current_illuminance);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(server_addr.sin6_port);

    if (inet_ntop(AF_INET6, &server_addr.sin6_addr, addr_str, sizeof(addr_str)) != NULL) 
    {
        LOG_INF("Sending illuminance.PUT.Req. DST: %s:%u",
                addr_str,
                (unsigned int)port);
    } 
    else 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr.");
    }

    coap_send_request(COAP_METHOD_PUT,
                      (const struct sockaddr*)&server_addr,
                      illuminance_option,
                      payload,
                      payload_size,
                      illuminance_reply);
}

int dimmer_regulation_reply(const struct coap_packet *response,
				            struct coap_reply *reply,
				            const struct sockaddr *from)
{
    const uint8_t* payload;
    uint16_t payload_size = RESPONSE_PAYLOAD_SIZE_DIMMER_REGULATION;

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
    LOG_INF("Received dimmer_regulation.GET.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
              addr_str,
              (unsigned int)port,
              (int)(response_code >> 5),
              (int)(response_code & 0x1F));

    payload = coap_packet_get_payload(response, &payload_size);

    if (payload == NULL)
    {
        LOG_ERR("Did not receive data payload from dimmer_regulation.GET.Rsp. SRC: %s:%u, CODE: (%d.%02d)",
                addr_str,
                (unsigned int)port,
                (int)(response_code >> 5),
                (int)(response_code & 0x1F));
                
        return -EINVAL;
    }

    dimmer.output_power = atof(payload);
    LOG_INF("Received new power: %.1f", dimmer.output_power);

    LOG_INF("Received data payload from dimmer_regulation.GET.Rsp. DATA: %s, SRC: %s:%u, CODE: (%d.%02d)",
            payload,
            addr_str,
            (unsigned int)port,
            (int)(response_code >> 5),
            (int)(response_code & 0x1F));

    return 0;
}

void update_dimmer_regulation_work_cb(struct k_work *item)
{
    ARG_UNUSED(item);

    uint8_t payload[REQUEST_PAYLOAD_SIZE_DIMMER_REGULATION];
    uint32_t payload_size = sizeof(payload);

    snprintk(payload, payload_size, DIMMER_REGULATION_FORMAT, dimmer.output_power);

    char addr_str[NET_IPV6_ADDR_LEN];
    uint16_t port = ntohs(server_addr.sin6_port);

    if (inet_ntop(AF_INET6, &server_addr.sin6_addr, addr_str, sizeof(addr_str)) != NULL) 
    {
        LOG_INF("Sending dimmer_regulation.GET.Req. DST: %s:%u",
                addr_str,
                (unsigned int)port);
    } 
    else 
    {
        LOG_WRN("Unable to resolve IP address from sockaddr.");
    }

    coap_send_request(COAP_METHOD_GET,
                      (const struct sockaddr*)&server_addr,
                      dimmer_regulation_option,
                      payload,
                      payload_size,
                      dimmer_regulation_reply);
}