import logging
import asyncio

from aiocoap import *

logging.basicConfig(level=logging.INFO)

async def main():

    # Specify the desired address and port
    host = "172.18.0.1"  # Change this to your desired address
    port = 8383       # Change this to your desired port

    uri = f'coap://{host}:{port}/curr_temp'

    protocol = await Context.create_client_context()

    request = Message(code=GET, uri=uri)

    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        print('Result: %s\n%r'%(response.code, response.payload))
        temp_str = response.payload.decode('ascii')
        temp_flt = float(temp_str)
        print(temp_flt)

    new_temp = 60
    formatted = "{:.2f}".format(new_temp)
    payload = formatted.encode('ascii')

    request = Message(code=PUT, uri=uri, payload=payload)

    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to put resource:')
        print(e)
    else:
        print('Result: %s\n%r'%(response.code, response.payload))
        temp_str = response.payload.decode('ascii')
        temp_flt = float(temp_str)
        print(temp_flt)
        
    uri = f'coap://{host}:{port}/is_room_occupied' 
    request = Message(code=GET, uri=uri)

    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        print('Result: %s\n%r'%(response.code, response.payload))
        temp_str = response.payload.decode('ascii')
        temp_flt = int(temp_str)
        print(temp_flt)

if __name__ == "__main__":
    asyncio.run(main())