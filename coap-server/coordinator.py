
import asyncio
import aiosqlite

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'

class ServerSetParameters:
    def __init__(self):
        self.set_temperature = 0

    def Update(self, new):
        self.set_temperature = float(new)

    def GetTemperature(self):
        return self.set_temperature

class ServerCurrentParameters:
    def __init__(self):
        self.cur_temperature = 0

    def Update(self, new):
        self.cur_temperature = new

    def GetTemperature(self):
        return self.cur_temperature

def calc_correction_temp(set_val, curr_val):
    correction = set_val - curr_val
    return round(correction, 2)

async def pull_data(state):
    async with aiosqlite.connect(db_path) as db:
        async with db.execute("SELECT set_temperature_1 FROM WEB_MANAGER_LOGS ORDER BY id DESC LIMIT 1") as cursor:
            new_val = await cursor.fetchone()
            state.Update(new_val[0])

async def periodic_task(state):
    while True:
        # Add your periodic task logic here
        await pull_data(state)

        # Use asyncio.sleep with await to yield control to the event loop
        await asyncio.sleep(5)  # Adjust the sleep interval as needed

