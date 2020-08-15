CXXFLAGS=-O3 -Wall -std=c++11
TARGET=find24
$(TARGET) : main.o find24_simple.o find24.o expr.o
	$(CXX) $^ -o $@
clean :
	rm *.o $(TARGET)
