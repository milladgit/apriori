
CFLAGS = -std=c++11 -O3 -ftree-vectorize -funroll-loops -ffast-math -finline-functions


all:
	g++ $(CFLAGS) apriori.cpp -o apriori

clean:
	rm apriori

