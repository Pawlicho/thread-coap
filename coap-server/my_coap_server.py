
import asyncio

import aiocoap
import aiocoap.resource as resource

class HelloWorldResource(resource.Resource):
    async def render_get(self, request):
        print("Received GET request")
        payload = "Hello there!!!\n".encode('ascii')
        return aiocoap.Message(payload=payload)

class CurrTempResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.temp = 21.5

    def encodeContent(self):
        retVal = "{:.2f}".format(self.temp)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        self.temp = float(decoded)

    async def render_get(self, request):
        self.encodeContent()
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print('PUT payload: %s' % request.payload)
        self.decodeContent(request.payload)
        self.encodeContent()
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)
    
class SetTempResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.temp = 20.0

    def encodeContent(self):
        retVal = "{:.2f}".format(self.temp)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        self.temp = float(decoded)

    async def render_get(self, request):
        self.encodeContent()
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print('PUT payload: %s' % request.payload)
        self.decodeContent(request.payload)
        self.encodeContent()
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)
    
class IsRoomOccupiedResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.is_occupied = 0

    def encodeContent(self):
        self.encoded = str(self.is_occupied).encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        self.is_occupied = int(decoded)

    async def render_get(self, request):
        self.encodeContent()
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print('PUT payload: %s' % request.payload)
        self.decodeContent(request.payload)
        self.encodeContent()
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)

class AdjustTempResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.adjustment = 0.0

    def encodeContent(self):
        retVal = "{:.2f}".format(self.adjustment)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        self.adjustment = float(decoded)

    async def render_get(self, request):
        self.encodeContent()
        return aiocoap.Message(payload=self.encoded)

async def main():

    # Specify the desired address and port
    host = "172.18.0.1"
    port = 8383

    root = resource.Site()
    root.add_resource(['hello_world'], HelloWorldResource())
    root.add_resource(['curr_temp'], CurrTempResource())
    root.add_resource(['set_temp'], SetTempResource())
    root.add_resource(['is_room_occupied'], IsRoomOccupiedResource())
    root.add_resource(['adjust_temp'], AdjustTempResource())

    await aiocoap.Context.create_server_context(root, bind=(host, port))

    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main ())