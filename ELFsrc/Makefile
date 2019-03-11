CC := gcc
CFLAGS := -g -Wall
BIN_NAME := infector
OBJS := infector.o fullnelson.o

all: $(BIN_NAME)

$(BIN_NAME): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(BIN_NAME)

clean:
	rm -f *.o $(BIN_NAME)

tool:
	cd ./Tools && make 
virus:
	cd ./virusBody && make
cleantool:
	cd ./Tools && make clean
cleanvirus:	
	cd ./virusBody && make clean
