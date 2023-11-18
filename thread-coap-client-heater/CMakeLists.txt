cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(thread-coap-client)

target_sources(app PRIVATE
    src/main.c
    src/communication/coap/coap_client.c
    src/communication/thread/thread.c
    src/communication/thread/nat64_utils.c
    src/communication/thread/mtd/mtd.c
    src/io/buttons/buttons.c
    src/io/simulated/regulator/regulator.c
    src/io/simulated/sensor/sensor.c
    src/utils/client_utils.c)

target_include_directories(app PUBLIC
    src/
    src/common
    src/communication/coap
    src/communication/thread
    src/communication/thread/mtd
    src/io/buttons
    src/io/simulated/
    src/io/simulated/regulator
    src/io/simulated/sensor
    src/utils)