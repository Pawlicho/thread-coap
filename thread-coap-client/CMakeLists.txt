cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(thread-coap-client)

target_sources(app PRIVATE
    src/main.c
    src/utils/client_utils.c
    src/coap/coap_client.c
    src/thread/mtd/mtd.c
    src/thread/thread.c
    src/utils/buttons.c
    src/regulator/regulator.c
    src/sensor/sensor.c)

target_include_directories(app PUBLIC
    src/
    src/common
    src/coap
    src/regulator
    src/sensor
    src/thread
    src/thread/mtd
    src/utils)