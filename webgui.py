#!/usr/bin/python

import sqlite3
import sys
import cgi
import cgitb


# global variables
speriod=(15*60)-1
dbname='/home/pi/project/sensordata.db'



# print the HTTP header
def printHTTPheader():
    print "Content-type: text/html\n\n"


def printCSS():
    print "<style>"
    print "* {box-sizing: border-box;}"
    print ".column {float: left;"
    print "width: 33%;"
    print "padding: 10px;"
    print "height: 300px; }"
    print ".row: after {content:\"\";"
    print "display: table;"
    print "clear: both;"
    print "</style>"

# print the HTML head section
# arguments are the page title and the table for the chart
def printHTMLHead(title, table):
    print "<head>"
    print "    <title>"
    print title
    print "    </title>"
    
    print_graph_script(table)

    print "</head>"


# get data from the database
# if an interval is passed, 
# return a list of records from the database
def get_data(interval):

    conn=sqlite3.connect(dbname)
    conn.text_factory = str
    curs=conn.cursor()

    if interval == None:
        #curs.execute("SELECT update_time, read_int FROM dhtreadings where read_type = 'TEMPERATURE' limit 5")
        curs.execute("SELECT * from dhtreadings where read_type = 'TEMPERATURE'")
    else:
        #curs.execute("SELECT * FROM temps WHERE timestamp>datetime('now','-%s hours')" % interval)
        curs.execute("SELECT * FROM dhtreadings WHERE update_time > datetime('now','-%s hours') and read_type = 'TEMPERATURE'" % interval)
        #curs.execute("SELECT update_time, read_int FROM dhtreadings WHERE read_type = 'TEMPERATURE' and update_time>datetime('2020-01-01 21:30:02','-%s hours') AND update_time<=datetime('2020-09-19 21:31:02') limit 5" % interval)

    rows=curs.fetchall()
    #print rows
    conn.close()

    return rows


# convert rows from database into a javascript table
def create_table(rows):
    chart_table=""

    for row in rows[:-1]:
        #print(row)
        #rowstr="['{0}', {1}],\n".format(str(row[0]), str(row[1]))
        rowstr="['{0}', {1}], \n".format(str(row[2]), str(row[5]))
        chart_table+=rowstr

    row=rows[-1]
    rowstr="['{0}', {1}]\n".format(str(row[2]), str(row[5]))
    #rowstr="['{0}', {1}]\n".format(str(row[0]),str(row[1]))
    chart_table+=rowstr

    return chart_table


# print the javascript to generate the chart
# pass the table generated from the database info
def print_graph_script(table):

    # google chart snippet
    chart_code="""
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Time', 'Temperature'],
%s
        ]);
        var options = {
          title: 'Temperature'
        };
        var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>"""

    print chart_code % (table)




# print the div that contains the graph
def show_graph():
    print "<h2>Temperature Chart</h2>"
    print '<div id="chart_div" style="width: 900px; height: 500px;"></div>'



# connect to the db and show some stats
# argument option is the number of hours
def show_stats(option):

    conn=sqlite3.connect(dbname)
    conn.text_factory = str
    curs=conn.cursor()

    if option is None:
        option = str(24)

#    curs.execute("SELECT timestamp,max(temp) FROM temps WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    curs.execute("SELECT update_time, read_type, max(read_int) FROM dhtreadings WHERE read_type = 'TEMPERATURE' and update_time>datetime('2013-09-19 21:30:02','-%s hour') AND update_time<=datetime('2020-09-19 21:31:02')" % option)
    rowmax=curs.fetchone()
    #rowstrmax="{0}&nbsp&nbsp&nbsp{1}C".format(str(rowmax[0]),str(rowmax[1]), str(rowmax[2]), str(rowmax[3]), rowmax[4])
    rowstrmax="{0}&nbsp&nbsp&nbsp{2}F&nbsp&nbsp{1}".format(str(rowmax[0]),str(rowmax[1]), rowmax[2])
#    curs.execute("SELECT timestamp,min(temp) FROM temps WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    curs.execute("SELECT update_time, read_type, min(read_int) FROM dhtreadings WHERE read_type = 'TEMPERATURE' and update_time>datetime('2013-09-19 21:30:02','-%s hour') AND update_time<=datetime('2020-09-19 21:31:02')" % option)
    rowmin=curs.fetchone()
    rowstrmin="{0}&nbsp&nbsp&nbsp{2}F&nbsp&nbsp{1}".format(str(rowmin[0]),str(rowmin[1]), rowmin[2])

