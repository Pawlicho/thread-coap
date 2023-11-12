
import asyncio

import aiocoap
import aiocoap.resource as resource
import sqlite3

from coordinator import periodic_task, ServerSetParameters, ServerCurrentParameters, calc_correction_temp
from logs_manager import LogsManager


db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'
server_set_params = ServerSetParameters()
server_cur_params = ServerCurrentParameters()
logs_manager = LogsManager(db_path)

class HeaterResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.temperature = 21.5

    def encodeContent(self, value):
        retVal = "{:.2f}".format(value)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.temperature = float(decoded)

    async def render_get(self, request):
        print("\nReceived curr_temp.GET.Req\n")
        self.encodeContent(self.temperature)
        print("\Responding with curr_temp.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived curr_temp.PUT.Req")

        # Decode received request's payload from ascii, convert to temp format
        self.decodeContent(request.payload)
        print("New curr_temp value on the server side: " + str(self.temperature))

        # Update current temperature value from client req
        server_cur_params.Update(self.temperature)

        # Calculate temperature correction to send in resp
        correction = calc_correction_temp(server_set_params.GetTemperature(), server_cur_params.GetTemperature())

        # Encode response payload in ascii
        self.encodeContent(correction)

        # Log request
        logs_manager.AddClient1Log(self.temperature, correction, request.remote.hostinfo)
        

        # Send the response to client
        print("Responding with curr_temp.PUT.Rsp\n")
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)

async def main():

    # Server address and port number
    host = "172.18.0.1"
    port = 8383

    # Set up resources on server side
    root = resource.Site()
    root.add_resource(['heater'], HeaterResource())

    # Routine for database set parameters pulling
    periodic_task_coroutine = asyncio.create_task(periodic_task(server_set_params, logs_manager))

    # Start Server
    await aiocoap.Context.create_server_context(root, bind=(host, port))

    # Await for periodic, infinite routine
    await periodic_task_coroutine

    # Stay in the loop forever
    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main ())