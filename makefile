all: compile

compile:
	gcc -Wall -Wextra -std=c99 -O2 defer.c -o defer

clean:
	rm defer
