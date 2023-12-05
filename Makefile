all : MinHeap.cpp ThreadPool.cpp test.cpp
	g++ -o test MinHeap.cpp ThreadPool.cpp test.cpp -pthread

clean :
	rm test *-* data/*.txt_out out.txt