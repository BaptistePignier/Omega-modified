import sys,os

num = int(sys.argv[-1])
if num == 0:
	os.system('make MODEL=n0100 EPSILON_VERSION=13.0.0 PLATFORM=simulator clean')
	os.system('make MODEL=n0100 EPSILON_VERSION=13.0.0 PLATFORM=simulator')
	os.system('output/release/simulator/linux/epsilon.bin')

if num == 1:
	os.system('make MODEL=n0100 EPSILON_VERSION=13.0.0 clean')
	os.system('make MODEL=n0100 EPSILON_VERSION=13.0.0 epsilon_flash')