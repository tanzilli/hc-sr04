import os

iopath="/sys/class/hcsr04/value"

if os.path.exists(iopath): 
	f = open(iopath + '/value','r')
	d=f.read()
	f.close()
	
	if (long(d)==-1):
		print "N.A."
	else:	
		print "%ld cm" % (long(d)/58) 
else:
	print "Driver module not loaded"
