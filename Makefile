build:
	g++ -Wall -Werror map-reduce.cpp -o mapReduce -lpthread

clean:
	rm -f mapReduce
