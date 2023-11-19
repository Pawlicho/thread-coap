#!/bin/bash

source /home/tymoczko/src/thread-coap/env/coap-server-venv/bin/activate

nohup python /home/tymoczko/src/thread-coap/coap-server/web_gui/web_gui.py &
disown

python /home/tymoczko/src/thread-coap/coap-server/coap_server.py