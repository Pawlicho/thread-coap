#pragma once
#include <stdbool.h>

#define THREAD_CONNECTION_LED       (DK_LED2)
#define SERVER_NAT64_SUFIX          ":0:0:ac12:1"

/* Global var indicating Thread state */
extern bool is_connected;
