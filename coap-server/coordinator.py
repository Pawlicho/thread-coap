
import asyncio
import aiosqlite
from logs_manager import LogsManager

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'

class ServerSetParameters:
    def __init__(self):
        self.set_temperature = 0
        self.set_illuminance = 0

    def UpdateHeater(self, new):
        self.set_temperature = float(new)

    def UpdateDimmer(self, new):
        self.set_illuminance = new

    def GetTemperature(self):
        return self.set_temperature
    
    def GetIlluminance(self):
        return self.set_illuminance

class ServerCurrentParameters:
    def __init__(self):
        self.cur_temperature = 0
        self.set_illuminance = 0

    def UpdateHeater(self, new):
        self.cur_temperature = new

    def UpdateDimmer(self, new):
        self.set_illuminance = new

    def GetTemperature(self):
        return self.cur_temperature

    def GetIlluminance(self):
        return self.set_illuminance

def calc_heater_reg_param(set_val, curr_val, curr_power):
    
    delta_t = set_val - curr_val

    if delta_t < 0:
        reg_param = 0.0
    elif delta_t >= 10:
        reg_param = 100.0
    elif delta_t < 10 and delta_t >= 9:
        reg_param = 90.0
    elif delta_t < 9 and delta_t >= 8:
        reg_param = 80.0
    elif delta_t < 7 and delta_t >= 6:
        reg_param = 70.0
    elif delta_t < 6 and delta_t >= 5:
        reg_param = 60.0
    elif delta_t < 5 and delta_t >= 5:
        reg_param = 50.0
    elif delta_t < 4 and delta_t >= 4:
        reg_param = 40.0
    elif delta_t < 3 and delta_t >= 2:
        reg_param = 30.0
    elif delta_t < 2 and delta_t >= 1:
        reg_param = 20.0
    elif delta_t < 1 and delta_t >= 0.5:
        reg_param = 10.0
    else:
        reg_param = 0.0

    print("Returning: " + str(reg_param))
    # return round(reg_param, 2)
    return reg_param

def calc_dimmer_reg_param(set_val, curr_val, curr_power):
    reg_param = 10
    return reg_param

async def pull_data(state):
    async with aiosqlite.connect(db_path) as db:
        async with db.execute("SELECT set_temperature, set_illuminance FROM WEB_MANAGER ORDER BY id DESC LIMIT 1") as cursor:
            new_val = await cursor.fetchone()
            set_illuminance = new_val[1]
            heater_params = new_val[0]
            state.UpdateHeater(heater_params)
            state.UpdateDimmer(set_illuminance)

async def periodic_task(state, log_manager):
    while True:
        # Add your periodic task logic here
        await pull_data(state)

        await log_manager.PushLogs()
        print("Pushed logs into Database")

        # Use asyncio.sleep with await to yield control to the event loop
        await asyncio.sleep(5)  # Adjust the sleep interval as needed

