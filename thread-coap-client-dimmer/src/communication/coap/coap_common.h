#pragma once

/*! Server Enpoint definitions */
                                     
#define SERVER_PORT                          (8383)

/*! Resources definitions */

#define ILLUMINANCE_URI_PATH                            "illuminance"
#define DIMMER_REGULATION_URI_PATH                      "dimmer_regulation"

/* Other commons */

#define ILLUMINANCE_FORMAT                              "%.2f"
#define DIMMER_REGULATION_FORMAT                        "%.1f"

/* 1B sign + 2B integer part + 1B dot + 2B fractional part + End sign */
#define ENCODED_ILLUMINANCE_MAX_SIZE                    (8)
#define ENCODED_DIMMER_POWER_MAX_SIZE                   (6)

/* REQUEST PAYLOAD SIZES */
#define REQUEST_PAYLOAD_SIZE_ILLUMINANCE                 (ENCODED_ILLUMINANCE_MAX_SIZE)
#define REQUEST_PAYLOAD_SIZE_DIMMER_REGULATION           (ENCODED_DIMMER_POWER_MAX_SIZE)

/* RESPONSE PAYLOAD SIZES */
#define RESPONSE_PAYLOAD_SIZE_ILLUMINANCE                (ENCODED_ILLUMINANCE_MAX_SIZE)
#define RESPONSE_PAYLOAD_SIZE_DIMMER_REGULATION          (ENCODED_DIMMER_POWER_MAX_SIZE)