CC=g++
CFLAGS=-std=c++11
GLFLAGS=-lGLEW -lGL -lglut

gg: gg.cpp
	$(CC) gg.cpp -o gg -O2 $(CFLAGS) $(GLFLAGS)

opt: gg.cpp
	$(CC) gg.cpp -o gg -O2 $(CFLAGS) $(GLFLAGS)

debug: gg.cpp
	$(CC) gg.cpp -o gg -g $(CFLAGS) $(GLFLAGS)
