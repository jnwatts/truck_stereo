.PHONY: build
all: build


IDF_PORT ?= /dev/ttyUSB0

build:
	mkdir -p build
	cd build; \
		/usr/bin/cmake ..
	+$(MAKE) -C build

menuconfig:
	idf.py $@

flash monitor:
	idf.py -p $(IDF_PORT) $@

clean:
	rm -Rf build

