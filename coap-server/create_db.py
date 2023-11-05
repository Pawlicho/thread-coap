import sqlite3
import time

def DropAllTables(db_name):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()

    
    # Get a list of all table names in the database
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    table_names = cursor.fetchall()

    # Iterate over the table names and drop each table
    for table_name in table_names:
        table_name = table_name[0]  # Extract the table name from the result
        cursor.execute(f"DROP TABLE IF EXISTS {table_name};")

    conn.commit()
    conn.close()

def CreateTables(db_name):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()

    CreateWemMenagerLogsTable(conn)
    CreateClient1LogsTable(conn)
    CreateClient2LogsTable(conn)

    conn.close()

def CreateWemMenagerLogsTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS WEB_MANAGER_LOGS (
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP PRIMARY KEY,
                   set_temperature_1 REAL NOT NULL,                   
                   set_temperature_2 REAL NOT NULL
                   )''')
    conn.commit()

def CreateClient1LogsTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS CLIENT_1_LOGS (
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP PRIMARY KEY,
                   measured_temperature REAL NOT NULL,
                   sent_temperature_correction REAL,
                   src_IP TEXT DEFAULT 'Unknown'
                   )''')
    conn.commit()
    
def CreateClient2LogsTable(conn):
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS CLIENT_2_LOGS (
                   time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP PRIMARY KEY,
                   measured_temperature REAL NOT NULL,
                   sent_temperature_correction REAL,
                   src_IP TEXT DEFAULT 'Unknown'
                   )''')
    conn.commit()

# CreateTables('thread_coap_database.db')
# conn = sqlite3.connect('thread_coap_database.db')
# cursor = conn.cursor()
# cursor.execute('''SELECT * FROM CLIENT_1_LOGS''')
# a = cursor.fetchall()
# print(type(a))
# for i in range(0, 10):
#     cursor.execute('''INSERT INTO CLIENT_1_LOGS (measured_temperature, sent_temperature_correction) VALUES(:measured_temperature, :sent_temperature_correction)''',
#                    {'measured_temperature':i, 'sent_temperature_correction': i / 2 * -1})
#     conn.commit()
#     time.sleep(3)
# cursor.execute('''SELECT * FROM WEB_MANAGER_LOGS ORDER BY ABS(strftime('%s', 'now') - strftime('%s', time)) LIMIT 1''')
# a = cursor.fetchone()
# print(a)
# conn.close()
