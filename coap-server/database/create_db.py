import sqlite3
import time

def DropAllTables(db_path):
    with sqlite3.connect(db_path) as conn:
        cursor = conn.cursor()
        
        # Get a list of all table names in the database
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
        table_names = cursor.fetchall()

        # Iterate over the table names and drop each table except sqlite_sequence
        for table_name in table_names:
            table_name = table_name[0]
            if table_name != 'sqlite_sequence':
                cursor.execute(f"DROP TABLE IF EXISTS {table_name};")

        conn.commit()

def CreateTables(db_path):
    with sqlite3.connect(db_path) as conn:
        cursor = conn.cursor()

        CreateWemMenagerLogsTable(conn)
        CreateHeaterTable(conn)
        CreateDimmerTable(conn)

def CreateWemMenagerLogsTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS WEB_MANAGER (
                   id INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
                   set_temperature REAL NOT NULL,                   
                   set_illuminance REAL NOT NULL
                   )''')
    conn.commit()

def CreateHeaterTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS HEATER (
                   id INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
                   measured_temperature REAL NOT NULL,
                   sent_temperature_correction REAL,
                   src_IP TEXT DEFAULT 'Unknown'
                   )''')
    conn.commit()
    
def CreateDimmerTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS DIMMER (
                   id INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
                   measured_illuminance REAL NOT NULL,
                   sent_illuminance_correction REAL,
                   src_IP TEXT DEFAULT 'Unknown'
                   )''')
    conn.commit()

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'
DropAllTables(db_path)
CreateTables(db_path)
# with sqlite3.connect(db_path) as conn:
#     cursor = conn.cursor()
#     cursor.execute('''SELECT * FROM HEATER''')
#     a = cursor.fetchall()
#     # print(type(a))
#     # for i in range(0, 10):
#     #     cursor.execute('''INSERT INTO HEATER (measured_temperature, sent_temperature_correction) VALUES(:measured_temperature, :sent_temperature_correction)''',
#     #                 {'measured_temperature':i, 'sent_temperature_correction': i / 2 * -1})
#     #     conn.commit()
#     #     time.sleep(3)
#     cursor.execute('''SELECT * FROM HEATER ORDER BY id DESC LIMIT 1''')
#     a = cursor.fetchall()
#     print(a)
