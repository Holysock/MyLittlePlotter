#!/usr/bin/env python
# -*- coding: utf-8 -*-

#Filter: ALLES was nicht mit G00 G01 G02 G03 anfaengt
#Filter: Optional mit Z

import serial
import sys, os, time
import subprocess as sp

W  = '\033[0m'  # white (normal)
R  = '\033[31m' # red
G  = '\033[32m' # green
O  = '\033[33m' # orange
B  = '\033[34m' # blue
P  = '\033[35m' # purple
C  = '\033[36m' # cyan
GR = '\033[37m' # gray

sp.call("clear",shell = True)
print "Welcome at Nick's super duper mega plotter gcode parser" + P
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
print W + ""

filterForZ = True
debugging = False
usegui = False
port = ""
baud = 0
filename = ""
depth = 0
deltime = 0
configed = False
manualZ = False 

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
	if filterForZ and sys.argv[8] == "True":
		manualZ = True	
except:
	print "Usage: " + R + "[\"port\"] [baudrate] [\"path-to-gcode\"] [Dropping chars] [Debugging] [deltime] [Use-Z-Axis] [Manual-Z-Axis]"
	exit()  

print B + "Trying to connect to the device at " + port
ser = serial.Serial()
ser.baudrate = baud
ser.port = port
ser.setTimeout(0.5)

try:
	ser.open()
except:
	print R + "Unable to connect..."
	print R + "Try to reconnect the device or run as root/fakeroot"
	exit()
print G + "Connection established with " + baud + " baud"
ser.write("on")
time.sleep(2)
ser.write("G28")
raw_input(C + "Homed?")

#ser.write("T=10")
#time.sleep(0.5)
#ser.write("thisIsHome")
#time.sleep(1)
try:
	target = open(filename)
except:
	print R + "File " + filename + " not found..."

x = []
v = ""



while not v is None:
	try:
		v = target.next()
		if(debugging): print v
		x.append(v)
	except:
		print G + "Finished reading file"
		v = None
k = l = ' '
while not k == '':
	k = ser.read()
	l += k
print l
for i in xrange(len(x)):
	if(debugging): print W + "for"
	use = True
	if not "G00" in x[i] and not "G01" in x[i] and not "G02" in x[i] and not "G03" in x[i]:
		use = False
	if filterForZ and "Z" in x[i]:
		use = False
		if manualZ:
			print x[i]
			raw_input(C + "Done moving Z-Axis?")
	if(debugging): print W + "USE: " + str(use) 
	if use:
		ser.write(x[i][:-depth])
		print(G + "Done " + str((i/(len(x)*1.0))*100) + "% ")
		if debugging: print W + "w"
		if debugging: print W + x[i][:-depth]
		o = True
		while o:
			i = ser.read()
			if debugging: print W + i
			if(i == 'D'):
				o = False
		v = True
		while v:
			if ser.read() == '.': v = False
		v = True
		while v:
			if ser.read() == '': v = False
		time.sleep(deltime)
		
print G + "*DONE!!*"

def write(text):
	sys.stdout.write(text)
	sys.stdout.flush()
