import sqlite3

conn = sqlite3.connect('mydatabase.db')
cursor = conn.cursor()
# Create a table to store parameters
cursor.execute('''CREATE TABLE IF NOT EXISTS parameters (
                  id INTEGER PRIMARY KEY,
                  name TEXT,
                  value TEXT
                )''')
conn.commit()
conn.close()
