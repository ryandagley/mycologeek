#######################################################
# test_times.py - check time in database and OS       
#######################################################

import sqlite3
import datetime

dbname='/home/pi/project/sensordata.db'
now = datetime.datetime.now()

time_sql = "SELECT datetime('now') FROM dhtreadings"

def get_data():
    conn=sqlite3.connect(dbname)
    curs = conn.cursor()

    curs.execute(time_sql)
    rows = curs.fetchone()
    conn.close()

    return rows[0]

def main():
    db_time = get_data()
    print("Time in the database is: " + str(db_time))
    print("Time in OS is: " + str(now))
    datetime_db_time = datetime.datetime.strptime(db_time, '%Y-%m-%d %H:%M:%S')
    difference = datetime_db_time - now
    print("Difference in time is: " + str(difference))

if __name__=="__main__":
    main()
