
import asyncio

import aiocoap
import aiocoap.resource as resource

import ssl

from coordinator import periodic_task, ServerSetParameters, ServerCurrentParameters, calc_correction_temp, calc_correction_illuminance
from logs_manager import LogsManager

dtls_enabled = False

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
        print("\nReceived heater.GET.Req\n")
        self.encodeContent(self.temperature)
        print("\Responding with heater.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived heater.PUT.Req")

        # Decode received request's payload from ascii, convert to temp format
        self.decodeContent(request.payload)
        print("New current_temp value on the server side: " + str(self.temperature))

        # Update current temperature value from client req
        server_cur_params.UpdateHeater(self.temperature)

        # Calculate temperature correction to send in resp
        correction = calc_correction_temp(server_set_params.GetTemperature(), server_cur_params.GetTemperature())

        # Encode response payload in ascii
        self.encodeContent(correction)

        # Log request
        logs_manager.AddHeaterLog(self.temperature, correction, request.remote.hostinfo)
        

        # Send the response to client
        print("Responding with heater.PUT.Rsp\n")
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.encoded)
    
class DimmerResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.illuminance = 300

    def encodeContent(self, value):
        retVal = "{:.1f}".format(value)
        self.encoded = retVal.encode('ascii')

    def decodeContent(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.illuminance = float(decoded)

    async def render_get(self, request):
        print("\nReceived dimmer.GET.Req\n")
        self.encodeContent(self.illuminance)
        print("\Responding with dimmer.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived dimmer.PUT.Req")

        # Decode received request's payload from ascii, convert to temp format
        self.decodeContent(request.payload)
        print("New current_illuminance value on the server side: " + str(self.illuminance))

        # Update current illuminance value from client req
        server_cur_params.UpdateDimmer(self.illuminance)

        # Calculate illuminance correction to send in resp
        correction = calc_correction_illuminance(server_set_params.GetIlluminance(), server_cur_params.GetIlluminance())

        # Encode response payload in ascii
        self.encodeContent(correction)

        # Log request
        logs_manager.AddHeaterLog(self.illuminance, correction, request.remote.hostinfo)
        
        # Send the response to client
        print("Responding with dimmer.PUT.Rsp\n")
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

    if dtls_enabled:
        # Path to the server's private key and public key certificate
        private_key_path = '/home/tymoczko/src/thread-coap/coap-server/private-key.pem'
        certificate_path = '/home/tymoczko/src/thread-coap/coap-server/certificate.pem'

        # Create an SSL context with server credentials
        server_credentials = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        server_credentials.load_cert_chain(certfile=certificate_path, keyfile=private_key_path)

        # Start Server
        await aiocoap.Context.create_server_context(root, bind=(host, port), transports=['tinydtls_server'], server_credentials=server_credentials)
    else:
        await aiocoap.Context.create_server_context(root, bind=(host, port))

    # Await for periodic, infinite routine
    await periodic_task_coroutine

    # Stay in the loop forever
    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main ())