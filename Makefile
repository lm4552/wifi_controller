
.PHONY: all web certs compile upload serial clean clean_web clean_certs

all: web certs compile upload
	
web:
	cd ./src/web/ && make

certs:
	- cd ./certs && make

compile:
	arduino-cli compile -b esp32:esp32:esp32 wifi_controller.ino 

upload:
	arduino-cli upload -b esp32:esp32:esp32 -p /dev/ttyUSB0 wifi_controller.ino 

serial:
	screen /dev/ttyUSB0 115200

clean: clean_web clean_certs

clean_web:
	cd ./src/web/ && make clean

clean_certs:
	cd ./certs && make clean
