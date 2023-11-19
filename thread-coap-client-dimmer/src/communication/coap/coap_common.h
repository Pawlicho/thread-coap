#pragma once

/*! Server Enpoint definitions */
                                     
#define SERVER_PORT                          (8383)

/*! Resources definitions */

#define DIMMER_URI_PATH                      "dimmer"

/* Other commons */

#define ILLUMINANCE_FORMAT                   "%.2f"

/* 1B sign + 2B integer part + 1B dot + 2B fractional part + End sign */
#define ENCODED_DIMMER_MAX_SIZE              (8)

/* REQUEST PAYLOAD SIZES */
#define REQUEST_PAYLOAD_SIZE_DIMMER          (ENCODED_DIMMER_MAX_SIZE)

/* RESPONSE PAYLOAD SIZES */
#define RESPONSE_PAYLOAD_SIZE_DIMMER         (ENCODED_DIMMER_MAX_SIZE)