all: prog

prog: *.cpp *.o
		g++ -c main.cpp -lGLU -lGL -lglut

		g++ -c ObstacleRunner.cpp ObstacleRunner.h -lglut -lGL -lGLU

		g++ -o prog ObstacleRunner.o main.o -lGL -lGLU -lglut