#    curs.execute("SELECT avg(temp) FROM temps WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    curs.execute("SELECT avg(read_int) FROM dhtreadings WHERE read_type = 'TEMPERATURE' and update_time>datetime('2013-09-19 21:30:02','-%s hour') AND update_time<=datetime('2020-09-19 21:31:02')" % option)
    rowavg=curs.fetchone()

    curs.execute("SELECT update_time, read_type, min(read_int) FROM dhtreadings WHERE read_type = 'LIGHT' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option)
    rowlimin = curs.fetchone()
    rowlimin = "{0}&nbsp&nbsp&nbsp{2}%&nbsp&nbsp{1}".format(str(rowlimin[0]), str(rowlimin[1]), rowlimin[2])

    curs.execute("SELECT update_time, read_type, max(read_int) FROM dhtreadings WHERE read_type = 'LIGHT' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option)
    rowlimax = curs.fetchone()
    rowslightmax="{0}&nbsp&nbsp&nbsp{2}%&nbsp&nbsp{1}".format(str(rowlimax[0]),str(rowlimax[1]), rowlimax[2])

    curs.execute("SELECT avg(read_int) FROM dhtreadings WHERE read_type = 'LIGHT' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option)
    rowliavg = curs.fetchone()

    #MOISTURE
    curs.execute("SELECT update_time, read_type, min(read_int) FROM dhtreadings WHERE read_type = 'MOISTURE' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option)
    rowmomin = curs.fetchone()
    rowmomin = "{0}&nbsp&nbsp&nbsp{2}%&nbsp&nbsp{1}".format(str(rowmomin[0]), str(rowmomin[1]), rowmomin[2])

    curs.execute("SELECT update_time, read_type, max(read_int) FROM dhtreadings WHERE read_type = 'MOISTURE' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option) 
    rowmomax = curs.fetchone()
    rowmomax = "{0}&nbsp&nbsp&nbsp{2}%&nbsp&nbsp{1}".format(str(rowmomax[0]), str(rowmomax[1]), rowmomax[2])

    curs.execute("SELECT avg(read_int) FROM dhtreadings WHERE read_type = 'MOISTURE' and update_time > datetime('2013-09-18 00:00:00', '-%s hour')" % option)
    rowmoavg = curs.fetchone()

    print "<hr>"
    printCSS()
    print "<div class=\"row\">"
    print "<div class=\"column\">"
    print "<h2>Minumum temperature&nbsp</h2>"
    print rowstrmin
    #print "some rowsmin"
    print "<h2>Maximum temperature</h2>"
    print rowstrmax
    #print "some rows max"
    print "<h2>Average temperature</h2>"
    print "%.3f" % rowavg+"F"
    #print "some average logic for later"
    print "</div>"
    print "<div class=\"column\">"
    print "<h2>Min light reading</h2>"
    print rowlimin
    print "<h2>Max light reading</h2>"
    print rowslightmax
    print "<h2>Average light</h2>"
    print "%.3f" % rowliavg + "%"
    print "</div>"
    print "<div class=\"column\">"
    print "<h2>Min moisture reading</h2>"
    print rowmomin
    print "<h2>Max moisture reading</h2>"
    print rowmomax
    print "<h2>Average moisture</h2>"
    print "%.3f" % rowmoavg + "%"
    print "</div>"
    print "<hr>"

    print "<h2>Most recent 5:</h2>"
    print "<table>"
    print "<tr><td><strong>Record #</strong></td><td><strong>Sensor Name</strong></td><td><strong>Date/Time</strong></td><td><strong>Sensor Type</strong></td><td><strong>Metric</strong></td></tr>"

#    rows=curs.execute("SELECT * FROM temps WHERE timestamp>datetime('new','-1 hour') AND timestamp<=datetime('new')")
    rows=curs.execute("SELECT * FROM(SELECT * FROM dhtreadings ORDER BY update_time DESC) LIMIT 5;")
    for row in rows:    
        rowstr="<tr><td>{0}&emsp;&emsp;</td><td>{1} F</td><td>{2}</td><td>{3}</td><td>{4}</td></tr>".format(str(row[0]),str(row[1]),str(row[2]),str(row[3]), row[4])
        print rowstr
    print "</table>"

    print "<hr>"

    conn.close()




def print_time_selector(option):

    print """<form action="/cgi-bin/webgui.py" method="POST">
        Show the temperature logs for  
        <select name="timeinterval">"""


    if option is not None:

        if option == "6":
            print "<option value=\"6\" selected=\"selected\">the last 6 hours</option>"
        else:
            print "<option value=\"6\">the last 6 hours</option>"

        if option == "12":
            print "<option value=\"12\" selected=\"selected\">the last 12 hours</option>"
        else:
            print "<option value=\"12\">the last 12 hours</option>"

        if option == "24":
            print "<option value=\"24\" selected=\"selected\">the last 24 hours</option>"
        else:
            print "<option value=\"24\">the last 24 hours</option>"

        if option == "72":
            print "<option value=\"72\" selected=\"selected\">the last 72 hours</option>"
        else:
            print "<option value=\"72\">the last 72 hours</option>"

    else:
        print """<option value="6">the last 6 hours</option>
            <option value="12">the last 12 hours</option>
            <option value="24" selected="selected">the last 24 hours</option>
            <option value="72" selected="selected">the last 72 hours</options>"""
    print """        </select>
        <input type="submit" value="Display">
    </form>"""


# check that the option is valid
# and not an SQL injection
def validate_input(option_str):
    # check that the option string represents a number
    if option_str.isalnum():
        # check that the option is within a specific range
        if int(option_str) > 0 and int(option_str) <= 72:
            return option_str
        else:
            return None
    else: 
        return None


#return the option passed to the script
def get_option():
    form=cgi.FieldStorage()
    if "timeinterval" in form:
        option = form["timeinterval"].value
        return validate_input (option)
    else:
        return None




# main function
# This is where the program starts 
def main():

    cgitb.enable()

    # get options that may have been passed to this script
    option=get_option()

    if option is None:
        option = str(24)

    # get data from the database
    records=get_data(option)

    # print the HTTP header
    printHTTPheader()

    if len(records) != 0:
        # convert the data into a table
        table=create_table(records)
    else:
        print "No data found"
        return

    # start printing the page
    print "<html>"
    # print the head section including the table
    # used by the javascript for the chart
    printHTMLHead("Artemis Lab", table)

    # print the page body
    print "<body>"
    print "<h1>Artemis Lab Device Dashboard</h1>"
    print "<hr>"
    print_time_selector(option)
    show_graph()
    show_stats(option)
    print "</body>"
    print "</html>"

    sys.stdout.flush()

if __name__=="__main__":
    main()
