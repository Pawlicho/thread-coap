import datetime
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
                await cursor.execute('''INSERT INTO HEATER (time, measured_temperature, sent_temperature_correction, src_IP)
                                     VALUES(:time, :measured_temperature, :sent_temperature_correction, :src_IP)''',
                                     {'time': log['time'], 'measured_temperature': log['measured_temperature'], 'sent_temperature_correction': log['sent_temperature_correction'], 'src_IP': log['src_IP']})
            for log in self.buff_dimmer:
                await cursor.execute('''INSERT INTO DIMMER (time, measured_illuminance, sent_illuminance_correction, src_IP)
                                     VALUES(:time, :measured_illuminance, :sent_illuminance_correction, :src_IP)''',
                                     {'time': log['time'], 'measured_illuminance': log['measured_illuminance'], 'sent_illuminance_correction': log['sent_illuminance_correction'], 'src_IP': log['src_IP']})
            await conn.commit()
        self.Clear()
    
    def AddHeaterLog(self, measured_temp, correction_temp, source_ip):
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log = {'time': timestamp, 'measured_temperature': measured_temp, 'sent_temperature_correction': correction_temp, 'src_IP': source_ip}
        self.buff_heater.append(log)

    def AddDimmerLog(self, measured_illuminance, correction_illuminance, source_ip):
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log = {'time': timestamp, 'measured_illuminance': measured_illuminance, 'sent_illuminance_correction': correction_illuminance, 'src_IP': source_ip}
        self.buff_dimmer.append(log)