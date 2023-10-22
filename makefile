JustDoIt_webserver: main.o
	g++ -o JustDoIt_webserver main.o -std=c++11

main.o: main.cpp echo.h processpool.h
	g++ -c main.cpp -std=c++11

clean:
	rm main.o
	rm JustDoIt_webserver