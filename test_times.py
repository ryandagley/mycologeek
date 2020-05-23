#######################################################
# test_times.py - check time in database and OS       
#######################################################

import sqlite3
import datetime

dbname='/home/pi/project/sensordata.db'
now = datetime.datetime.now()

def get_data():
    conn=sqlite3.connect(dbname)
    curs = conn.cursor()

    curs.execute("SELECT datetime('now') FROM dhtreadings")
    rows = curs.fetchone()
    conn.close()

    print("Time in the database is: " + str(rows))
    print("Time in OS is: " + str(now))

get_data()
