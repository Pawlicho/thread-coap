import datetime
from datetime import datetime
import pytz

import aiosqlite

class LogsManager:
    def __init__(self, db_path):
        self.buff_heater = []
        self.buff_dimmer = []
        self.db_path = db_path
    
    def Clear(self):
        self.buff_heater = []
        self.buff_dimmer = []

    async def PushLogs(self):
        async with aiosqlite.connect(self.db_path) as conn: 
            cursor = await conn.cursor()
            for log in self.buff_heater:
                await cursor.execute('''INSERT INTO HEATER (time, msg_type, value, src_IP)
                                     VALUES(:time, :msg_type, :value, :src_IP)''',
                                     {'time': log['time'], 'msg_type': log['msg_type'], 'value': log['value'], 'src_IP': log['src_IP']})
            for log in self.buff_dimmer:
                await cursor.execute('''INSERT INTO DIMMER (time, msg_type, value, src_IP)
                                     VALUES(:time, :msg_type, :value, :src_IP)''',
                                     {'time': log['time'], 'msg_type': log['msg_type'], 'value': log['value'], 'src_IP': log['src_IP']})
            await conn.commit()
        self.Clear()
    
    def AddHeaterLog(self, msg_type, value, source_ip):
        poland_timezone = pytz.timezone('Europe/Warsaw')
        timestamp = datetime.now(poland_timezone).strftime("%Y-%m-%d %H:%M:%S")
        log = {'time': timestamp, 'msg_type': msg_type, 'value': value, 'src_IP': source_ip}
        self.buff_heater.append(log)

    def AddDimmerLog(self, msg_type, value, source_ip):
        poland_timezone = pytz.timezone('Europe/Warsaw')
        timestamp = datetime.now(poland_timezone).strftime("%Y-%m-%d %H:%M:%S")
        log = {'time': timestamp, 'msg_type': msg_type, 'value': value, 'src_IP': source_ip}
        self.buff_dimmer.append(log)