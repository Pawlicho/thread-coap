import datetime
import asyncio
import aiosqlite

class LogsManager:
    def __init__(self, db_path):
        self.buff_client1 = []
        self.buff_client2 = []
        self.db_path = db_path
    
    def Clear(self):
        self.buff_client1 = []
        self.buff_client2 = []

    async def PushLogs(self):
        async with aiosqlite.connect(self.db_path) as conn: 
            cursor = await conn.cursor()
            for log in self.buff_client1:
                await cursor.execute('''INSERT INTO CLIENT_1_LOGS (time, measured_temperature, sent_temperature_correction, src_IP)
                                     VALUES(:time, :measured_temperature, :sent_temperature_correction, :src_IP)''',
                                     {'time': log['time'], 'measured_temperature': log['measured_temperature'], 'sent_temperature_correction': log['sent_temperature_correction'], 'src_IP': log['src_IP']})
            for log in self.buff_client2:
                pass
            await conn.commit()
        self.Clear()
    
    def AddClient1Log(self, measured_temp, correction_temp, source_ip):
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log = {'time': timestamp, 'measured_temperature': measured_temp, 'sent_temperature_correction': correction_temp, 'src_IP': source_ip}
        self.buff_client1.append(log)

    def AddClient2Log(self):
        passf