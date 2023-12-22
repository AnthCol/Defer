CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -O2

all: compile

compile:
	$(CC) $(CFLAGS) defer.cpp -o defer

clean:
	rm defer
