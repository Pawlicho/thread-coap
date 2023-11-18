#include "nat64_utils.h"
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static bool get_nat64_prefix(uint8_t* prefix)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig config;

    bool retval = false;

    while (otNetDataGetNextRoute(openthread_get_default_instance(), &iterator, &config) == OT_ERROR_NONE)
    {
        if (config.mNat64)
        {
            retval = true;
            memcpy(prefix, config.mPrefix.mPrefix.mFields.mComponents.mNetworkPrefix.m8, 8);
            
            LOG_INF("Found NAT64 prefix %02x %02x %02x %02x %02x %02x %02x %02x",
                    prefix[0],
                    prefix[1],
                    prefix[2],
                    prefix[3],
                    prefix[4],
                    prefix[5],
                    prefix[6],
                    prefix[7]);
        }
    }
    return retval;
}

bool synthesize_ipv4_to_ipv6(char* ipv6_buff)
{
    uint8_t prefix[8] = {0};
    bool retval = get_nat64_prefix(prefix);

    if ( retval == false )
    {
        return retval;
    }
    else
    {
        snprintk(ipv6_buff, INET6_ADDRSTRLEN + 1, "%02x%02x:%02x%02x:%02x%02x:%02x%02x%s",
             prefix[0],
             prefix[1],
             prefix[2],
             prefix[3],
             prefix[4],
             prefix[5],
             prefix[6],
             prefix[7],
             SERVER_NAT64_SUFIX);

        LOG_INF("Synthesized Server IPv6 addres: %s",
             ipv6_buff);
    }

    return retval;
}