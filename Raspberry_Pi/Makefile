prefix := /usr/local
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -l json -l rf24-bcm -std=c++11

# define all programs
PROGRAMS = reciever
SOURCES = ${PROGRAMS:=.cpp}



all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	g++ -o $@ -Wall -I../ $@.cpp ${CCFLAGS} 

clean:
	rm -rf $(PROGRAMS)

