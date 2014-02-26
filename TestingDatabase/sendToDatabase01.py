#!/usr/bin/env python

# Posts to MESH database for testing without wifi shield

import urllib, urllib2, httplib, datetime, time, serial


PORT = 'COM10' # Port of Arduino
SPEED = 9600 # Serial communication speed; must match Arduino speed

DATAPTS = 6 # Number of data points being sent to program

DEBUG = True # Set true of output is wanted

def send_to_thingspeak(NUM, TEMP, HUM, LDR1, LDR2, PIR):
    #try:
		# params = urllib.urlencode({'node':int(NUM), 'temp':float(TEMP), 'humidity':float(HUM), 'light1':float(LDR1), 'light2':float(LDR2), 'motion':int(PIR)})
		# headers = {'Content-type': 'application/x-www-form-urlencoded', 'Accept': 'text/plain'}
		# url = 'http://mesh.org.ohio-state.edu/hook1.php'
		# results = urllib2.urlopen(url,params)
		#conn = httplib.HTTPConnection('mesh.org.ohio-state.edu')
		# conn.request('GET', '/hook1.php', params)
		# response = conn.getresponse()
		req = urllib2.Request('http://mesh.org.ohio-state.edu/hook1.php?node='+str(NUM)+'&temp='+str(TEMP)+'&humidity='+str(HUM)+'&light1='+str(LDR1)+'&light2='+str(LDR2)+'&motion='+str(PIR))
		res = urllib2.urlopen(req)
		# response = res.read()
		# if DEBUG:
			#print str(datetime.datetime.now()), response.status, response.reason
		#conn.close()
    # except:
        # if DEBUG:
            # print 'Connection to ThingSpeak Failed'

def get_data(node):
	data = node.readline()
	return data
	

if __name__ == "__main__":
	arduino = serial.Serial(PORT, SPEED) # Established Arduino connection
	while True:
		data = get_data(arduino) # Get CSV stream
		data = data.split(',') # Split into groups
		if len(data) == DATAPTS: # Checks to make sure all data points are there
			send_to_thingspeak(data[0], data[1], data[2], data[3], data[4], data[5]) # Sends data points to ThingSpeak
			if DEBUG:
				print data
			time.sleep(1) # Buffer
