from flask import Flask, render_template, request
import sqlite3

app = Flask(__name__)

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'

def insert_data(temperature1, temperature2):
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("INSERT INTO WEB_MANAGER_LOGS (set_temperature_1, set_temperature_2) VALUES (?, ?)",
                   (temperature1, temperature2))
    conn.commit()
    conn.close()

def get_web_manager_last_update():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM WEB_MANAGER_LOGS ORDER BY time DESC LIMIT 1")
    last_update = cursor.fetchone()
    conn.close()
    return last_update

def get_client_1_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM CLIENT_1_LOGS")
    logs = cursor.fetchall()
    conn.close()
    return logs

def get_client_2_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM CLIENT_2_LOGS")
    logs = cursor.fetchall()
    conn.close()
    return logs

@app.route('/')
def index():
    last_update = get_web_manager_last_update()
    return render_template('index.html', last_update=last_update)

@app.route('/submit', methods=['POST'])
def submit():
    temperature1 = request.form['temperature1']
    temperature2 = request.form['temperature2']
    insert_data(temperature1, temperature2)
    last_update = get_web_manager_last_update()
    return render_template('index.html', last_update=last_update)

@app.route('/logs/client1')
def logs_client1():
    logs_data = get_client_1_logs()
    return render_template('logs_client1.html', logs_data=logs_data)

@app.route('/logs/client2')
def logs_client2():
    logs_data = get_client_2_logs()
    return render_template('logs_client2.html', logs_data=logs_data)

if __name__ == '__main__':
    # CreateTables(db_path)
    app.run(debug=True)
