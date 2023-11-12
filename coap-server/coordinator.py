
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

def calc_correction_temp(set_val, curr_val):
    correction = set_val - curr_val
    return round(correction, 2)

def calc_correction_illuminance(set_val, curr_val):
    correction = set_val - curr_val
    return round(correction, 1)

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

