import datetime

now = datetime.datetime.now()

f = open("runDate.txt", "w")
f.write("Date and Time when this run was done: "+now.strftime("%Y-%m-%d %H:%M"))
f.close()
