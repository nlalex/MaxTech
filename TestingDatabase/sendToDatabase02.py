#!/usr/bin/env python

# Posts to MESH database for testing without wifi shield

import urllib, urllib2, httplib, datetime, time, serial, requests


#PORT = '/dev/ttyACM0' # Port of Arduino
PORT = 'COM10'
SPEED = 9600 # Serial communication speed; must match Arduino speed

DATAPTS = 6 # Number of data points being sent to program

DEBUG = True # Set true of output is wanted

def sendToDatabase(NUM, TEMP, HUM, LDR1, LDR2, PIR):
    try:
		params = {'node':int(NUM), 'temp':float(TEMP), 'humidity':float(HUM), 'light1':float(LDR1), 'light2':float(LDR2), 'motion':int(PIR)}
		requests.get(url='www.mesh.org.ohio-state.edu/hook1.php', data=params)
    except:
        if DEBUG:
            print 'Connection to Database Failed'

def get_data(node):
	data = node.readline()
	return data
	

if __name__ == "__main__":
	arduino = serial.Serial(PORT, SPEED) # Established Arduino connection
	while True:
		data = get_data(arduino) # Get CSV stream
		data = data.split(',') # Split into groups
		if len(data) == DATAPTS: # Checks to make sure all data points are there
			sendToDatabase(data[0], data[1], data[2], data[3], data[4], data[5]) # Sends data points to ThingSpeak
			if DEBUG:
				print data

