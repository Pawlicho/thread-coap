#pragma once

/*! Server Enpoint definitions */
                                     
#define SERVER_PORT                 (8383)

/*! Resources definitions */

#define TEMPERATURE_URI_PATH          "temperature"
#define HEATER_REGULATION_URI_PATH    "heater_regulation"

/* Other commons */

#define TEMPERATURE_FORMAT                "%.2f"
#define HEATER_REGULATION_FORMAT          "%.1f"

/* 1B sign + 2B integer part + 1B dot + 2B fractional part + End sign */
#define ENCODED_TEMPERATUR_MAX_SIZE (8)

#define ENCODED_HEATER_POWER_MAX_SIZE (6)

/* REQUEST PAYLOAD SIZES */
#define REQUEST_PAYLOAD_SIZE_TEMPERATURE          (ENCODED_TEMPERATUR_MAX_SIZE)
#define REQUEST_PAYLOAD_SIZE_HEATER_REGULATION    (ENCODED_HEATER_POWER_MAX_SIZE)

/* RESPONSE PAYLOAD SIZES */
#define RESPONSE_PAYLOAD_SIZE_TEMPERATURE         (ENCODED_TEMPERATUR_MAX_SIZE)
#define RESPONSE_PAYLOAD_SIZE_HEATER_REGULATION   (ENCODED_HEATER_POWER_MAX_SIZE)