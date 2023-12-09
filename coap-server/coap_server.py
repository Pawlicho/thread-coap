
import asyncio

import aiocoap
import aiocoap.resource as resource

from coordinator import periodic_task, ServerSetParameters, ServerCurrentParameters, calc_heater_reg_param, calc_dimmer_reg_param
from logs_manager import LogsManager
from database.create_db import CreateTables, DropAllTables

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'
server_set_params = ServerSetParameters()
server_cur_params = ServerCurrentParameters()
logs_manager = LogsManager(db_path)

class TemperatureResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.temperature = 21.5

    def encode_content(self, value):
        ret_val = "{:.2f}".format(value)
        self.encoded = ret_val.encode('ascii')

    def decode_content(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.temperature = float(decoded)

    async def render_get(self, request):
        print("\nReceived temperature.GET.Req\n")
        self.encode_content(self.temperature)
        print("\nResponding with temperature.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived temperature.PUT.Req")

        # Decode received request's payload from ascii, convert to temp format
        self.decode_content(request.payload)
        print("\nNew current_temp value on the server side: " + str(self.temperature))

        # Update current temperature value from client req
        server_cur_params.UpdateHeater(self.temperature)

        # Log request
        logs_manager.AddHeaterLog(msg_type="current_temperature",
                                  value=self.temperature,
                                  source_ip=request.remote.hostinfo)

        # Send the response to client
        # Echo received payload
        print("\nResponding with temperature.PUT.Rsp\n")
        return aiocoap.Message(code=aiocoap.CHANGED, payload=request.payload)
    
class HeaterRegulationResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.current_power = 20

    def encode_content(self, value):
        ret_val = "{:.1f}".format(value)
        self.encoded = ret_val.encode('ascii')

    def decode_content(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.current_power = float(decoded)

    async def render_get(self, request):
        print("\nReceived heater_regulation.GET.Req\n")

        # Decode received power, acquire current power
        self.decode_content(request.payload)

        # Calculate regulation parameter
        regulation_parameter = calc_heater_reg_param(server_set_params.GetTemperature(), server_cur_params.GetTemperature(), self.current_power)

        # Encode regulation parameter
        self.encode_content(regulation_parameter)

        # Log request
        logs_manager.AddHeaterLog(msg_type="heater_regulation",
                                  value=regulation_parameter,
                                  source_ip=request.remote.hostinfo)

        print("\nResponding with heater_regulation.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
class IlluminanceResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.illuminance = 300

    def encode_content(self, value):
        ret_val = "{:.1f}".format(value)
        self.encoded = ret_val.encode('ascii')

    def decode_content(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.illuminance = float(decoded)

    async def render_get(self, request):
        print("\nReceived dimmer.GET.Req\n")
        self.encode_content(self.illuminance)
        print("\nResponding with dimmer.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)
    
    async def render_put(self, request):
        print("\nReceived dimmer.PUT.Req")

        # Decode received request's payload from ascii, convert to temp format
        self.decode_content(request.payload)
        print("\nNew current_illuminance value on the server side: " + str(self.illuminance))

        # Update current illuminance value from client req
        server_cur_params.UpdateDimmer(self.illuminance)

        # Log request
        logs_manager.AddDimmerLog(msg_type="current_illuminance",
                                  value=self.illuminance,
                                  source_ip=request.remote.hostinfo)
        
        # Send the response to client
        # Echo request payload
        print("\nResponding with dimmer.PUT.Rsp\n")
        return aiocoap.Message(code=aiocoap.CHANGED, payload=request.payload)

class DimmerRegulationResource(resource.Resource):
    def __init__(self):
        super().__init__()
        self.current_power = 20

    def encode_content(self, value):
        ret_val = "{:.1f}".format(value)
        self.encoded = ret_val.encode('ascii')

    def decode_content(self, payload):
        decoded = payload.decode('ascii')
        decoded = decoded.strip('\x00')
        self.current_power = float(decoded)

    async def render_get(self, request):
        print("\nReceived dimmer_regulation.GET.Req\n")

        # Decode received power, acquire current power
        self.decode_content(request.payload)

        # Calculate regulation parameter
        regulation_parameter = calc_dimmer_reg_param(server_set_params.GetIlluminance(), server_cur_params.GetIlluminance(), self.current_power)

        # Encode regulation parameter
        self.encode_content(regulation_parameter)

        # Log request
        logs_manager.AddDimmerLog(msg_type="heater_regulation",
                                  value=regulation_parameter,
                                  source_ip=request.remote.hostinfo)

        print("\nResponding with dimmer_regulation.GET.Rsp\n")
        return aiocoap.Message(payload=self.encoded)

async def main():

    # Create Database if does not exist
    CreateTables(db_path=db_path)

    # Server address and port number
    host = "172.18.0.1"
    port = 8383

    # Set up resources on server side
    root = resource.Site()
    root.add_resource(['temperature'], TemperatureResource())
    root.add_resource(['illuminance'], IlluminanceResource())
    root.add_resource(['heater_regulation'], HeaterRegulationResource())
    root.add_resource(['dimmer_regulation'], DimmerRegulationResource())

    # Routine for database set parameters pulling
    periodic_task_coroutine = asyncio.create_task(periodic_task(server_set_params, logs_manager))

    await aiocoap.Context.create_server_context(root, bind=(host, port))

    # Await for periodic, infinite routine
    await periodic_task_coroutine

    # Stay in the loop forever
    await asyncio.get_event_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main())