# MyLittlePoltter

#PC-Software by OleLL
Usage: ["port"] [baudrate] ["path-to-gcode"] [Dropping chars] [Debugging] [deltime]
Example: <sudo> python gcodeparser.py "/dev/ttyUSB0" 9600 "gcode.nc" 1 False 500

Description:
["port"] -> Serial port -> /dev/ttyUSBX
[baudrate] -> Baudrate -> 9600
["path-to-gcode"] -> PATH-TO-GCODE!!!!
[Dropping chars] -> How many characters at the end of the line to be dropped -> If there is any "\n" or "\r" remove it!
[Debugging] -> Show debugging informations
[deltime] -> How long to wait after the "Done." response -> Normal is 500

