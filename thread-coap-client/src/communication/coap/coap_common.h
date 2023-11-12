#pragma once

/*! Server Enpoint definitions */

/*! Synthesised IPv4 Address, from: 172.18.0.1 */
/*! Need to find better sollution than static synthesied prefix becouse the prefix apparently changes */
#define SERVER_IPV6_ADDRESS         "fd35:59b3:baba:2:0:0:ac12:1"
                                     
#define SERVER_PORT                 (8383)

/*! Resources definitions */

#define HEATER_URI_PATH          "heater"

/* Other commons */

#define TEMPERATURE_FORMAT          "%.2f"

/* 1B sign + 2B integer part + 1B dot + 2B fractional part + End sign */
#define ENCODED_TEMPERATUR_MAX_SIZE (8)

/* REQUEST PAYLOAD SIZES */
#define REQUEST_PAYLOAD_SIZE_HEATER          (ENCODED_TEMPERATUR_MAX_SIZE)

/* RESPONSE PAYLOAD SIZES */
#define RESPONSE_PAYLOAD_SIZE_HEATER         (ENCODED_TEMPERATUR_MAX_SIZE)