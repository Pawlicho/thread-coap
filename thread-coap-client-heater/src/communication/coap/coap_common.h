#pragma once

/*! Server Enpoint definitions */
                                     
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