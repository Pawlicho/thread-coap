from flask import Flask, request, render_template, redirect, url_for
import sqlite3

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/save_parameter', methods=['POST'])
def save_parameter():
    name = request.form['name']
    value = request.form['value']

    conn = sqlite3.connect('mydatabase.db')
    cursor = conn.cursor()
    cursor.execute('INSERT INTO parameters (name, value) VALUES (?, ?)', (name, value))
    conn.commit()
    conn.close()

    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run()
