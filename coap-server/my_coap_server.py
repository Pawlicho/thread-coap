
import asyncio

import aiocoap
import aiocoap.resource as resource

class HelloWorldResource(resource.Resource):
    async def render_get(self, request):
        print("Received GET request")
        payload = "Hello there!!!\n".encode('ascii')
        return aiocoap.Message(payload=payload)    

async def main():

    # Specify the desired address and port
    host = "172.18.0.1"
    port = 8383

    root = resource.Site()
    root.add_resource(['hello_world'], HelloWorldResource())

    await aiocoap.Context.create_server_context(root, bind=(host, port))

    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main ())