#pragma once

/*! Server Enpoint definitions */

/*! Synthesised IPv4 Address, from: 172.18.0.1 */
/*! Need to find better sollution than static synthesied prefix becouse the prefix apparently changes */
#define SERVER_IPV6_ADDRESS         "fdf2:d7b6:d894:2:0:0:ac12:1"
                                     
#define SERVER_PORT                 (8383)

/*! Resources definitions */

#define HELLO_WORLD_URI_PATH        "hello_world"
#define CURR_TEMP_URI_PATH          "curr_temp"
#define IS_ROOM_OCCUPIED_URI_PATH   "is_room_occupied"

/* Other commons */

#define TEMPERATURE_FORMAT          "%.2f"

/* 1B sign + 2B integer part + 1B dot + 2B fractional part + End sign */
#define ENCODED_TEMPERATUR_MAX_SIZE (8)

/* REQUEST PAYLOAD SIZES */
#define REQUEST_PAYLOAD_SIZE_HELLO_WORLD        (10)
#define REQUEST_PAYLOAD_SIZE_CURR_TEMP          (ENCODED_TEMPERATUR_MAX_SIZE)
#define REQUEST_PAYLOAD_SIZE_IS_ROOM_OCCUPIED   (2)

/* RESPONSE PAYLOAD SIZES */
#define RESPONSE_PAYLOAD_SIZE_HELLO_WORLD       (10)
#define RESPONSE_PAYLOAD_SIZE_CURR_TEMP         (ENCODED_TEMPERATUR_MAX_SIZE)
#define RESPONSE_PAYLOAD_SIZE_IS_ROOM_OCCUPIED  (2)