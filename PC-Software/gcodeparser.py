#!/usr/bin/env python
# -*- coding: utf-8 -*-

#Filter: ALLES was nicht mit G00 G01 G02 G03 anfaengt
#Filter: Optional mit Z

import serial
import sys, os, time
import subprocess as sp

sp.call("clear",shell = True)
print "Welcome at Nick's super duper mega plotter gcode parser"
print "                                  ▓▓▓▓▓▓"
print "                         ▓▓▓▓▓▓░░░░░░░▓▓▓▓▓"
print "                     ▓▓▓▒▒▒▒▒▒▒░░░░░▓▓▓▓▓▓▓▓▓"
print "      ▒▒         ▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒░░░░░▓   ▓▓▓"
print "    ▒▒▒▒▒   ▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒░░░▓"
print "   ▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▒▒░▓▓"
print "  ▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▒▒▒▓▓"
print "  ▒▒▒▓▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▓▓▒▒▒▓"
print " ▒▒▒▒▓▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▒▒▒▓"
print " ▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▓▓▓▓"
print "  ▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▓▓"
print "  ▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▓"
print "   ▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▓▓▓▓▓▒▒▒▒▒▒▓"
print "    ▒▒▒▒▒▒▅▅▄▒▒▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▒▒▒▓▓▓▒▒▒▒▒▓▓"
print "   ▓░▒▒▒▒▅▅▅▅█▀▀▀▅▅▅▅▄▄▒▒▒▒▓▒█▒▒▄▒▒▒▄▅▓▒▒▒▒▒▓"
print "   ▓░░▒▒▒▒▄▅▅▌        ▓ ▀▀██▅▄█▒▒▀▀▀  █"
print "  ▓░░░░▓▒▒▒▒▒         ▓    ▐██▒▒▒▒▒▌▓  █"
print "  ▓░░░░▓▒▒▒▒▒         ▓  ▄███▌▒▒▒▒▐ ▓██"
print "  ▓░░░░░▓▒▒▒▒▒        ▓▓█  ██▒▒▒▒▒▒▓▌▐"
print " ▓▒▒░░░░░▓▒▒▒▒▒       ▓█████▒▒▒▒▒▒▒██"
print " ▓▒▒░░░░░░▓▓▓▒▒▒▒     ▓███▀▒▒▒▒▒▒▒▒▒▒▒"
print " ▓▒▒▒░░░░░░░░░▓▓▓▓▒▒  ▓▒▒▒▒▒▒▒ ▒▒▀▀▒▒"
print " ▓▒▒▒▒░░░░░░░░░▓▓▒▒▒▒▒▒▒▒▒▒▒▒   ▒▒▒▒"
print "  ▓▒▒▒▒▒░░░░▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▄▀"
print "  ▓▓▒▒▒▒▒░░░░░▓    ▒▒▒▒▒▒▒▒▒▒▒▀"
print ""
print ""

filterForZ = True;
debugging = False
usegui = False
port = ""
baud = 0
filename = ""
depth = 0
deltime = 0
configed = False
try:
	port = sys.argv[1]
	baud = sys.argv[2]
	filename = sys.argv[3]
	depth = int(sys.argv[4])
	if sys.argv[5] == "True":
		debugging = True
	deltime = int(sys.argv[6])/1000.0
	if sys.argv[7] == "True":
		filterForZ = True	
except:
	print "Usage: [\"port\"] [baudrate] [\"path-to-gcode\"] [Dropping chars] [Debugging] [deltime] [Use-Z-Axis]"
	exit()  



print "Trying to connect to the device at " + port
ser = serial.Serial()
ser.baudrate = baud
ser.port = port
ser.setTimeout(0.5)

try:
	ser.open()
except:
	print "Unable to connect..."
	print "Try to reconnect the device or run as root/fakeroot"
	exit()
print "Connection established with " + baud + " baud"
ser.write("on")
time.sleep(2)
ser.write("G28")
print "Waiting for home"
time.sleep(10.0)
print "Homed"

#ser.write("T=10")
#time.sleep(0.5)
#ser.write("thisIsHome")
#time.sleep(1)
try:
	target = open(filename)
except:
	print "File " + filename + " not found..."

x = []
v = ""



while not v is None:
	try:
		v = target.next()
		x.append(v)
	except:
		print "Filed readed succesfully..."
		v = None
k = l = ' '
while not k == '':
	k = ser.read()
	l += k
print l
for i in xrange(len(x)):
	use = True
	if not "G00" in x[i] and not "G01" in x[i] and not "G02" in x[i] and not "G03" in x[i]:
		use = False
	if filterForZ and "Z" in x[i]:
		use = False
	if use:
		ser.write(x[i][:-depth])
		print "Done " + str((i/(len(x)*1.0))*100) + "% "
		if debugging: print "w"
		if debugging: print x[i][:-depth]
		o = True
		while o:
			i = ser.read()
			if debugging: print i
			if(i == 'D'):
				o = False
		v = True
		while v:
			if ser.read() == '.': v = False
		v = True
		while v:
			if ser.read() == '': v = False
		time.sleep(deltime)
		
print "DONE!!"

