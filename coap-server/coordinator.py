
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

    if delta_t < 0.0:
        reg_param = 0.0
    elif delta_t >= 10.0:
        reg_param = 100.0
    elif delta_t < 10.0 and delta_t >= 9.0:
        reg_param = 90.0
    elif delta_t < 9.0 and delta_t >= 8.0:
        reg_param = 80.0
    elif delta_t < 8.0 and delta_t >= 7.0:
        reg_param = 70.0
    elif delta_t < 7.0 and delta_t >= 6.0:
        reg_param = 60.0
    elif delta_t < 6.0 and delta_t >= 5.0:
        reg_param = 50.0
    elif delta_t < 5.0 and delta_t >= 4.0:
        reg_param = 40.0
    elif delta_t < 4.0 and delta_t >= 3.0:
        reg_param = 30.0
    elif delta_t < 3.0 and delta_t >= 1.0:
        reg_param = 20.0
    elif delta_t < 1.0 and delta_t >= 0.1:
        reg_param = 10.0
    else:
        reg_param = 0.0

    return reg_param

def calc_dimmer_reg_param(set_val, curr_val, curr_power):
    delta_i = set_val - curr_val
    tolerance = 10

    if abs(delta_i) < tolerance:
        return curr_power

    new_power = curr_power

    if delta_i < 0.0:
        new_power -= 5.0
    else:
        new_power += 5.0

    if new_power < 0:
        new_power = 0.0
    if new_power > 100.0:
        new_power = 100.0

    return new_power

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

