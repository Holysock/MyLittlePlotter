import plotlib
import pygame, sys, time

try:
	img_path = sys.argv[1]
	DPI = int(sys.argv[2])
except(IndexError):
	print "USAGE: python image_to_gcode.py [img_path] [resolution] > filename_of_your_gcode_file.gcode"
	quit()

img = pygame.image.load(img_path)

img_height = img.get_height()
img_width = img.get_width()


plotter = plotlib.plot(img_width, img_height, autof = False)

plotter.plotimage(img_path)
plotter.show()

lines = []
for y in xrange(img_height):
	line = []
	for x in xrange(img_width):
		r = plotter.getcolor(x,y)[0]
		g = plotter.getcolor(x,y)[1]
		b = plotter.getcolor(x,y)[2]
		if(r > 100 and g > 100 and b > 100):
			v = 0
			plotter.setColor(0,0,0)
			plotter.plotline(x,y,x,y)
		elif( r < 100 and g < 100 and b < 100):
			v = 1
			plotter.setColor(255,255,255)
			plotter.plotline(x,y,x,y)
		line.append(v)
	lines.append(line)


def conv_line_to_list(line):
	s = sb = c = 0
	list = []
	for i in range(len(line)):
		s = line[i]
		
		if(s == sb):
			c += 1
		elif(c > 0):
			list.append((c/(DPI * 1.0))*25.4)
			s = sb = c = 0
		sb = s
	sum = 0
	for i in range(len(list)):
		sum += list[i]
	if(sum < (img_width/(DPI*1.0))*25.4):
		list.append(((img_width/(DPI*1.0))*25.4)-sum)
	return list


def conv_list_to_gcode(list,y):
	#print list
	line_length = len(list)
	pos = 0
	gcode = ""
	gcode += "M300 0\nG00 X0 Y" + str(y) + "\n"
	for i in range(0,line_length):
		pos += list[i]
		is_white = (i % 2 == 0)
		if(is_white):
			gcode += ("M300 100") + "\n" #ANPASSEN!
		else:
			gcode += ("M300 0") + "\n" #ANPASSEN!
		gcode += ("G00 X" + str(pos) + "\n")
	return gcode

for y in xrange(img_height):
	print "(Line " + str(y) + ")"
	print conv_list_to_gcode(conv_line_to_list(lines[y]),(y/(DPI*1.0) * 25.4))
		

print "(THANKS FOR USING IMAGE_TO_GCODE!!!!!!)"

plotter.show()
