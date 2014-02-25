#!/usr/bin/env python

# Post data to ThingSpeak from one serial output string from Arduino

import urllib, urllib2, httplib, datetime, time, serial

KEY = 'ODFI3NCHSTHWOAV7'
PORT = '/dev/ttyACM0' # Port of Arduino
SPEED = 9600 # Serial communication speed; must match Arduino speed

DATAPTS = 3 # Number of data points being sent to program

DEBUG = True # Set true of output is wanted

def send_to_thingspeak(TMP, LDR, CO):
    try:
		params = urllib.urlencode({'field1':float(TMP), 'field2':float(LDR), 'field3':float(CO), 'key':KEY})
		headers = {'Content-type': 'application/x-www-form-urlencoded', 'Accept': 'text/plain'}
		conn = httplib.HTTPConnection('api.thingspeak.com')
		conn.request('POST', '/update', params, headers)
		response = conn.getresponse()
		if DEBUG:
			print str(datetime.datetime.now()), response.status, response.reason
		conn.close()
    except:
        if DEBUG:
            print 'Connection to ThingSpeak Failed'

def get_data(node):
	data = node.readline()
	return data
	

if __name__ == "__main__":
	arduino = serial.Serial(PORT, SPEED) # Established Arduino connection
	while True:
		data = get_data(arduino) # Get CSV stream
		data = data.split(',') # Split into groups
		if len(data) == DATAPTS: # Checks to make sure all data points are there
			send_to_thingspeak(data[0], data[1], data[2]) # Sends data points to ThingSpeak
			if DEBUG:
				print data
			time.sleep(16) # Minimum wait of 15 sec
