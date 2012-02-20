all:
	g++ cocor/*.cpp *.cpp -o Lua $(CFLAGS)

clean:
	rm -f Lua

