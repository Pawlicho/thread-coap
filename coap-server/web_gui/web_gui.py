from flask import Flask, render_template, request, redirect, url_for
import sqlite3

app = Flask(__name__)

db_path = '/home/tymoczko/src/thread-coap/coap-server/database/thread_coap_database.db'

def insert_data(temperature, illuminance):
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("INSERT INTO WEB_MANAGER (set_temperature, set_illuminance) VALUES (?, ?)",
                   (temperature, illuminance))
    conn.commit()
    conn.close()

def get_web_manager_last_update():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM WEB_MANAGER ORDER BY id DESC LIMIT 1")
    last_update = cursor.fetchone()
    conn.close()
    return last_update

def get_heater_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM HEATER ORDER BY id DESC")
    logs = cursor.fetchall()
    conn.close()
    return logs

def get_dimmer_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM DIMMER ORDER BY id DESC")
    logs = cursor.fetchall()
    conn.close()
    return logs

@app.route('/')
def index():
    last_update = get_web_manager_last_update()
    return render_template('index.html', last_update=last_update)

@app.route('/submit', methods=['POST'])
def submit():
    temperature = request.form['temperature']
    illuminance = request.form['illuminance']
    insert_data(temperature, illuminance)
    last_update = get_web_manager_last_update()
    return render_template('index.html', last_update=last_update)

@app.route('/logs/heater')
def heater_logs():
    logs_data = get_heater_logs()
    return render_template('heater_logs.html', logs_data=logs_data)

@app.route('/logs/dimmer')
def dimmer_logs():
    logs_data = get_dimmer_logs()
    return render_template('dimmer_logs.html', logs_data=logs_data)

@app.route('/clear/dimmer', methods=['POST'])
def clear_dimmer_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("DELETE FROM DIMMER")
    conn.commit()
    conn.close()
    return redirect(url_for('dimmer_logs'))

@app.route('/clear/heater', methods=['POST'])
def clear_heater_logs():
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("DELETE FROM HEATER")
    conn.commit()
    conn.close()
    return redirect(url_for('heater_logs'))

if __name__ == '__main__':
    app.run(debug=True)
