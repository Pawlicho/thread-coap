
import asyncio

import aiocoap
import aiocoap.resource as resource

from coordinator import periodic_task, ServerSetParameters

server_params = ServerSetParameters()

class HeaterResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.temp = 21.5

    def encodeContent(self):
        retVal = "{:.2f}".format(self.temp)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.temp = float(decoded)

    async def render_get(self, request):
        print("\nReceived curr_temp.GET.Req\n")
        self.encodeContent()
        print("\Responding with curr_temp.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived curr_temp.PUT.Req")
        self.decodeContent(request.payload)

        print("New curr_temp value on the server side: " + str(self.temp))

        # Here should be anchored mechanism to count correction
        self.temp = server_params.set_temperature - self.temp

        self.encodeContent()
        print("Responding with curr_temp.PUT.Rsp\n")
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)

async def main():

    # Specify the desired address and port
    host = "172.18.0.1"
    port = 8383

    root = resource.Site()
    root.add_resource(['heater'], HeaterResource())

    periodic_task_coroutine = asyncio.create_task(periodic_task(server_params))

    await aiocoap.Context.create_server_context(root, bind=(host, port))

    await periodic_task_coroutine

    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main ())