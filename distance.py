f = open("/sys/class/hcsr04/value",'r')
d=f.read()
f.close()

if (long(d)==-1):
	print "N.A."
else:	
	print "%.1f cm" % (float(d)/58) 
